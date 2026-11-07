#pragma once

#include "raylib.h"

// Khai bao font dung chung cho cac tep GUI.
extern Font app_font;

// He so phong to font chung cho toan bo ung dung.
const float FONT_SCALE = 1.10f;

inline void DrawReadableText(const char* text, int x, int y,
                             int font_size, Color color) {
    DrawTextEx(app_font, text, {(float)x, (float)y},
               (float)font_size * FONT_SCALE, 1.0f, color);
}

inline int MeasureReadableText(const char* text, int font_size) {
    Vector2 size = MeasureTextEx(app_font, text, (float)font_size * FONT_SCALE, 1.0f);
    return (int)size.x;
}

// Sau khi include tep nay, cac lenh DrawText va MeasureText tu dong dung Segoe UI.
#define DrawText(text, x, y, font_size, color) DrawReadableText(text, x, y, font_size, color)
#define MeasureText(text, font_size) MeasureReadableText(text, font_size)