/*
 * aphelion_styling.c
 *
 * Portable C99 implementation of the Aphelion styling primitives. See
 * aphelion_styling.h for the public contract. The QSS emitted here is a
 * direct, line-for-line port of the Python package:
 *
 *   theme_tokens.py     -> token storage + built-in presets
 *   theme_engine.py     -> styling_build_{main,dock,menubar,context_menu,
 *                                     timeline,preferences}
 *   property_style.py   -> styling_build_properties
 *   theme.py            -> the static named QSS catalog
 *   component_styles.py -> styling_{selection_bar,custom_node_dialog}_style
 */

/* When this file is added straight into a consuming project (no build-system
 * define), treat the build as static so the symbols below are definitions,
 * not ``dllimport`` redeclarations. CMake sets STYLING_BUILD / STYLING_STATIC
 * explicitly. */
#if defined(_WIN32) && !defined(STYLING_BUILD) && !defined(STYLING_STATIC)
#define STYLING_STATIC
#endif

#include "aphelion_styling.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Minimal growing string builder                                     */
/* ------------------------------------------------------------------ */

typedef struct {
    char *buf;
    size_t cap;      /* total capacity, including room for the NUL */
    size_t len;      /* bytes written so far (excluding the NUL)    */
    size_t required; /* bytes that would be needed without a cap    */
    int overflow;    /* set once the buffer fills up                */
} sb_t;

static void sb_init(sb_t *sb, char *buf, size_t cap)
{
    sb->buf = buf;
    sb->cap = cap;
    sb->len = 0;
    sb->required = 0;
    sb->overflow = 0;
    if (cap > 0 && buf != NULL) {
        buf[0] = '\0';
    }
}

#if defined(__GNUC__) || defined(__clang__)
static void sb_cat(sb_t *sb, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#endif

static void sb_cat(sb_t *sb, const char *fmt, ...)
{
    va_list ap;
    int needed;
    size_t remaining;

    va_start(ap, fmt);
    if (sb->overflow) {
        /* Buffer already full: still measure, do not write. */
        needed = vsnprintf(NULL, 0, fmt, ap);
        va_end(ap);
        if (needed > 0) {
            sb->required += (size_t)needed;
        }
        return;
    }

    remaining = sb->cap - sb->len;
    needed = vsnprintf(sb->buf + sb->len, remaining, fmt, ap);
    va_end(ap);

    if (needed < 0) {
        sb->overflow = 1;
        return;
    }
    if ((size_t)needed >= remaining) {
        sb->overflow = 1;
        sb->len = (sb->cap > 0) ? sb->cap - 1 : 0;
        sb->required += (size_t)needed;
        return;
    }
    sb->len += (size_t)needed;
    sb->required += (size_t)needed;
}

static int sb_finish(const sb_t *sb)
{
    if (sb->overflow) {
        return (int)sb->required;
    }
    return (int)sb->len;
}

/* ------------------------------------------------------------------ */
/* Token initialisation                                               */
/* ------------------------------------------------------------------ */

static void styling_defaults(styling_tokens *t)
{
    memset(t, 0, sizeof(*t));

    strcpy(t->theme_id, "aphelion_dark");
    strcpy(t->display_name, "Aphelion Dark");

    strcpy(t->window_bg, "#1e1e1e");
    strcpy(t->panel_bg, "#1a1a1a");
    strcpy(t->surface_bg, "#252525");
    strcpy(t->surface_elevated, "#2a2a2a");
    strcpy(t->text_primary, "#ffffff");
    strcpy(t->text_secondary, "#c8c8c8");
    strcpy(t->text_muted, "#8a8a94");
    strcpy(t->border, "#333333");
    strcpy(t->border_subtle, "#121212");
    strcpy(t->accent, "#2b6ea8");
    strcpy(t->accent_hover, "#347ebc");
    strcpy(t->accent_pressed, "#245f91");
    strcpy(t->accent_text, "#9ecfff");
    strcpy(t->menu_bg, "#2a2a2a");
    strcpy(t->menu_selected, "#2b6ea8");
    strcpy(t->scrollbar_track, "#2a2a2a");
    strcpy(t->scrollbar_handle, "#555555");

    t->graph_bg_rgb[0] = 24; t->graph_bg_rgb[1] = 24; t->graph_bg_rgb[2] = 26;
    t->selection_rgb[0] = 0; t->selection_rgb[1] = 150; t->selection_rgb[2] = 255;
    t->socket_input_rgb[0] = 80; t->socket_input_rgb[1] = 160; t->socket_input_rgb[2] = 255;
    t->socket_output_rgb[0] = 255; t->socket_output_rgb[1] = 160; t->socket_output_rgb[2] = 80;
    t->wire_active_rgb[0] = 0; t->wire_active_rgb[1] = 120; t->wire_active_rgb[2] = 220;
    t->node_body_rgb[0] = 38; t->node_body_rgb[1] = 38; t->node_body_rgb[2] = 42;
    t->node_body_hover_rgb[0] = 46; t->node_body_hover_rgb[1] = 46; t->node_body_hover_rgb[2] = 52;
    t->node_border_rgb[0] = 70; t->node_border_rgb[1] = 70; t->node_border_rgb[2] = 78;

    t->node_color_count = 0;
}

void styling_aphelion_dark(styling_tokens *out)
{
    styling_defaults(out);
}

void styling_midnight_blue(styling_tokens *out)
{
    styling_defaults(out);

    strcpy(out->theme_id, "midnight_blue");
    strcpy(out->display_name, "Midnight Blue");
    strcpy(out->window_bg, "#141820");
    strcpy(out->panel_bg, "#161922");
    strcpy(out->surface_bg, "#1c2230");
    strcpy(out->surface_elevated, "#222a3a");
    strcpy(out->text_primary, "#eef2ff");
    strcpy(out->text_secondary, "#b8c0d8");
    strcpy(out->text_muted, "#7a849c");
    strcpy(out->border, "#2a3348");
    strcpy(out->border_subtle, "#0e121a");
    strcpy(out->accent, "#3a7bd5");
    strcpy(out->accent_hover, "#4a8be5");
    strcpy(out->accent_pressed, "#2f6bbf");
    strcpy(out->accent_text, "#a8c8ff");
    strcpy(out->menu_bg, "#1a2030");
    strcpy(out->menu_selected, "#3a7bd5");
    strcpy(out->scrollbar_track, "#1a2030");
    strcpy(out->scrollbar_handle, "#4a5568");

    out->graph_bg_rgb[0] = 18; out->graph_bg_rgb[1] = 22; out->graph_bg_rgb[2] = 32;
    out->selection_rgb[0] = 58; out->selection_rgb[1] = 123; out->selection_rgb[2] = 213;
    out->socket_input_rgb[0] = 96; out->socket_input_rgb[1] = 168; out->socket_input_rgb[2] = 255;
    out->socket_output_rgb[0] = 255; out->socket_output_rgb[1] = 176; out->socket_output_rgb[2] = 96;
    out->wire_active_rgb[0] = 58; out->wire_active_rgb[1] = 123; out->wire_active_rgb[2] = 213;
    out->node_body_rgb[0] = 34; out->node_body_rgb[1] = 40; out->node_body_rgb[2] = 54;
    out->node_body_hover_rgb[0] = 42; out->node_body_hover_rgb[1] = 50; out->node_body_hover_rgb[2] = 66;
    out->node_border_rgb[0] = 64; out->node_border_rgb[1] = 76; out->node_border_rgb[2] = 98;
}

void styling_graphite(styling_tokens *out)
{
    styling_defaults(out);

    strcpy(out->theme_id, "graphite");
    strcpy(out->display_name, "Graphite");
    strcpy(out->window_bg, "#181818");
    strcpy(out->panel_bg, "#1c1c1c");
    strcpy(out->surface_bg, "#262626");
    strcpy(out->surface_elevated, "#2c2c2c");
    strcpy(out->text_primary, "#f2f2f2");
    strcpy(out->text_secondary, "#c4c4c4");
    strcpy(out->text_muted, "#888888");
    strcpy(out->border, "#3a3a3a");
    strcpy(out->border_subtle, "#101010");
    strcpy(out->accent, "#6b8cae");
    strcpy(out->accent_hover, "#7a9abe");
    strcpy(out->accent_pressed, "#5a7c9e");
    strcpy(out->accent_text, "#b8cce0");
    strcpy(out->menu_bg, "#242424");
    strcpy(out->menu_selected, "#6b8cae");
    strcpy(out->scrollbar_track, "#242424");
    strcpy(out->scrollbar_handle, "#5a5a5a");

    out->graph_bg_rgb[0] = 22; out->graph_bg_rgb[1] = 22; out->graph_bg_rgb[2] = 24;
    out->selection_rgb[0] = 107; out->selection_rgb[1] = 140; out->selection_rgb[2] = 174;
    out->socket_input_rgb[0] = 120; out->socket_input_rgb[1] = 160; out->socket_input_rgb[2] = 200;
    out->socket_output_rgb[0] = 210; out->socket_output_rgb[1] = 170; out->socket_output_rgb[2] = 120;
    out->wire_active_rgb[0] = 107; out->wire_active_rgb[1] = 140; out->wire_active_rgb[2] = 174;
    out->node_body_rgb[0] = 40; out->node_body_rgb[1] = 40; out->node_body_rgb[2] = 44;
    out->node_body_hover_rgb[0] = 48; out->node_body_hover_rgb[1] = 48; out->node_body_hover_rgb[2] = 54;
    out->node_border_rgb[0] = 78; out->node_border_rgb[1] = 78; out->node_border_rgb[2] = 86;
}

void styling_warm_studio(styling_tokens *out)
{
    styling_defaults(out);

    strcpy(out->theme_id, "warm_studio");
    strcpy(out->display_name, "Warm Studio");
    strcpy(out->window_bg, "#1c1816");
    strcpy(out->panel_bg, "#201c18");
    strcpy(out->surface_bg, "#2a2420");
    strcpy(out->surface_elevated, "#322c28");
    strcpy(out->text_primary, "#f6f0ea");
    strcpy(out->text_secondary, "#d8cdc0");
    strcpy(out->text_muted, "#9a8e82");
    strcpy(out->border, "#3a342e");
    strcpy(out->border_subtle, "#120f0d");
    strcpy(out->accent, "#c4844a");
    strcpy(out->accent_hover, "#d4945a");
    strcpy(out->accent_pressed, "#a8743a");
    strcpy(out->accent_text, "#f0c898");
    strcpy(out->menu_bg, "#2a2420");
    strcpy(out->menu_selected, "#c4844a");
    strcpy(out->scrollbar_track, "#2a2420");
    strcpy(out->scrollbar_handle, "#6a5a4a");

    out->graph_bg_rgb[0] = 28; out->graph_bg_rgb[1] = 24; out->graph_bg_rgb[2] = 20;
    out->selection_rgb[0] = 196; out->selection_rgb[1] = 132; out->selection_rgb[2] = 74;
    out->socket_input_rgb[0] = 120; out->socket_input_rgb[1] = 170; out->socket_input_rgb[2] = 220;
    out->socket_output_rgb[0] = 240; out->socket_output_rgb[1] = 170; out->socket_output_rgb[2] = 90;
    out->wire_active_rgb[0] = 196; out->wire_active_rgb[1] = 132; out->wire_active_rgb[2] = 74;
    out->node_body_rgb[0] = 44; out->node_body_rgb[1] = 38; out->node_body_rgb[2] = 34;
    out->node_body_hover_rgb[0] = 52; out->node_body_hover_rgb[1] = 46; out->node_body_hover_rgb[2] = 40;
    out->node_border_rgb[0] = 88; out->node_border_rgb[1] = 76; out->node_border_rgb[2] = 66;
}

/* ------------------------------------------------------------------ */
/* Runtime QSS builders                                               */
/* ------------------------------------------------------------------ */

int styling_build_main(const styling_tokens *t, char *out, size_t cap)
{
    sb_t sb;
    sb_init(&sb, out, cap);

    sb_cat(&sb,
        "\n"
        "    QMainWindow {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "    }\n"
        "    QLabel#StatusKeyHints {\n"
        "        color: %s;\n"
        "        font-size: 11px;\n"
        "        padding-right: 8px;\n"
        "    }\n"
        "    QMainWindow::separator {\n"
        "        background-color: %s;\n"
        "        width: 1px;\n"
        "        height: 1px;\n"
        "    }\n"
        "    QDockWidget {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-right: 1px solid %s;\n"
        "        border-bottom: 1px solid %s;\n"
        "        titlebar-close-icon: url(none);\n"
        "        titlebar-normal-icon: url(none);\n"
        "    }\n"
        "    QDockWidget::title {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        padding: 1px 8px;\n"
        "        border: none;\n"
        "        border-bottom: 1px solid %s;\n"
        "        border-top: 1px solid %s;\n"
        "        font-weight: 600;\n"
        "        font-size: 10px;\n"
        "        text-transform: uppercase;\n"
        "    }\n"
        "    QDockWidget::close-button, QDockWidget::float-button {\n"
        "        background-color: transparent;\n"
        "        border: none;\n"
        "        padding: 0px;\n"
        "        margin: 1px 2px 0px 0px;\n"
        "        width: 12px;\n"
        "        height: 12px;\n"
        "    }\n"
        "    QDockWidget::close-button:hover, QDockWidget::float-button:hover {\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QScrollBar:vertical {\n"
        "        width: 12px;\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QScrollBar::handle:vertical {\n"
        "        background-color: %s;\n"
        "        border-radius: 6px;\n"
        "        min-height: 20px;\n"
        "    }\n"
        "    QScrollBar::handle:vertical:hover {\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
        "        border: none;\n"
        "        background: none;\n"
        "    }\n"
        "    ",
        t->window_bg, t->text_primary, t->text_muted, t->border_subtle,
        t->panel_bg, t->text_primary, t->border_subtle, t->border, t->border,
        t->surface_bg, t->text_secondary, t->border_subtle, t->border,
        t->surface_elevated, t->scrollbar_track, t->scrollbar_handle,
        t->text_muted);

    return sb_finish(&sb);
}

