#include "beton.h"
#include <math.h>
#include <stddef.h>

/* ========== HELPERS ========== */
bool CheckPointInsideRect(Vector2 point, UIRect rect) {
    return (point.x >= rect.x && point.x <= rect.x + rect.width) &&
        (point.y >= rect.y && point.y <= rect.y + rect.height);
}

Rectangle UIRectToRaylib(UIRect rect) {
    return (Rectangle){
        rect.x,
        rect.y,
        rect.width,
        rect.height,
    };
}

/* ========== INPUT ========== */
MouseInput GetMouseInput() {
    MouseInput mouse;
    mouse.pos = (Vector2){GetMouseX(), GetMouseY()};
    mouse.mouse_down = IsMouseButtonDown(0);
    mouse.mouse_pressed = IsMouseButtonPressed(0);
    mouse.mouse_released = IsMouseButtonReleased(0);

    return mouse;
}

/* ========== WIDGET LOGIC ========== */
ButtonState ButtonLogic(UIContext *ctx, UIRect rect, int id) {
    ButtonState state;

    if (ctx->captured_id == id || ctx->captured_id == -1) { // if this captured or no capture
        state.hovered = CheckPointInsideRect(ctx->mouse_input.pos, rect);
        state.pressed = state.hovered && ctx->mouse_input.mouse_down;
        state.clicked = state.hovered && ctx->mouse_input.mouse_released;

        if (ctx->captured_id == -1 && state.pressed) { // capture input
            ctx->captured_id = id;
        }
    } else {
        state.hovered = false;
        state.pressed = false;
        state.clicked = false;
    }

    return state;
}

ButtonState CheckBoxLogic(UIContext *ctx, UIRect rect, int *enabled, int id) {
    ButtonState state = ButtonLogic(ctx, rect, id);

    if (state.clicked) {
        *enabled = !(*  enabled);
    }
    return state;
}

ButtonState SliderLogic(UIContext *ctx, UIRect rect, int id, float min_value, float max_value, float *value) {
    ButtonState state;


    if (ctx->captured_id == id || ctx->captured_id == -1) { // if this captured or no capture        
        state.hovered = CheckPointInsideRect(ctx->mouse_input.pos, rect);
        state.pressed = (state.hovered || ctx->captured_id == id) && ctx->mouse_input.mouse_down;
        state.clicked = state.hovered && ctx->mouse_input.mouse_released;

        if (ctx->captured_id == -1 && state.pressed) { // capture input
            ctx->captured_id = id;
        }
        if (ctx->captured_id == id) {
            *value = fminf(fmaxf((ctx->mouse_input.pos.x - rect.x - rect.height * 0.5f) / (rect.width - rect.height), 0), 1.0f) * (max_value - min_value) + min_value;
        }
    } else {
        state.hovered = false;
        state.pressed = false;
        state.clicked = false;
    }

    return state;
}

/* ========== RENDERING ========== */
void Label(UIRect rect, int fontsize, Color color, char *text) {
    size_t text_width = MeasureText(text, fontsize);
    DrawText(text, rect.x + (rect.width - text_width) * 0.5, rect.y + rect.height * 0.5 - fontsize * 0.5, fontsize, color);
}

void Panel(UIRect rect, PanelStyle style) {
    DrawRectangleRec(UIRectToRaylib(rect), style.bg);
    DrawRectangleLinesEx(UIRectToRaylib(rect), 1, style.border);
}

void DrawBevel(UIRect rect, int bevel, Color color) {
    DrawRectangle(rect.x + rect.width - bevel - 1, rect.y + bevel + 1, 
        bevel, rect.height - bevel - 1, color);
    DrawRectangle(rect.x + bevel + 1, rect.y + rect.height - bevel - 1, 
        rect.width - bevel * 2, bevel, color);

    DrawTriangle((Vector2){rect.x + 1, rect.y + rect.height}, 
        (Vector2){rect.x + bevel + 1, rect.y + rect.height}, 
        (Vector2){rect.x + bevel + 1, rect.y + rect.height - bevel}, color);
    DrawTriangle((Vector2){rect.x + rect.width - bevel, rect.y + bevel + 1}, 
        (Vector2){rect.x + rect.width, rect.y + bevel}, 
        (Vector2){rect.x + rect.width, rect.y}, color);
}

void DrawBevelAround(UIRect rect, int bevel, Color light, Color dark) {
    DrawRectangle(rect.x + 1, rect.y + 1, bevel, rect.height - 1, light);
    DrawRectangle(rect.x + bevel + 1, rect.y + 1, rect.width - bevel - 1, bevel, light);

    DrawBevel(rect, bevel, dark);
}

void DrawButtonBevel(UIRect rect, ButtonState state, int bevel, ButtonStyle style) {
    if (state.hovered && !state.pressed) {
        DrawRectangleRec(UIRectToRaylib(rect), style.light);

        DrawBevel(rect, bevel, style.dark);
    }
    else if (state.pressed) {
        DrawRectangleRec(UIRectToRaylib(rect), style.dark);

        DrawBevel(rect, bevel, style.light);
    }
    else {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, style.bg);

        DrawBevelAround(rect, bevel, style.light, style.dark);
    } 
    DrawRectangleLinesEx(UIRectToRaylib(rect), 1, BLACK);
}

