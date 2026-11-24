#include "fs.h"
#include "shell/builtins/builtins.h"

#include "driver/keyboard/event_queue.h"
#include "driver/keyboard/keyboard_keys.h"

#include "mm/heap.h"
#include "stdio.h"
#include "string.h"
#include "vga.h"

#define ED_TEXT_ROWS    23 /* text area: screen rows 0..22 */
#define ED_MSG_ROW      23 /* transient messages */
#define ED_STATUS_ROW   24
#define ED_SCREEN_WIDTH 80

#define ED_MAX_LINES    128
#define ED_MAX_LINE_LEN 248

#define ED_ATTR_NORMAL ((uint8_t)(DEFUALT_F | (DEFUALT_B << 4)))
#define ED_ATTR_STATUS ((uint8_t)(BLACK | (LIGHT_GRAY << 4)))
#define ED_ATTR_MSG    ((uint8_t)(CYAN | (BLACK << 4)))

typedef struct {
    char chars[ED_MAX_LINE_LEN + 1];
    size_t length;
} ed_line_t;

typedef struct {
    ed_line_t lines[ED_MAX_LINES];
    size_t line_count;

    size_t cy;  /* cursor line index */
    size_t cx;  /* cursor column within the line */
    size_t top; /* first line shown at text row 0 */

    bool dirty;
    bool quit_pending; /* awaiting a second Ctrl+C to discard changes */
    bool quit_requested;

    char filename[SHELL_PATH_SIZE];
    char message[128];
} ed_t;

static void ed_clamp_cursor(ed_t *e) {
    if (e->cy >= e->line_count)
        e->cy = e->line_count - 1;

    if (e->cx > e->lines[e->cy].length)
        e->cx = e->lines[e->cy].length;

    if (e->cy < e->top)
        e->top = e->cy;

    if (e->cy >= e->top + ED_TEXT_ROWS)
        e->top = e->cy - ED_TEXT_ROWS + 1;
}

static void ed_message(ed_t *e, const char *text) {
    size_t i;

    for (i = 0; i < sizeof(e->message) - 1 && text[i] != '\0'; i++)
        e->message[i] = text[i];
    e->message[i] = '\0';
}

static void ed_init(ed_t *e, const char *filename) {
    size_t i;

    for (i = 0; i < sizeof(e->filename) - 1 && filename[i] != '\0'; i++)
        e->filename[i] = filename[i];
    e->filename[i] = '\0';

    e->line_count = 1;
    e->lines[0].chars[0] = '\0';
    e->lines[0].length = 0;

    e->cy = 0;
    e->cx = 0;
    e->top = 0;
    e->dirty = false;
    e->quit_pending = false;
    e->quit_requested = false;
    e->message[0] = '\0';
}

/* Load an existing file into the buffer. Missing file -> empty buffer. */
static void ed_load(ed_t *e, const char *path) {
    static char file_buffer[4096];
    fs_node_t *node;
    size_t size;
    size_t i;
    size_t li; /* current line index */
    size_t ci; /* current char index within the line */

    node = fs_lookup(path);

    e->line_count = 1;
    e->lines[0].chars[0] = '\0';
    e->lines[0].length = 0;
    li = 0;
    ci = 0;

    if (node != NULL && node->type == FS_FILE && node->size > 0) {
        size = node->size > sizeof(file_buffer) - 1 ? sizeof(file_buffer) - 1 : node->size;
        fs_read(node, file_buffer, size, 0);

        for (i = 0; i < size; i++) {
            char c = file_buffer[i];

            if (c == '\n') {
                e->lines[li].length = ci;
                ci = 0;
                li++;
                if (li >= ED_MAX_LINES)
                    break;
                e->line_count = li + 1;
            } else if (ci < ED_MAX_LINE_LEN) {
                e->lines[li].chars[ci++] = c;
            }
        }

        e->lines[li].length = ci;

        if (li >= ED_MAX_LINES)
            e->line_count = ED_MAX_LINES;
    }

    e->cy = 0;
    e->cx = 0;
    e->top = 0;
}

static void ed_insert_char(ed_t *e, char c) {
    ed_line_t *line = &e->lines[e->cy];

    if (line->length >= ED_MAX_LINE_LEN)
        return;

    for (size_t i = line->length; i > e->cx; i--)
        line->chars[i] = line->chars[i - 1];

    line->chars[e->cx] = c;
    line->length++;
    e->cx++;
    e->dirty = true;
}

static void ed_insert_newline(ed_t *e) {
    ed_line_t *line;
    ed_line_t *next;
    size_t remainder;
    size_t i;

    if (e->line_count >= ED_MAX_LINES)
        return;

    line = &e->lines[e->cy];
    remainder = line->length - e->cx;

    for (i = e->line_count; i > e->cy + 1; i--)
        e->lines[i] = e->lines[i - 1];

    next = &e->lines[e->cy + 1];
    for (i = 0; i < remainder; i++)
        next->chars[i] = line->chars[e->cx + i];
    next->length = remainder;

    line->length = e->cx;
    e->line_count++;
    e->cy++;
    e->cx = 0;
    e->dirty = true;
}