int styling_build_dock(const styling_tokens *t, char *out, size_t cap)
{
    sb_t sb;
    sb_init(&sb, out, cap);

    sb_cat(&sb,
        "\n"
        "    QDockWidget {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-right: 1px solid %s;\n"
        "        border-bottom: 1px solid %s;\n"
        "    }\n"
        "    QDockWidget::title {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        padding: 1px 8px;\n"
        "        border: none;\n"
        "        border-top: 1px solid %s;\n"
        "        border-bottom: 1px solid %s;\n"
        "        font-weight: 600;\n"
        "        font-size: 10px;\n"
        "    }\n"
        "    QDockWidget > QWidget {\n"
        "        background-color: %s;\n"
        "        border: none;\n"
        "        border-top: 1px solid %s;\n"
        "    }\n"
        "    ",
        t->panel_bg, t->text_secondary, t->border_subtle, t->border, t->border,
        t->surface_bg, t->text_secondary, t->border, t->border_subtle,
        t->panel_bg, t->border_subtle);

    return sb_finish(&sb);
}

int styling_build_menubar(const styling_tokens *t, char *out, size_t cap)
{
    sb_t sb;
    sb_init(&sb, out, cap);

    sb_cat(&sb,
        "\n"
        "    QMenuBar {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border-bottom: 1px solid %s;\n"
        "        spacing: 4px;\n"
        "        padding: 4px 10px;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QMenuBar::item {\n"
        "        background-color: transparent;\n"
        "        color: %s;\n"
        "        padding: 5px 12px;\n"
        "        border-radius: 4px;\n"
        "        font-weight: 500;\n"
        "    }\n"
        "    QMenuBar::item:selected {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "    }\n"
        "    QMenuBar::item:pressed {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "    }\n"
        "    ",
        t->window_bg, t->text_secondary, t->border_subtle,
        t->text_secondary, t->accent, t->text_primary,
        t->accent_pressed, t->text_primary);

    return sb_finish(&sb);
}

int styling_build_context_menu(const styling_tokens *t, char *out, size_t cap)
{
    sb_t sb;
    sb_init(&sb, out, cap);

    sb_cat(&sb,
        "\n"
        "    QMenu {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        padding: 4px 0px;\n"
        "    }\n"
        "    QMenu::item {\n"
        "        padding: 6px 24px 6px 16px;\n"
        "    }\n"
        "    QMenu::item:selected {\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QMenu::separator {\n"
        "        background-color: %s;\n"
        "        height: 1px;\n"
        "        margin: 4px 8px;\n"
        "    }\n"
        "    ",
        t->menu_bg, t->text_primary, t->border, t->menu_selected, t->border);

    return sb_finish(&sb);
}

