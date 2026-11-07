#pragma once
#include "raylib.h"

namespace ui_layout {
    // 1. Kích thước bản vẽ gốc của đồ án
    const float BASE_WIDTH = 1220.0f;
    const float BASE_HEIGHT = 750.0f;

    // 2. Thuật toán Auto-Scaling (Zoom thông minh)
    inline float scale() {
        float sx = (float)GetScreenWidth() / BASE_WIDTH;
        float sy = (float)GetScreenHeight() / BASE_HEIGHT;
        // Luôn chọn tỷ lệ nhỏ hơn để giao diện không bị cắt xén hay méo chữ
        return sx < sy ? sx : sy; 
    }

    // 3. Thuật toán Căn giữa (Tạo viền đen)
    inline Vector2 offset() {
        float s = scale();
        return {
            (GetScreenWidth() - BASE_WIDTH * s) / 2.0f,  // Chia đều khoảng trống theo chiều ngang
            (GetScreenHeight() - BASE_HEIGHT * s) / 2.0f // Chia đều khoảng trống theo chiều dọc
        };
    }

    // 4. Máy chiếu Camera 2D
    inline Camera2D camera() {
        Camera2D c = {0};
        c.offset = offset();
        c.target = {0.0f, 0.0f};
        c.rotation = 0.0f;
        c.zoom = scale(); // Áp dụng tỷ lệ phóng to/thu nhỏ vào máy chiếu
        return c;
    }

    // 5. Thuật toán Đồng bộ tọa độ chuột
    inline Vector2 mouse_position() {
        Vector2 p = GetMousePosition();
        Vector2 o = offset();
        float s = scale();

        // Dịch ngược tọa độ chuột thực tế của Windows về lại không gian 1220x750 của đồ án
        return {
            (p.x - o.x) / s,
            (p.y - o.y) / s
        };
    }
}