static void ed_backspace(ed_t *e) {
    ed_line_t *line;
    ed_line_t *pline;
    ed_line_t *cline;
    size_t prev;
    size_t i;

    if (e->cx > 0) {
        line = &e->lines[e->cy];
        for (i = e->cx - 1; i + 1 < line->length; i++)
            line->chars[i] = line->chars[i + 1];
        line->length--;
        e->cx--;
        e->dirty = true;
        return;
    }

    if (e->cy == 0)
        return;

    prev = e->cy - 1;
    pline = &e->lines[prev];
    cline = &e->lines[e->cy];

    if (pline->length + cline->length > ED_MAX_LINE_LEN)
        return;

    for (i = 0; i < cline->length; i++)
        pline->chars[pline->length + i] = cline->chars[i];
    pline->length += cline->length;

    for (i = e->cy + 1; i < e->line_count; i++)
        e->lines[i - 1] = e->lines[i];

    e->line_count--;
    e->cy = prev;
    e->cx = pline->length;
    e->dirty = true;
}

static void ed_delete(ed_t *e) {
    ed_line_t *line;
    ed_line_t *next;
    size_t i;

    line = &e->lines[e->cy];

    if (e->cx < line->length) {
        for (i = e->cx; i + 1 < line->length; i++)
            line->chars[i] = line->chars[i + 1];
        line->length--;
        e->dirty = true;
        return;
    }

    if (e->cy + 1 >= e->line_count)
        return;

    next = &e->lines[e->cy + 1];

    if (line->length + next->length > ED_MAX_LINE_LEN)
        return;

    for (i = 0; i < next->length; i++)
        line->chars[line->length + i] = next->chars[i];
    line->length += next->length;

    for (i = e->cy + 2; i < e->line_count; i++)
        e->lines[i - 1] = e->lines[i];

    e->line_count--;
    e->dirty = true;
}

static void ed_save(ed_t *e) {
    fs_node_t *node;
    char *data;
    size_t total;
    size_t offset;
    size_t i;
    size_t j;

    node = fs_lookup(e->filename);
    if (node == NULL) {
        if (fs_create(e->filename) < 0) {
            ed_message(e, "Error: could not create file");
            return;
        }
        node = fs_lookup(e->filename);
        if (node == NULL) {
            ed_message(e, "Error: could not create file");
            return;
        }
    }

    total = 0;
    for (i = 0; i < e->line_count; i++)
        total += e->lines[i].length + 1; /* line contents + '\n' */

    data = malloc(total);
    if (data == NULL) {
        ed_message(e, "Error: out of memory");
        return;
    }

    offset = 0;
    for (i = 0; i < e->line_count; i++) {
        for (j = 0; j < e->lines[i].length; j++)
            data[offset++] = e->lines[i].chars[j];
        data[offset++] = '\n';
    }

    if (node->data != NULL)
        free(node->data);
    node->data = data;
    node->size = total;

    e->dirty = false;
    ed_message(e, "Saved");
}

static void ed_cmd_quit(ed_t *e) {
    if (e->dirty && !e->quit_pending) {
        e->quit_pending = true;
        ed_message(e, "Unsaved changes. Ctrl+C again to exit, Ctrl+S to save");
        return;
    }
    e->quit_requested = true;
}

