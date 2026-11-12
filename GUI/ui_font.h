#pragma once

#include "raylib.h"

// Khai báo font dùng chung cho các tệp GUI.
extern Font app_font;

// Hệ số phóng to font chung cho toàn bộ ứng dụng.
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

// Sau khi include tệp này, các lệnh DrawText và MeasureText tự động dùng Segoe UI.
#define DrawText(text, x, y, font_size, color) DrawReadableText(text, x, y, font_size, color)
#define MeasureText(text, font_size) MeasureReadableText(text, font_size)