int styling_build_timeline(const styling_tokens *t, char *out, size_t cap)
{
    sb_t sb;
    sb_init(&sb, out, cap);

    sb_cat(&sb,
        "\n"
        "    TimelineWidget {\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QLabel#TimelineTimecodeLabel {\n"
        "        color: %s;\n"
        "        font-family: \"Cascadia Mono\", \"Consolas\", monospace;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        padding: 1px 6px;\n"
        "        background-color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 3px;\n"
        "        min-width: 84px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QLabel#TimelineFrameLabel {\n"
        "        color: %s;\n"
        "        font-family: \"Cascadia Mono\", \"Consolas\", monospace;\n"
        "        font-size: 10px;\n"
        "        padding: 1px 4px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QPushButton#TimelineTransportButton,\n"
        "    QPushButton#TimelinePlayButton,\n"
        "    QPushButton#TimelineToggleButton {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 3px;\n"
        "        padding: 2px;\n"
        "        min-height: 22px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QPushButton#TimelineTransportButton:hover,\n"
        "    QPushButton#TimelinePlayButton:hover,\n"
        "    QPushButton#TimelineToggleButton:hover {\n"
        "        background-color: %s;\n"
        "        border-color: %s;\n"
        "    }\n"
        "    QPushButton#TimelinePlayButton[playing=\"true\"] {\n"
        "        background-color: %s;\n"
        "        border-color: %s;\n"
        "    }\n"
        "    QComboBox#TimelineSpeedCombo {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 3px;\n"
        "        padding: 1px 6px;\n"
        "        min-height: 22px;\n"
        "        max-height: 22px;\n"
        "        font-size: 10px;\n"
        "    }\n"
        "    QComboBox#TimelineSpeedCombo QAbstractItemView {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        selection-background-color: %s;\n"
        "    }\n"
        "    QFrame#TimelineToolbar {\n"
        "        background-color: %s;\n"
        "        border: none;\n"
        "        border-bottom: 1px solid %s;\n"
        "    }\n"
        "    ",
        t->panel_bg, t->text_secondary, t->border_subtle, t->border,
        t->text_muted, t->surface_bg, t->text_secondary, t->border,
        t->surface_elevated, t->text_muted, t->accent_pressed, t->accent,
        t->surface_bg, t->text_secondary, t->border, t->window_bg,
        t->text_secondary, t->border, t->accent, t->panel_bg, t->border_subtle);

    return sb_finish(&sb);
}

int styling_build_preferences(const styling_tokens *t, char *out, size_t cap)
{
    sb_t sb;
    sb_init(&sb, out, cap);

    sb_cat(&sb,
        "\n"
        "    QDialog#PreferencesDialog {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "    }\n"
        "    QLabel#PreferencesTitle {\n"
        "        color: %s;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QTabWidget#PreferencesTabs::pane {\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 6px;\n"
        "        background-color: %s;\n"
        "        top: -1px;\n"
        "    }\n"
        "    QTabWidget#PreferencesTabs QTabBar::tab {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-bottom: none;\n"
        "        padding: 8px 16px;\n"
        "        margin-right: 2px;\n"
        "        border-top-left-radius: 5px;\n"
        "        border-top-right-radius: 5px;\n"
        "    }\n"
        "    QTabWidget#PreferencesTabs QTabBar::tab:selected {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border-bottom: 1px solid %s;\n"
        "    }\n"
        "    QGroupBox#PreferencesGroup {\n"
        "        color: %s;\n"
        "        font-weight: 600;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 6px;\n"
        "        margin-top: 10px;\n"
        "        padding-top: 14px;\n"
        "    }\n"
        "    QGroupBox#PreferencesGroup::title {\n"
        "        subcontrol-origin: margin;\n"
        "        left: 10px;\n"
        "        padding: 0 4px;\n"
        "    }\n"
        "    QLabel#PreferencesHint {\n"
        "        color: %s;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QLabel#PreferencesPath,\n"
        "    QLabel#PreferencesStatus {\n"
        "        color: %s;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QLineEdit#KeyCaptureField,\n"
        "    QSpinBox#PreferencesSpin,\n"
        "    QComboBox#PreferencesCombo,\n"
        "    QLineEdit#PreferencesField {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 4px;\n"
        "        padding: 4px 8px;\n"
        "        min-height: 24px;\n"
        "    }\n"
        "    QLineEdit#KeyCaptureField:focus,\n"
        "    QSpinBox#PreferencesSpin:focus,\n"
        "    QComboBox#PreferencesCombo:focus,\n"
        "    QLineEdit#PreferencesField:focus {\n"
        "        border: 1px solid %s;\n"
        "    }\n"
        "    QPushButton#PreferencesPrimaryButton {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 5px;\n"
        "        padding: 7px 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QPushButton#PreferencesPrimaryButton:hover {\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QPushButton#PreferencesSecondaryButton {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 5px;\n"
        "        padding: 7px 16px;\n"
        "    }\n"
        "    QPushButton#PreferencesSecondaryButton:hover {\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QFrame#PreferencesRow {\n"
        "        background-color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 5px;\n"
        "    }\n"
        "    QScrollArea#PreferencesScroll {\n"
        "        background: transparent;\n"
        "        border: none;\n"
        "    }\n"
        "    ",
        t->panel_bg, t->text_primary, t->text_primary, t->border, t->window_bg,
        t->surface_bg, t->text_secondary, t->border, t->window_bg,
        t->text_primary, t->window_bg, t->accent_text, t->border,
        t->text_muted, t->text_secondary, t->border_subtle, t->text_secondary,
        t->border, t->accent, t->accent, t->text_primary, t->accent_hover,
        t->accent_hover, t->surface_bg, t->text_secondary, t->border,
        t->surface_elevated, t->surface_bg, t->border);

    return sb_finish(&sb);
}