static void ed_handle_event(ed_t *e, const input_event_t *event) {
    if (event->type == INPUT_EVENT_CHAR) {
        char c = event->character;

        /*
         * The keyboard driver emits one CHAR event on press and another on
         * release for printable keys (keyboard_translate ignores the action).
         * Only act on the press so each character is inserted exactly once.
         */
        if (event->action != KEY_PRESSED)
            return;

        if (event->ctrl) {
            if (c == 's') {
                ed_save(e);
                return;
            }
            if (c == 'c') {
                ed_cmd_quit(e);
                return;
            }
            return; /* ignore other control combinations */
        }

        if (e->quit_pending)
            e->quit_pending = false;

        if (c >= 32 && c < 127)
            ed_insert_char(e, c);
        return;
    }

    if (event->action != KEY_PRESSED)
        return;

    switch (event->key) {
    case KEY_ENTER:
        ed_insert_newline(e);
        break;

    case KEY_BACKSPACE:
        ed_backspace(e);
        break;

    case KEY_DELETE:
        ed_delete(e);
        break;

    case KEY_LEFT:
        if (e->cx > 0)
            e->cx--;
        break;

    case KEY_RIGHT:
        if (e->cx < e->lines[e->cy].length)
            e->cx++;
        break;

    case KEY_UP:
        if (e->cy > 0) {
            e->cy--;
            if (e->cx > e->lines[e->cy].length)
                e->cx = e->lines[e->cy].length;
        }
        break;

    case KEY_DOWN:
        if (e->cy + 1 < e->line_count) {
            e->cy++;
            if (e->cx > e->lines[e->cy].length)
                e->cx = e->lines[e->cy].length;
        }
        break;

    case KEY_HOME:
        e->cx = 0;
        break;

    case KEY_END:
        e->cx = e->lines[e->cy].length;
        break;

    case KEY_PAGE_UP:
        if (e->top >= ED_TEXT_ROWS)
            e->top -= ED_TEXT_ROWS;
        else
            e->top = 0;
        if (e->cy < e->top) {
            e->cy = e->top;
            e->cx = 0;
        }
        break;

    case KEY_PAGE_DOWN:
        if (e->top + ED_TEXT_ROWS < e->line_count)
            e->top += ED_TEXT_ROWS;
        if (e->cy >= e->top + ED_TEXT_ROWS) {
            e->cy = e->top + ED_TEXT_ROWS - 1;
            e->cx = 0;
        }
        break;

    case KEY_ESC:
        ed_cmd_quit(e);
        break;

    case KEY_TAB:
        for (int i = 0; i < 4; i++)
            ed_insert_char(e, ' ');
        break;

    default:
        break;
    }
}

static void ed_render(const ed_t *e) {
    size_t row;
    size_t x;

    for (row = 0; row < ED_TEXT_ROWS; row++) {
        size_t index = e->top + row;
        size_t len;

        vga_set_cursor((uint8_t)0, (uint8_t)row);

        for (x = 0; x < ED_SCREEN_WIDTH; x++)
            vga_put_at((uint8_t)x, (uint8_t)row, ' ', ED_ATTR_NORMAL);

        if (index < e->line_count) {
            const ed_line_t *line = &e->lines[index];

            len = line->length > ED_SCREEN_WIDTH ? ED_SCREEN_WIDTH : line->length;
            for (x = 0; x < len; x++)
                vga_put_at((uint8_t)x, (uint8_t)row, line->chars[x], ED_ATTR_NORMAL);
        } else {
            vga_put_at((uint8_t)0, (uint8_t)row, '~', ED_ATTR_NORMAL);
        }
    }

    /* message row */
    for (x = 0; x < ED_SCREEN_WIDTH; x++)
        vga_put_at((uint8_t)x, ED_MSG_ROW, ' ', ED_ATTR_MSG);
    if (e->message[0] != '\0')
        vga_print_at(0, ED_MSG_ROW, e->message, ED_ATTR_MSG);

    /* status row: filename on the left, hints on the right */
    for (x = 0; x < ED_SCREEN_WIDTH; x++)
        vga_put_at((uint8_t)x, ED_STATUS_ROW, ' ', ED_ATTR_STATUS);

    {
        const char *hints = "^S Save   ^C Exit";
        size_t hint_len = 0;

        while (hints[hint_len] != '\0')
            hint_len++;

        for (x = 0; x + hint_len < ED_SCREEN_WIDTH && e->filename[x] != '\0'; x++)
            vga_put_at((uint8_t)x, ED_STATUS_ROW, e->filename[x], ED_ATTR_STATUS);

        vga_print_at((uint8_t)(ED_SCREEN_WIDTH - hint_len), ED_STATUS_ROW, hints, ED_ATTR_STATUS);
    }

    /* hardware cursor */
    {
        uint8_t cx = e->cx >= ED_SCREEN_WIDTH ? ED_SCREEN_WIDTH - 1 : (uint8_t)e->cx;

        vga_set_cursor(cx, (uint8_t)(e->cy - e->top));
    }
}

int shell_builtin_ed(shell_context_t *context, int argc, char *argv[]) {
    static ed_t editor;
    char resolved[SHELL_PATH_SIZE];
    input_event_t event;

    (void)context;

    if (argc < 2 || argv[1] == NULL || argv[1][0] == '\0') {
        printf("ed: missing operand\n");
        printf("usage: ed <file>\n");
        return -1;
    }

    shell_resolve_path(context, argv[1], resolved, sizeof(resolved));

    ed_init(&editor, resolved);
    ed_load(&editor, resolved);

    vga_clear();
    vga_set_color(DEFUALT_F, DEFUALT_B);
    vga_cursor_enable();

    ed_render(&editor);

    while (!editor.quit_requested) {
        /* Wait for a key event from the keyboard IRQ handler. */
        while (!input_queue_pop(&event))
            ;

        ed_handle_event(&editor, &event);
        ed_clamp_cursor(&editor);
        ed_render(&editor);
    }

    /* Restore a clean screen for the shell. */
    vga_set_color(DEFUALT_F, DEFUALT_B);
    vga_clear();
    vga_set_cursor(0, 0);

    return 0;
}