void DrawButtonBevelText(UIRect rect, ButtonState state, int bevel, ButtonStyle style, int fontsize, char *text) {
    DrawButtonBevel(rect, state, bevel, style);
    Label(rect, fontsize, style.border, text);
}

void DrawCheckBox(UIRect rect, CheckBoxStyle style, ButtonState state, int enabled, int bevel) {    
    if (state.pressed) {
        DrawRectangleRec(UIRectToRaylib(rect), style.dark);
    } else {
        DrawRectangleRec(UIRectToRaylib(rect), style.bg);

        DrawRectangle(rect.x + 1, rect.y + 1, bevel, rect.height - 1, style.dark);
        DrawRectangle(rect.x + bevel + 1, rect.y + 1, rect.width - bevel - 1, bevel, style.dark);
    }    
    DrawRectangleLinesEx(UIRectToRaylib(rect), 1, style.border);

    if (enabled == 1) {
        DrawRectangle(rect.x + 5, rect.y + 5, rect.width - 10, rect.height - 10, style.mark);
    }
}

void DrawPauseButton(UIRect rect, ButtonStyle style, ButtonState state, int enabled, int bevel) {    
    DrawButtonBevel(rect, state, bevel, style);

    if (enabled == 1) {
        DrawRectangle(rect.x + rect.width * 0.25, rect.y + rect.height * 0.2,  rect.height * 0.15, rect.height * 0.6, style.border);
        DrawRectangle(rect.x + rect.width * 0.6, rect.y + rect.height * 0.2,  rect.height * 0.15, rect.height * 0.6, style.border);
    } else {
        DrawTriangle((Vector2){rect.x + rect.width * 0.25, rect.y + rect.height * 0.2}, 
        (Vector2){rect.x + rect.width * 0.25, rect.y + rect.height * 0.8}, 
        (Vector2){rect.x + rect.width * 0.75, rect.y + rect.height * 0.5}, style.border);
    }
}

void DrawSlider(UIRect rect, ButtonState state, int bevel, PanelStyle panel_style, ButtonStyle button_style, float value, float max_value) {
    float size = rect.height;
    float x = rect.x + (rect.width - rect.height) * value / max_value;
    UIRect slider_rect = (UIRect){x, rect.y, size, size};
    UIRect rail_rect = (UIRect){rect.x, rect.y + rect.height * 0.25f, rect.width, rect.height * 0.5f};

    Panel(rail_rect, panel_style);
    // DrawBevelAround(rail_rect, 1, GRAY, BLACK);
    // DrawBevel(rail_rect, 1, GRAY);
    DrawBevelAround(rail_rect, 1, BLACK, GRAY);
    DrawButtonBevel(slider_rect, state, bevel, button_style);
}

/* ========== LAYOUT ========== */
UILayout UI_NewLayout(UIRect bounds, Direction direction, int spacing, int padding_x, int padding_y) {
    UILayout layout = {
        bounds,
        direction,
        bounds.x,
        bounds.y,
        spacing,
        padding_x,
        padding_y
    };

    return layout;
}

UIRect UI_CropLayout(UILayout *layout) {
    UIRect r = layout->bounds;
    if (layout->direction == VERTICAL) { // layout is vertical
        r.height = layout->cursor_y - layout->bounds.y;
    }
    else { // layout is horizontal
        r.width = layout->cursor_x - layout->bounds.x;
    }

    return r;
}

UIRect UI_NextRow(UILayout *layout, int height) {
    if (layout->cursor_y == layout->bounds.y)
        layout->cursor_y += layout->spacing;
    UIRect r = {
        layout->cursor_x + layout->padding_x,
        layout->cursor_y + layout->padding_y,
        layout->bounds.width - layout->padding_x * 2,
        height - layout->padding_y * 2
    };

    layout->cursor_y += height + layout->spacing;

    return r;
}

UIRect UI_NextColumn(UILayout *layout, int width) {
    if (layout->cursor_x == layout->bounds.x)
        layout->cursor_x += layout->spacing;
    UIRect r = {
        layout->cursor_x + layout->padding_x,
        layout->cursor_y + layout->padding_y,
        width - layout->padding_x * 2,
        layout->bounds.height - layout->padding_y * 2
    };

    layout->cursor_x += width + layout->spacing;

    return r;
}

UIRect UI_NextColumnFixedHeight(UILayout *layout, int width, int height) {
    if (layout->cursor_x == layout->bounds.x)
        layout->cursor_x += layout->spacing;
    int offset = (layout->bounds.height - layout->padding_y * 2 - height) / 2;
    UIRect r = {
        layout->cursor_x + layout->padding_x,
        layout->cursor_y + layout->padding_y + offset,
        width - layout->padding_x * 2,
        height
    };

    layout->cursor_x += width + layout->spacing;

    return r;
}

/* ========== API BOUNDS ========== */
void UI_Begin(UIContext *ctx) {
    ctx->mouse_input = GetMouseInput();
}

void UI_End(UIContext *ctx) {
    if (!ctx->mouse_input.mouse_down) { // release capture
        ctx->captured_id = -1;
    }
}