int styling_build_properties(const styling_tokens *t, char *out, size_t cap)
{
    sb_t sb;
    sb_init(&sb, out, cap);

    sb_cat(&sb,
        "\n"
        "    QWidget#PropertiesPanel,\n"
        "    QWidget#PropertiesContent,\n"
        "    QScrollArea#PropertiesScroll {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "    }\n"
        "    QWidget#PropertiesPanel QLabel {\n"
        "        color: %s;\n"
        "    }\n"
        "    QLabel#PropertiesNodeTitle {\n"
        "        color: %s;\n"
        "        font-size: 12px;\n"
        "        font-weight: 700;\n"
        "        padding: 1px 1px 0px 1px;\n"
        "    }\n"
        "    QLabel#PropertiesNodeMeta {\n"
        "        color: %s;\n"
        "        font-size: 9px;\n"
        "        padding: 0px 1px 3px 1px;\n"
        "    }\n"
        "    QFrame#PropertiesDivider {\n"
        "        background-color: %s;\n"
        "        max-height: 1px;\n"
        "        min-height: 1px;\n"
        "        border: none;\n"
        "    }\n"
        "    QLabel#PropertiesEmptyLabel {\n"
        "        color: %s;\n"
        "        font-style: italic;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QWidget#PropertyRow {\n"
        "        background-color: transparent;\n"
        "        border: none;\n"
        "        min-height: 24px;\n"
        "        max-height: 28px;\n"
        "    }\n"
        "    QLabel#PropertyRowLabel {\n"
        "        color: %s;\n"
        "        font-size: 10px;\n"
        "        font-weight: 500;\n"
        "    }\n"
        "    QLabel#PropertySectionLabel {\n"
        "        color: %s;\n"
        "        font-size: 9px;\n"
        "        font-weight: 700;\n"
        "        letter-spacing: 0.5px;\n"
        "        padding: 6px 1px 1px 1px;\n"
        "    }\n"
        "    QDoubleSpinBox#PropertySpin,\n"
        "    QSpinBox#PropertySpin,\n"
        "    QLineEdit#PropertyField,\n"
        "    QComboBox#PropertyCombo {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-top: 1px solid #050505;\n"
        "        border-left: 1px solid #080808;\n"
        "        border-right: 1px solid #303030;\n"
        "        border-bottom: 1px solid #404040;\n"
        "        border-radius: 4px;\n"
        "        padding: 2px 6px;\n"
        "        min-height: 20px;\n"
        "        max-height: 24px;\n"
        "        font-size: 11px;\n"
        "        selection-background-color: %s;\n"
        "        selection-color: %s;\n"
        "    }\n"
        "    QDoubleSpinBox#PropertySpin:focus,\n"
        "    QSpinBox#PropertySpin:focus,\n"
        "    QLineEdit#PropertyField:focus,\n"
        "    QComboBox#PropertyCombo:focus {\n"
        "        background-color: %s;\n"
        "        border: 1px solid %s;\n"
        "        color: %s;\n"
        "    }\n"
        "    QDoubleSpinBox#PropertySpin::up-button,\n"
        "    QDoubleSpinBox#PropertySpin::down-button,\n"
        "    QSpinBox#PropertySpin::up-button,\n"
        "    QSpinBox#PropertySpin::down-button {\n"
        "        background-color: %s;\n"
        "        border: none;\n"
        "        width: 14px;\n"
        "    }\n"
        "    QComboBox#PropertyCombo::drop-down {\n"
        "        border: none;\n"
        "        width: 16px;\n"
        "    }\n"
        "    QComboBox#PropertyCombo QAbstractItemView {\n"
        "        background-color: %s;\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        selection-background-color: %s;\n"
        "        selection-color: %s;\n"
        "        outline: none;\n"
        "    }\n"
        "    QWidget#PropertyColorSwatch {\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 4px;\n"
        "    }\n"
        "    QLabel#PropertyColorHex {\n"
        "        color: %s;\n"
        "        font-size: 10px;\n"
        "        font-family: Consolas, \"Cascadia Mono\", monospace;\n"
        "    }\n"
        "    QPushButton#PropertyBrowseButton {\n"
        "        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
        "            stop:0 %s, stop:1 %s);\n"
        "        color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-top: 1px solid #555555;\n"
        "        border-bottom: 2px solid %s;\n"
        "        border-radius: 4px;\n"
        "        min-width: 26px;\n"
        "        max-width: 26px;\n"
        "        min-height: 22px;\n"
        "        font-weight: 700;\n"
        "    }\n"
        "    QPushButton#PropertyBrowseButton:hover {\n"
        "        color: %s;\n"
        "        border-color: %s;\n"
        "    }\n"
        "    QSlider#PropertySlider {\n"
        "        min-height: 24px;\n"
        "    }\n"
        "    QSlider#PropertySlider::groove:horizontal {\n"
        "        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
        "            stop:0 #0a0a0a, stop:0.15 %s, stop:1 %s);\n"
        "        border-top: 1px solid #050505;\n"
        "        border-bottom: 1px solid #404040;\n"
        "        height: 7px;\n"
        "        border-radius: 4px;\n"
        "    }\n"
        "    QSlider#PropertySlider::sub-page:horizontal {\n"
        "        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
        "            stop:0 %s, stop:1 %s);\n"
        "        border-radius: 4px;\n"
        "    }\n"
        "    QSlider#PropertySlider::handle:horizontal {\n"
        "        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
        "            stop:0 #f4f4f4, stop:1 #a8a8a8);\n"
        "        border: 1px solid #1a1a1a;\n"
        "        border-top: 1px solid #ffffff;\n"
        "        width: 13px;\n"
        "        height: 13px;\n"
        "        margin: -5px -1px;\n"
        "        border-radius: 7px;\n"
        "    }\n"
        "    QCheckBox#PropertyCheck {\n"
        "        color: %s;\n"
        "        font-size: 10px;\n"
        "        spacing: 6px;\n"
        "    }\n"
        "    QCheckBox#PropertyCheck::indicator {\n"
        "        width: 14px;\n"
        "        height: 14px;\n"
        "        border-radius: 3px;\n"
        "        border: 1px solid %s;\n"
        "        background-color: %s;\n"
        "    }\n"
        "    QCheckBox#PropertyCheck::indicator:checked {\n"
        "        background-color: %s;\n"
        "        border-color: %s;\n"
        "    }\n"
        "    QLabel#PropertySliderValue {\n"
        "        color: %s;\n"
        "        font-size: 10px;\n"
        "        font-weight: 600;\n"
        "        min-width: 36px;\n"
        "        padding: 2px 5px;\n"
        "        background-color: %s;\n"
        "        border: 1px solid %s;\n"
        "        border-radius: 4px;\n"
        "    }\n"
        "    ",
        t->panel_bg, t->text_secondary, t->text_secondary, t->text_primary,
        t->text_muted, t->border, t->text_muted, t->text_secondary,
        t->accent_text, t->border_subtle, t->text_primary, t->border,
        t->accent, t->text_primary, t->surface_bg, t->accent, t->text_primary,
        t->surface_bg, t->panel_bg, t->text_primary, t->border, t->accent,
        t->text_primary, t->border, t->text_secondary, t->surface_elevated,
        t->surface_bg, t->text_primary, t->border, t->border_subtle,
        t->text_primary, t->accent_hover, t->border_subtle, t->surface_bg,
        t->accent_hover, t->accent, t->text_secondary, t->border,
        t->border_subtle, t->accent, t->accent_hover, t->accent_text,
        t->border_subtle, t->border);

    return sb_finish(&sb);
}

/* ------------------------------------------------------------------ */
/* Static named QSS catalog                                           */
/* ------------------------------------------------------------------ */

const char *styling_dark_theme(void)
{
    static const char qss[] =
        "\n"
        "    QMainWindow {\n"
        "        background-color: #1e1e1e;\n"
        "        color: #ffffff;\n"
        "    }\n"
        "    QLabel#StatusKeyHints {\n"
        "        color: #8a8a94;\n"
        "        font-size: 11px;\n"
        "        padding-right: 8px;\n"
        "    }\n"
        "    QMainWindow::separator {\n"
        "        background-color: #121212;\n"
        "        width: 1px;\n"
        "        height: 1px;\n"
        "    }\n"
        "    QDockWidget {\n"
        "        background-color: #1a1a1a;\n"
        "        color: #ffffff;\n"
        "        border: 1px solid #0a0a0a;\n"
        "        border-right: 1px solid #2e2e2e;\n"
        "        border-bottom: 1px solid #2e2e2e;\n"
        "        titlebar-close-icon: url(none);\n"
        "        titlebar-normal-icon: url(none);\n"
        "    }\n"
        "    QDockWidget::title {\n"
        "        background-color: #252525;\n"
        "        color: #c8c8c8;\n"
        "        padding: 1px 8px;\n"
        "        border: none;\n"
        "        border-bottom: 1px solid #0a0a0a;\n"
        "        border-top: 1px solid #323232;\n"
        "        font-weight: 600;\n"
        "        font-size: 10px;\n"
        "        text-transform: uppercase;\n"
        "    }\n"
        "    QDockWidget::close-button, QDockWidget::float-button {\n"
        "        background-color: transparent;\n"
        "        border: none;\n"
        "        padding: 0px;\n"
        "        margin: 1px 2px 0px 0px;\n"
        "        width: 12px;\n"
        "        height: 12px;\n"
        "    }\n"
        "    QDockWidget::close-button:hover, QDockWidget::float-button:hover {\n"
        "        background-color: #3a3a3a;\n"
        "    }\n"
        "    QMenuBar {\n"
        "        background-color: #2a2a2a;\n"
        "        color: #ffffff;\n"
        "        border-bottom: 1px solid #3a3a3a;\n"
        "        spacing: 8px;\n"
        "    }\n"
        "    QMenuBar::item:selected {\n"
        "        background-color: #0078d4;\n"
        "        padding: 4px 12px;\n"
        "        border-radius: 0px;\n"
        "    }\n"
        "    QMenu {\n"
        "        background-color: #2a2a2a;\n"
        "        color: #ffffff;\n"
        "        border: 1px solid #3a3a3a;\n"
        "        padding: 4px 0px;\n"
        "    }\n"
        "    QMenu::item:selected {\n"
        "        background-color: #0078d4;\n"
        "        padding: 4px 16px;\n"
        "    }\n"
        "    QMenu::separator {\n"
        "        background-color: #3a3a3a;\n"
        "        height: 1px;\n"
        "        margin: 4px 0px;\n"
        "    }\n"
        "    QScrollBar:vertical {\n"
        "        width: 12px;\n"
        "        background-color: #2a2a2a;\n"
        "    }\n"
        "    QScrollBar::handle:vertical {\n"
        "        background-color: #555555;\n"
        "        border-radius: 6px;\n"
        "        min-height: 20px;\n"
        "    }\n"
        "    QScrollBar::handle:vertical:hover {\n"
        "        background-color: #666666;\n"
        "    }\n"
        "    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
        "        border: none;\n"
        "        background: none;\n"
        "    }\n";
    return qss;
}

