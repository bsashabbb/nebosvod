#pragma once

#include <raylib.h>

/* ========== STYLE PRESETS ========== */

#define BETON_PANEL_STYLE (PanelStyle){LIGHTGRAY, BLACK}
#define BETON_BUTTON_STYLE (ButtonStyle){GRAY, LIGHTGRAY, DARKGRAY, BLACK}
#define BETON_CHECKBOX_STYLE (CheckBoxStyle){RAYWHITE, GRAY, BLACK, BLACK}
#define BETON_SLIDER_PANEL_STYLE (PanelStyle){DARKGRAY, BLACK}
#define BETON_ACCENT_BUTTON_STYLE (ButtonStyle){ORANGE, (Color){255, 181, 20, 255}, (Color){235, 141, 0, 255}, BLACK}

/* ========== ENUMS ================== */
typedef enum {
    HORIZONTAL,
    VERTICAL
} Direction;

/* ========== CORE STRUCTS =========== */
typedef struct UIRect {
    int x, y;
    int width, height;
} UIRect;

typedef struct MouseInput {
    Vector2 pos;
    bool mouse_down, mouse_pressed, mouse_released;
} MouseInput;

typedef struct UIContext {
    MouseInput mouse_input;
    int captured_id;
} UIContext;

typedef struct ButtonState {
    bool hovered;
    bool pressed;
    bool clicked;
} ButtonState;

typedef struct UILayout {
    UIRect bounds;
    Direction direction;

    int cursor_x, cursor_y;

    int spacing;
    int padding_x, padding_y;
} UILayout;

/* ========== STYLES ========== */
typedef struct PanelStyle {
    Color bg;
    Color border;
} PanelStyle;

typedef struct ButtonStyle {
    Color bg;
    Color light;
    Color dark;
    Color border;
} ButtonStyle;

typedef struct CheckBoxStyle {
    Color bg;
    Color dark;
    Color mark;
    Color border;
} CheckBoxStyle;

/* ========== HELPERS ========== */
bool CheckPointInsideRect(Vector2 point, UIRect rect);
Rectangle UIRectToRaylib(UIRect rect);

/* ========== WIDGET LOGIC ========== */
ButtonState ButtonLogic(UIContext *ctx, UIRect rect, int id);
ButtonState CheckBoxLogic(UIContext *ctx, UIRect rect, int *enabled, int id);
ButtonState SliderLogic(UIContext *ctx, UIRect rect, int id, float min_value, float max_value, float *value);

/* ========== RENDERING ========== */
void Label(UIRect rect, int fontsize, Color color, char *text);
void Panel(UIRect rect, PanelStyle style);
void DrawButtonBevel(UIRect rect, ButtonState state, int bevel, ButtonStyle style);
void DrawButtonBevelText(UIRect rect, ButtonState state, int bevel, ButtonStyle style, int fontsize, char *text);
void DrawCheckBox(UIRect rect, CheckBoxStyle style, ButtonState state, int enabled, int bevel);
void DrawPauseButton(UIRect rect, ButtonStyle style, ButtonState state, int enabled, int bevel);
void DrawSlider(UIRect rect, ButtonState state, int bevel, PanelStyle panel_style, ButtonStyle button_style, float value, float max_value);

/* ========== LAYOUT ========== */
UILayout UI_NewLayout(UIRect bounds, Direction direction, int spacing, int padding_x, int padding_y);
UIRect UI_CropLayout(UILayout *layout);
UIRect UI_NextRow(UILayout *layout, int height);
UIRect UI_NextColumn(UILayout *layout, int width);
UIRect UI_NextColumnFixedHeight(UILayout *layout, int width, int height);

/* ========== API BOUNDS ========== */
void UI_Begin(UIContext *ctx);
void UI_End(UIContext *ctx);