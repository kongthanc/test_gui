
#pragma once
#include "raylib.h"

namespace ui_layout {
    const float BASE_WIDTH = 1220.0f;
    const float BASE_HEIGHT = 750.0f;

    inline float scale() {
        float sx = GetScreenWidth() / BASE_WIDTH;
        float sy = GetScreenHeight() / BASE_HEIGHT;
        return sx < sy ? sx : sy;
    }

    inline Vector2 offset() {
        float s = scale();
        return {
            (GetScreenWidth() - BASE_WIDTH * s) / 2.0f,
            (GetScreenHeight() - BASE_HEIGHT * s) / 2.0f
        };
    }

    inline Camera2D camera() {
        Camera2D c = {0};
        c.offset = offset();
        c.target = {0, 0};
        c.rotation = 0;
        c.zoom = scale();
        return c;
    }

    inline Vector2 mouse_position() {
        Vector2 p = GetMousePosition();
        Vector2 o = offset();
        float s = scale();

        return {
            (p.x - o.x) / s,
            (p.y - o.y) / s
        };
    }
}