const char *styling_shortcuts_dialog_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#ShortcutsDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QLabel#ShortcutsDialogTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QLabel#ShortcutsDialogSubtitle {\n"
        "        color: #9a9aa4;\n"
        "        font-size: 12px;\n"
        "        padding-bottom: 4px;\n"
        "    }\n"
        "    QScrollArea#ShortcutsScroll {\n"
        "        background: transparent;\n"
        "        border: none;\n"
        "    }\n"
        "    QWidget#ShortcutsContent {\n"
        "        background: transparent;\n"
        "    }\n"
        "    QLabel#ShortcutsCategory {\n"
        "        color: #8ab4d8;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        letter-spacing: 0.5px;\n"
        "        padding: 4px 2px 2px 2px;\n"
        "    }\n"
        "    QFrame#ShortcutsRow {\n"
        "        background-color: #222228;\n"
        "        border: 1px solid #2e2e36;\n"
        "        border-radius: 5px;\n"
        "    }\n"
        "    QLabel#ShortcutsActionLabel {\n"
        "        color: #e6e6ec;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QLabel#ShortcutsKeyBadge {\n"
        "        background-color: #141418;\n"
        "        color: #d0d0d8;\n"
        "        border: 1px solid #3a3a44;\n"
        "        border-radius: 4px;\n"
        "        padding: 3px 8px;\n"
        "        font-size: 11px;\n"
        "        font-family: Consolas, \"Cascadia Mono\", monospace;\n"
        "        min-width: 72px;\n"
        "    }\n"
        "    QLabel#StatusKeyHints {\n"
        "        color: #8a8a94;\n"
        "        font-size: 11px;\n"
        "        padding-right: 8px;\n"
        "    }\n";
    return qss;
}

const char *styling_launcher_style(void)
{
    static const char qss[] =
        "\n"
        "    QWidget#ProjectLauncher {\n"
        "        background-color: #16161a;\n"
        "        color: #e8e8ee;\n"
        "    }\n"
        "    QLabel#LauncherBrand {\n"
        "        color: #f2f2f6;\n"
        "        font-size: 28px;\n"
        "        font-weight: 700;\n"
        "        letter-spacing: 1px;\n"
        "    }\n"
        "    QLabel#LauncherSubtitle {\n"
        "        color: #8e8e98;\n"
        "        font-size: 13px;\n"
        "        padding-bottom: 8px;\n"
        "    }\n"
        "    QPushButton#LauncherPrimaryButton {\n"
        "        background-color: #2b6ea8;\n"
        "        color: #ffffff;\n"
        "        border: 1px solid #3d7eb8;\n"
        "        border-radius: 6px;\n"
        "        padding: 12px 18px;\n"
        "        font-size: 13px;\n"
        "        font-weight: 600;\n"
        "        text-align: left;\n"
        "    }\n"
        "    QPushButton#LauncherPrimaryButton:hover {\n"
        "        background-color: #347ebc;\n"
        "    }\n"
        "    QPushButton#LauncherPrimaryButton:pressed {\n"
        "        background-color: #245f92;\n"
        "    }\n"
        "    QPushButton#LauncherSecondaryButton {\n"
        "        background-color: #222228;\n"
        "        color: #e0e0e8;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 6px;\n"
        "        padding: 12px 18px;\n"
        "        font-size: 13px;\n"
        "        font-weight: 600;\n"
        "        text-align: left;\n"
        "    }\n"
        "    QPushButton#LauncherSecondaryButton:hover {\n"
        "        background-color: #2a2a32;\n"
        "        border-color: #45454f;\n"
        "    }\n"
        "    QLabel#LauncherRecentHeader {\n"
        "        color: #9ecfff;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        letter-spacing: 0.6px;\n"
        "        padding-top: 10px;\n"
        "    }\n"
        "    QListWidget#LauncherRecentList {\n"
        "        background-color: #121216;\n"
        "        border: 1px solid #2c2c34;\n"
        "        border-radius: 6px;\n"
        "        color: #e0e0e8;\n"
        "        outline: none;\n"
        "        padding: 4px;\n"
        "    }\n"
        "    QListWidget#LauncherRecentList::item {\n"
        "        padding: 10px 12px;\n"
        "        border-radius: 4px;\n"
        "        margin: 2px 0px;\n"
        "    }\n"
        "    QListWidget#LauncherRecentList::item:selected {\n"
        "        background-color: #2b6ea8;\n"
        "        color: #ffffff;\n"
        "    }\n"
        "    QListWidget#LauncherRecentList::item:hover {\n"
        "        background-color: #222228;\n"
        "    }\n"
        "    QLabel#LauncherEmptyRecent {\n"
        "        color: #6e6e78;\n"
        "        font-size: 12px;\n"
        "        padding: 18px 8px;\n"
        "    }\n";
    return qss;
}

const char *styling_bootloader_style(void)
{
    static const char qss[] =
        "\n"
        "    QWidget#BootloaderWindow {\n"
        "        background-color: #121216;\n"
        "        color: #e6e6ec;\n"
        "    }\n"
        "    QLabel#BootloaderTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QLabel#BootloaderStage {\n"
        "        color: #9ecfff;\n"
        "        font-size: 12px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QPlainTextEdit#BootloaderLog {\n"
        "        background-color: #0c0c10;\n"
        "        color: #c8c8d0;\n"
        "        border: 1px solid #2a2a32;\n"
        "        border-radius: 6px;\n"
        "        padding: 8px;\n"
        "        font-family: Consolas, \"Cascadia Mono\", monospace;\n"
        "        font-size: 12px;\n"
        "        selection-background-color: #2b6ea8;\n"
        "    }\n"
        "    QProgressBar#BootloaderProgress {\n"
        "        background-color: #1a1a20;\n"
        "        border: 1px solid #2e2e36;\n"
        "        border-radius: 4px;\n"
        "        text-align: center;\n"
        "        color: #d0d0d8;\n"
        "        min-height: 16px;\n"
        "        max-height: 16px;\n"
        "    }\n"
        "    QProgressBar#BootloaderProgress::chunk {\n"
        "        background-color: #2b6ea8;\n"
        "        border-radius: 3px;\n"
        "    }\n"
        "    QLabel#BootloaderStatus {\n"
        "        color: #8a8a94;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QPushButton#BootloaderCancelButton {\n"
        "        background-color: #222228;\n"
        "        color: #d0d0d8;\n"
        "        border: 1px solid #3a3a44;\n"
        "        border-radius: 4px;\n"
        "        padding: 6px 14px;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QPushButton#BootloaderCancelButton:hover {\n"
        "        background-color: #2c2c34;\n"
        "    }\n"
        "    QPushButton#BootloaderCancelButton:disabled {\n"
        "        color: #666670;\n"
        "        border-color: #2a2a32;\n"
        "    }\n";
    return qss;
}

const char *styling_node_search_style(void)
{
    static const char qss[] =
        "\n"
        "    QFrame#NodeSearchPalette {\n"
        "        background-color: #1e1e1e;\n"
        "        border: 1px solid #3a3a42;\n"
        "        border-radius: 8px;\n"
        "    }\n"
        "    QLabel#NodeSearchTitle {\n"
        "        color: #c8c8d0;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        letter-spacing: 0.4px;\n"
        "        padding-left: 2px;\n"
        "    }\n"
        "    QLineEdit#NodeSearchField {\n"
        "        background-color: #141416;\n"
        "        color: #eaeaf0;\n"
        "        border: 1px solid #33333a;\n"
        "        border-radius: 5px;\n"
        "        padding: 7px 10px;\n"
        "        selection-background-color: #2b6ea8;\n"
        "        font-size: 13px;\n"
        "    }\n"
        "    QLineEdit#NodeSearchField:focus {\n"
        "        border: 1px solid #3d7eb8;\n"
        "    }\n"
        "    QListWidget#NodeSearchList {\n"
        "        background-color: #18181c;\n"
        "        color: #e6e6e6;\n"
        "        border: 1px solid #2c2c32;\n"
        "        border-radius: 5px;\n"
        "        outline: none;\n"
        "        font-size: 12px;\n"
        "        padding: 2px;\n"
        "    }\n"
        "    QListWidget#NodeSearchList::item {\n"
        "        padding: 7px 8px;\n"
        "        border-radius: 4px;\n"
        "        margin: 1px 2px;\n"
        "    }\n"
        "    QListWidget#NodeSearchList::item:selected {\n"
        "        background-color: #2b6ea8;\n"
        "        color: #ffffff;\n"
        "    }\n"
        "    QListWidget#NodeSearchList::item:hover:!selected {\n"
        "        background-color: #2a2a32;\n"
        "    }\n";
    return qss;
}

const char *styling_context_menu_style(void)
{
    static const char qss[] =
        "\n"
        "    QMenu {\n"
        "        background-color: #1e1e1e;\n"
        "        color: #e6e6e6;\n"
        "        border: 1px solid #333333;\n"
        "        border-radius: 6px;\n"
        "        padding: 6px 4px;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QMenu::item {\n"
        "        padding: 6px 28px 6px 10px;\n"
        "        margin: 1px 4px;\n"
        "        border-radius: 4px;\n"
        "        min-height: 22px;\n"
        "    }\n"
        "    QMenu::item:selected {\n"
        "        background-color: #2b6ea8;\n"
        "        color: #ffffff;\n"
        "    }\n"
        "    QMenu::item:pressed {\n"
        "        background-color: #245f91;\n"
        "    }\n"
        "    QMenu::item:disabled {\n"
        "        color: #666666;\n"
        "    }\n"
        "    QMenu::icon {\n"
        "        padding-left: 6px;\n"
        "        width: 14px;\n"
        "        height: 14px;\n"
        "    }\n"
        "    QMenu::separator {\n"
        "        background-color: #333333;\n"
        "        height: 1px;\n"
        "        margin: 5px 8px;\n"
        "    }\n"
        "    QMenu::right-arrow {\n"
        "        width: 8px;\n"
        "        height: 8px;\n"
        "        margin-right: 6px;\n"
        "    }\n";
    return qss;
}

const char *styling_dock_style(void)
{
    static const char qss[] =
        "\n"
        "    QDockWidget {\n"
        "        background-color: #1a1a1a;\n"
        "        color: #d8d8d8;\n"
        "        border: 1px solid #0a0a0a;\n"
        "        border-right: 1px solid #2c2c2c;\n"
        "        border-bottom: 1px solid #2c2c2c;\n"
        "    }\n"
        "    QDockWidget::title {\n"
        "        background-color: #252525;\n"
        "        color: #b8b8b8;\n"
        "        padding: 1px 8px;\n"
        "        border: none;\n"
        "        border-top: 1px solid #343434;\n"
        "        border-bottom: 1px solid #080808;\n"
        "        font-weight: 600;\n"
        "        font-size: 10px;\n"
        "    }\n"
        "    QDockWidget > QWidget {\n"
        "        background-color: #1a1a1a;\n"
        "        border: none;\n"
        "        border-top: 1px solid #0a0a0a;\n"
        "    }\n";
    return qss;
}

const char *styling_menubar_style(void)
{
    static const char qss[] =
        "\n"
        "    QMenuBar {\n"
        "        background-color: #1e1e1e;\n"
        "        color: #e8e8e8;\n"
        "        border-bottom: 1px solid #121212;\n"
        "        spacing: 4px;\n"
        "        padding: 4px 10px;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QMenuBar::item {\n"
        "        background-color: transparent;\n"
        "        color: #d0d0d0;\n"
        "        padding: 5px 12px;\n"
        "        border-radius: 4px;\n"
        "        font-weight: 500;\n"
        "    }\n"
        "    QMenuBar::item:selected {\n"
        "        background-color: #2b6ea8;\n"
        "        color: #ffffff;\n"
        "    }\n"
        "    QMenuBar::item:pressed {\n"
        "        background-color: #245f91;\n"
        "        color: #ffffff;\n"
        "    }\n";
    return qss;
}

const char *styling_timeline_style(void)
{
    static const char qss[] =
        "\n"
        "    TimelineWidget {\n"
        "        background-color: #1a1a1a;\n"
        "    }\n"
        "    QLabel#TimelineTimecodeLabel {\n"
        "        color: #e8e8e8;\n"
        "        font-family: \"Cascadia Mono\", \"Consolas\", \"Courier New\", monospace;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        padding: 1px 6px;\n"
        "        background-color: #141414;\n"
        "        border: 1px solid #2a2a2a;\n"
        "        border-radius: 3px;\n"
        "        min-width: 84px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QLabel#TimelineFrameLabel {\n"
        "        color: #8a8a8a;\n"
        "        font-family: \"Cascadia Mono\", \"Consolas\", \"Courier New\", monospace;\n"
        "        font-size: 10px;\n"
        "        padding: 1px 4px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QLabel#TimelineMetaLabel {\n"
        "        color: #6e6e6e;\n"
        "        font-size: 9px;\n"
        "        padding: 0px 4px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QPushButton#TimelineTransportButton {\n"
        "        background-color: #252525;\n"
        "        color: #d0d0d0;\n"
        "        border: 1px solid #333333;\n"
        "        border-radius: 3px;\n"
        "        padding: 2px;\n"
        "        min-width: 24px;\n"
        "        max-width: 24px;\n"
        "        min-height: 22px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QPushButton#TimelineTransportButton:hover {\n"
        "        background-color: #303030;\n"
        "        border-color: #454545;\n"
        "    }\n"
        "    QPushButton#TimelineTransportButton:pressed {\n"
        "        background-color: #1c1c1c;\n"
        "        border-color: #2a2a2a;\n"
        "    }\n"
        "    QPushButton#TimelinePlayButton {\n"
        "        background-color: #252525;\n"
        "        color: #d0d0d0;\n"
        "        border: 1px solid #3a3a3a;\n"
        "        border-radius: 3px;\n"
        "        padding: 2px;\n"
        "        min-width: 28px;\n"
        "        max-width: 28px;\n"
        "        min-height: 22px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QPushButton#TimelinePlayButton:hover {\n"
        "        background-color: #2e2e2e;\n"
        "        border-color: #4a4a4a;\n"
        "    }\n"
        "    QPushButton#TimelinePlayButton:pressed {\n"
        "        background-color: #1c1c1c;\n"
        "    }\n"
        "    QPushButton#TimelinePlayButton[playing=\"true\"] {\n"
        "        background-color: #1e3a52;\n"
        "        border-color: #2b6ea8;\n"
        "    }\n"
        "    QPushButton#TimelineToggleButton {\n"
        "        background-color: #252525;\n"
        "        color: #9a9a9a;\n"
        "        border: 1px solid #333333;\n"
        "        border-radius: 3px;\n"
        "        padding: 2px;\n"
        "        min-width: 24px;\n"
        "        max-width: 24px;\n"
        "        min-height: 22px;\n"
        "        max-height: 22px;\n"
        "    }\n"
        "    QPushButton#TimelineToggleButton:hover {\n"
        "        background-color: #303030;\n"
        "        border-color: #454545;\n"
        "    }\n"
        "    QPushButton#TimelineToggleButton:checked {\n"
        "        background-color: #243528;\n"
        "        border-color: #3d6b45;\n"
        "    }\n"
        "    QComboBox#TimelineSpeedCombo {\n"
        "        background-color: #252525;\n"
        "        color: #c8c8c8;\n"
        "        border: 1px solid #333333;\n"
        "        border-radius: 3px;\n"
        "        padding: 1px 6px;\n"
        "        min-height: 22px;\n"
        "        max-height: 22px;\n"
        "        min-width: 56px;\n"
        "        font-size: 10px;\n"
        "    }\n"
        "    QComboBox#TimelineSpeedCombo:hover {\n"
        "        border-color: #454545;\n"
        "        color: #ffffff;\n"
        "    }\n"
        "    QComboBox#TimelineSpeedCombo::drop-down {\n"
        "        border: none;\n"
        "        width: 16px;\n"
        "    }\n"
        "    QComboBox#TimelineSpeedCombo QAbstractItemView {\n"
        "        background-color: #1e1e1e;\n"
        "        color: #d0d0d0;\n"
        "        border: 1px solid #333333;\n"
        "        selection-background-color: #2b6ea8;\n"
        "        outline: none;\n"
        "    }\n"
        "    QFrame#TimelineToolbar {\n"
        "        background-color: #1a1a1a;\n"
        "        border: none;\n"
        "        border-bottom: 1px solid #121212;\n"
        "    }\n";
    return qss;
}

const char *styling_keyframes_panel_style(void)
{
    static const char qss[] =
        "\n"
        "    QWidget#KeyframesPanelWidget {\n"
        "        background-color: #18181c;\n"
        "    }\n"
        "    QLabel#KeyframesHeader {\n"
        "        color: #e8e8ec;\n"
        "        font-size: 12px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QWidget#KeyframesToolbar {\n"
        "        background-color: #141418;\n"
        "        border: 1px solid #2a2a32;\n"
        "        border-radius: 4px;\n"
        "        padding: 2px;\n"
        "    }\n"
        "    QComboBox#KeyframesPropertyCombo {\n"
        "        background-color: #222228;\n"
        "        color: #e0e0e8;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 4px;\n"
        "        padding: 2px 6px;\n"
        "        min-height: 22px;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QComboBox#KeyframesPropertyCombo::drop-down {\n"
        "        border: none;\n"
        "        width: 16px;\n"
        "    }\n"
        "    QPushButton#KeyframesActionButton {\n"
        "        background-color: #222228;\n"
        "        color: #d8d8e0;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 4px;\n"
        "        padding: 2px 10px;\n"
        "        min-height: 22px;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QPushButton#KeyframesActionButton:hover {\n"
        "        background-color: #2a2a34;\n"
        "        border-color: #45454f;\n"
        "    }\n"
        "    QPushButton#KeyframesActionButton:disabled {\n"
        "        color: #666670;\n"
        "        border-color: #2a2a30;\n"
        "    }\n"
        "    QFrame#KeyframesDivider {\n"
        "        color: #2a2a32;\n"
        "    }\n"
        "    QScrollArea#KeyframesScroll {\n"
        "        background-color: transparent;\n"
        "        border: none;\n"
        "    }\n"
        "    QWidget#KeyframesContent {\n"
        "        background-color: transparent;\n"
        "    }\n"
        "    QLabel#KeyframesEmptyState,\n"
        "    QLabel#KeyframesHintLabel,\n"
        "    QLabel#KeyframesEmptyLabel {\n"
        "        color: #888890;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QLabel#KeyframePropertyLabel {\n"
        "        color: #c8c8d0;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QPushButton#KeyframeFrameChip {\n"
        "        background-color: #222228;\n"
        "        color: #ffbe3c;\n"
        "        border: 1px solid #8a7030;\n"
        "        border-radius: 3px;\n"
        "        padding: 0 8px;\n"
        "        font-size: 10px;\n"
        "        min-width: 28px;\n"
        "    }\n"
        "    QPushButton#KeyframeFrameChip:hover {\n"
        "        background-color: #2c2c36;\n"
        "        border-color: #ffbe3c;\n"
        "    }\n"
        "    QPushButton#KeyframeFrameChip[current=\"true\"] {\n"
        "        background-color: #ffbe3c;\n"
        "        color: #141418;\n"
        "        border-color: #ffd878;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QPushButton#KeyframeMiniButton {\n"
        "        background-color: #222228;\n"
        "        color: #ffbe3c;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 3px;\n"
        "        font-size: 12px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QPushButton#KeyframeMiniButton:hover {\n"
        "        border-color: #ffbe3c;\n"
        "    }\n";
    return qss;
}

const char *styling_log_viewer_style(void)
{
    static const char qss[] =
        "\n"
        "    QWidget#LogViewerWidget {\n"
        "        background-color: #121216;\n"
        "    }\n"
        "    QWidget#LogViewerToolbar {\n"
        "        background-color: #18181c;\n"
        "        border-bottom: 1px solid #2a2a32;\n"
        "    }\n"
        "    QPlainTextEdit#LogViewerTerminal {\n"
        "        background-color: #0c0c10;\n"
        "        color: #c8c8d0;\n"
        "        border: none;\n"
        "        padding: 6px 8px;\n"
        "        font-family: Consolas, \"Cascadia Mono\", monospace;\n"
        "        font-size: 11px;\n"
        "        selection-background-color: #2b6ea8;\n"
        "    }\n"
        "    QCheckBox#LogViewerAutoScroll {\n"
        "        color: #b0b0b8;\n"
        "        font-size: 11px;\n"
        "        spacing: 5px;\n"
        "    }\n"
        "    QCheckBox#LogViewerAutoScroll::indicator {\n"
        "        width: 13px;\n"
        "        height: 13px;\n"
        "        border-radius: 2px;\n"
        "        border: 1px solid #3a3a3a;\n"
        "        background-color: #141414;\n"
        "    }\n"
        "    QCheckBox#LogViewerAutoScroll::indicator:checked {\n"
        "        background-color: #2b6ea8;\n"
        "        border-color: #3d7eb8;\n"
        "    }\n"
        "    QPushButton#LogViewerClearButton {\n"
        "        background-color: #222228;\n"
        "        color: #d8d8e0;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 4px;\n"
        "        padding: 2px 10px;\n"
        "        min-height: 20px;\n"
        "        max-height: 22px;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QPushButton#LogViewerClearButton:hover {\n"
        "        background-color: #2a2a32;\n"
        "        border-color: #45454f;\n"
        "    }\n"
        "    QPushButton#LogViewerClearButton:pressed {\n"
        "        background-color: #1a1a20;\n"
        "    }\n";
    return qss;
}

const char *styling_toolbar_style(void)
{
    static const char qss[] =
        "\n"
        "    QToolBar#PinBar {\n"
        "        background-color: #252525;\n"
        "        border: none;\n"
        "        border-bottom: 1px solid #3a3a3a;\n"
        "        spacing: 2px;\n"
        "        padding: 3px 8px;\n"
        "    }\n"
        "    QToolBar#PinBar::separator {\n"
        "        background-color: #3a3a3a;\n"
        "        width: 1px;\n"
        "        margin: 4px 6px;\n"
        "    }\n"
        "    QToolBar#PinBar QToolButton {\n"
        "        background-color: transparent;\n"
        "        border: 1px solid transparent;\n"
        "        border-radius: 4px;\n"
        "        padding: 4px 6px;\n"
        "        margin: 0px 1px;\n"
        "        color: #d8d8d8;\n"
        "    }\n"
        "    QToolBar#PinBar QToolButton:hover {\n"
        "        background-color: #333333;\n"
        "        border-color: #444444;\n"
        "    }\n"
        "    QToolBar#PinBar QToolButton:pressed {\n"
        "        background-color: #0078d4;\n"
        "        border-color: #0078d4;\n"
        "    }\n"
        "    QToolBar#PinBar QToolButton:disabled {\n"
        "        color: #666666;\n"
        "    }\n";
    return qss;
}

const char *styling_pin_bar_dialog_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#PinBarDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QLabel#PinBarDialogTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QLabel#PinBarDialogSubtitle {\n"
        "        color: #9a9aa4;\n"
        "        font-size: 12px;\n"
        "        padding-bottom: 4px;\n"
        "    }\n"
        "    QScrollArea#PinBarScroll {\n"
        "        background: transparent;\n"
        "        border: none;\n"
        "    }\n"
        "    QWidget#PinBarContent {\n"
        "        background: transparent;\n"
        "    }\n"
        "    QLabel#PinBarCategory {\n"
        "        color: #8ab4d8;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        letter-spacing: 0.5px;\n"
        "        padding: 4px 2px 2px 2px;\n"
        "    }\n"
        "    QCheckBox#PinBarCheckbox {\n"
        "        color: #e6e6ec;\n"
        "        font-size: 12px;\n"
        "        padding: 5px 8px;\n"
        "        background-color: #222228;\n"
        "        border: 1px solid #2e2e36;\n"
        "        border-radius: 5px;\n"
        "    }\n"
        "    QCheckBox#PinBarCheckbox:hover {\n"
        "        border-color: #444450;\n"
        "    }\n";
    return qss;
}

const char *styling_status_bar_style(void)
{
    static const char qss[] =
        "\n"
        "    QStatusBar {\n"
        "        background-color: #007acc;\n"
        "        color: #ffffff;\n"
        "        font-size: 11px;\n"
        "        border-top: 1px solid #005a9e;\n"
        "    }\n"
        "    QStatusBar::item {\n"
        "        border: none;\n"
        "    }\n"
        "    QLabel#StatusProjectInfo {\n"
        "        color: #e8f4ff;\n"
        "        font-size: 11px;\n"
        "        padding-left: 4px;\n"
        "    }\n"
        "    QLabel#StatusPlayhead {\n"
        "        color: #ffffff;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        padding: 0px 8px;\n"
        "    }\n"
        "    QLabel#StatusCacheInfo {\n"
        "        color: #c8e4ff;\n"
        "        font-size: 11px;\n"
        "        padding-right: 8px;\n"
        "    }\n"
        "    QLabel#StatusSeparator {\n"
        "        color: #66b3ff;\n"
        "        padding: 0px 4px;\n"
        "    }\n";
    return qss;
}

const char *styling_plugin_surface_style(void)
{
    static const char qss[] =
        "\n"
        "    QWidget#PluginSurface {\n"
        "        background-color: transparent;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QLabel#PluginSurfaceLabel {\n"
        "        color: #c8c8d0;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QPushButton#PluginSurfaceButton {\n"
        "        background-color: #2a2a32;\n"
        "        color: #e6e6e6;\n"
        "        border: 1px solid #3a3a44;\n"
        "        border-radius: 4px;\n"
        "        padding: 4px 10px;\n"
        "        min-height: 24px;\n"
        "    }\n"
        "    QPushButton#PluginSurfaceButton:hover {\n"
        "        background-color: #3a3a44;\n"
        "    }\n"
        "    QLineEdit#PluginSurfaceText {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "        border: 1px solid #3a3a44;\n"
        "        border-radius: 4px;\n"
        "        padding: 4px 8px;\n"
        "        min-height: 24px;\n"
        "    }\n"
        "    QFrame#PluginSurfaceDivider {\n"
        "        color: #3a3a44;\n"
        "    }\n";
    return qss;
}

const char *styling_plugin_dialog_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#PluginPopupDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n";
    return qss;
}

const char *styling_about_dialog_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#AboutDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QLabel#AboutTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 20px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QLabel#AboutVersion {\n"
        "        color: #8ab4d8;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QLabel#AboutBody {\n"
        "        color: #b0b0b8;\n"
        "        font-size: 12px;\n"
        "        line-height: 1.4;\n"
        "    }\n";
    return qss;
}

const char *styling_project_settings_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#ProjectSettingsDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QLabel#ProjectSettingsTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QGroupBox {\n"
        "        color: #c8c8d0;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 6px;\n"
        "        margin-top: 8px;\n"
        "        padding-top: 12px;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QGroupBox::title {\n"
        "        subcontrol-origin: margin;\n"
        "        left: 10px;\n"
        "        padding: 0 4px;\n"
        "    }\n";
    return qss;
}

const char *styling_export_dialog_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#ExportDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QLabel#ExportDialogTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QLabel#ExportStatusLabel {\n"
        "        color: #b8b8c0;\n"
        "    }\n"
        "    QProgressBar#ExportProgress {\n"
        "        background-color: #141418;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 4px;\n"
        "        text-align: center;\n"
        "        color: #d0d0d8;\n"
        "        min-height: 18px;\n"
        "    }\n"
        "    QProgressBar#ExportProgress::chunk {\n"
        "        background-color: #0078d4;\n"
        "        border-radius: 3px;\n"
        "    }\n";
    return qss;
}

const char *styling_tracking_dialog_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#TrackingProgressDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QLabel#ExportDialogTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QLabel#ExportStatusLabel {\n"
        "        color: #b8b8c0;\n"
        "    }\n"
        "    QProgressBar#ExportProgress {\n"
        "        background-color: #141418;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 4px;\n"
        "        text-align: center;\n"
        "        color: #d0d0d8;\n"
        "        min-height: 18px;\n"
        "    }\n"
        "    QProgressBar#ExportProgress::chunk {\n"
        "        background-color: #0078d4;\n"
        "        border-radius: 3px;\n"
        "    }\n";
    return qss;
}

const char *styling_media_pool_style(void)
{
    static const char qss[] =
        "\n"
        "    QWidget#MediaPoolWidget {\n"
        "        background-color: #1a1a1a;\n"
        "        color: #e0e0e0;\n"
        "    }\n"
        "    QListWidget#MediaPoolList {\n"
        "        background-color: #141414;\n"
        "        border: 1px solid #2a2a2a;\n"
        "        border-radius: 4px;\n"
        "        outline: none;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QListWidget#MediaPoolList::item {\n"
        "        padding: 6px 8px;\n"
        "        border-bottom: 1px solid #222222;\n"
        "    }\n"
        "    QListWidget#MediaPoolList::item:selected {\n"
        "        background-color: #0078d4;\n"
        "        color: #ffffff;\n"
        "    }\n"
        "    QListWidget#MediaPoolList::item:hover:!selected {\n"
        "        background-color: #252528;\n"
        "    }\n"
        "    QLabel#MediaPoolEmpty {\n"
        "        color: #777777;\n"
        "        font-size: 11px;\n"
        "        padding: 8px;\n"
        "    }\n"
        "    QPushButton#MediaPoolRefreshButton {\n"
        "        background-color: #222228;\n"
        "        color: #d8d8e0;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 4px;\n"
        "        padding: 2px 10px;\n"
        "        min-height: 20px;\n"
        "        font-size: 11px;\n"
        "    }\n"
        "    QPushButton#MediaPoolRefreshButton:hover {\n"
        "        background-color: #2a2a32;\n"
        "    }\n";
    return qss;
}

const char *styling_selection_bar_style(void)
{
    static const char qss[] =
        "\n"
        "    QFrame#SelectionActionBar {\n"
        "        background-color: rgba(26, 26, 32, 235);\n"
        "        border: 1px solid #383842;\n"
        "        border-radius: 8px;\n"
        "    }\n"
        "    QLabel#SelectionActionCount {\n"
        "        color: #eaeaf0;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "        padding: 0 4px 0 2px;\n"
        "    }\n"
        "    QLabel#SelectionActionDivider {\n"
        "        background-color: #3a3a44;\n"
        "        max-width: 1px;\n"
        "        min-width: 1px;\n"
        "    }\n"
        "    QToolButton#SelectionActionButton {\n"
        "        background-color: transparent;\n"
        "        border: 1px solid transparent;\n"
        "        border-radius: 5px;\n"
        "        padding: 3px;\n"
        "    }\n"
        "    QToolButton#SelectionActionButton:hover:enabled {\n"
        "        background-color: #2b6ea8;\n"
        "        border-color: #3d84c0;\n"
        "    }\n"
        "    QToolButton#SelectionActionButton:checked {\n"
        "        background-color: #2b6ea8;\n"
        "        border-color: #4a95d6;\n"
        "    }\n"
        "    QToolButton#SelectionActionButton:disabled {\n"
        "        background-color: transparent;\n"
        "    }\n";
    return qss;
}

const char *styling_custom_node_dialog_style(void)
{
    static const char qss[] =
        "\n"
        "    QDialog#CustomNodeDialog {\n"
        "        background-color: #1a1a1e;\n"
        "        color: #e6e6e6;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QLabel {\n"
        "        color: #c8c8d0;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QLabel#CustomNodeTitle {\n"
        "        color: #f0f0f4;\n"
        "        font-size: 16px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QLabel#CustomNodeSubtitle {\n"
        "        color: #9a9aa4;\n"
        "        font-size: 12px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QGroupBox {\n"
        "        color: #c8c8d0;\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 6px;\n"
        "        margin-top: 8px;\n"
        "        padding-top: 12px;\n"
        "        font-size: 11px;\n"
        "        font-weight: 600;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QGroupBox::title {\n"
        "        subcontrol-origin: margin;\n"
        "        left: 10px;\n"
        "        padding: 0 4px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QLineEdit,\n"
        "    QDialog#CustomNodeDialog QComboBox {\n"
        "        background-color: #222228;\n"
        "        color: #e6e6e6;\n"
        "        border: 1px solid #3a3a44;\n"
        "        border-radius: 4px;\n"
        "        padding: 4px 8px;\n"
        "        min-height: 22px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QToolButton {\n"
        "        background-color: #2a2a32;\n"
        "        color: #e6e6e6;\n"
        "        border: 1px solid #3a3a44;\n"
        "        border-radius: 4px;\n"
        "        padding: 2px 8px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QToolButton:hover {\n"
        "        background-color: #3a3a44;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QPushButton {\n"
        "        background-color: #2a2a32;\n"
        "        color: #e6e6e6;\n"
        "        border: 1px solid #3a3a44;\n"
        "        border-radius: 4px;\n"
        "        padding: 5px 12px;\n"
        "        min-height: 24px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QPushButton:hover {\n"
        "        background-color: #3a3a44;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QScrollArea {\n"
        "        background: transparent;\n"
        "        border: none;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QTabWidget::pane {\n"
        "        border: 1px solid #34343c;\n"
        "        border-radius: 6px;\n"
        "        top: -1px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QTabBar::tab {\n"
        "        background-color: #222228;\n"
        "        color: #b8b8c0;\n"
        "        border: 1px solid #34343c;\n"
        "        border-bottom: none;\n"
        "        border-top-left-radius: 5px;\n"
        "        border-top-right-radius: 5px;\n"
        "        padding: 5px 12px;\n"
        "        margin-right: 2px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QTabBar::tab:selected {\n"
        "        background-color: #2e2e36;\n"
        "        color: #f0f0f4;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QSlider::groove:horizontal {\n"
        "        height: 4px;\n"
        "        background: #2e2e36;\n"
        "        border-radius: 2px;\n"
        "    }\n"
        "    QDialog#CustomNodeDialog QSlider::handle:horizontal {\n"
        "        background: #8ab4d8;\n"
        "        width: 10px;\n"
        "        margin: -5px 0;\n"
        "        border-radius: 5px;\n"
        "    }\n";
    return qss;
}

int styling_default_properties_style(char *out, size_t cap)
{
    styling_tokens defaults;
    styling_aphelion_dark(&defaults);
    return styling_build_properties(&defaults, out, cap);
}
