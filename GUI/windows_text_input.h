#ifndef WINDOWS_TEXT_INPUT_H
#define WINDOWS_TEXT_INPUT_H

#ifdef _WIN32
// Raylib phai duoc include truoc Windows API vi hai ben co mot so ten trung nhau.
#include "raylib.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Doi ten cac ham Win32 trong luc include Windows headers de tranh xung dot
// voi Rectangle, CloseWindow va ShowCursor cua Raylib.
#define Rectangle Win32Rectangle
#define CloseWindow Win32CloseWindow
#define ShowCursor Win32ShowCursor
#include <windows.h>
#include <commctrl.h>
#undef Rectangle
#undef CloseWindow
#undef ShowCursor

// Windows dinh nghia DrawText/DrawTextEx thanh macro, gay xung dot voi ui_font.h.
#ifdef DrawText
#undef DrawText
#endif
#ifdef DrawTextEx
#undef DrawTextEx
#endif

#include <deque>
#include <cstdint>

namespace windows_text_input {
    enum class event_type { character, backspace };

    struct input_event {
        event_type type;
        int codepoint;
    };

    inline std::deque<input_event> events;
    inline wchar_t pending_high_surrogate = 0;
    inline HWND window_handle = nullptr;
    inline constexpr UINT_PTR subclass_id = 0x50424C32;

    inline void push_character(int codepoint) {
        if (codepoint >= 32 && codepoint <= 0x10FFFF &&
            !(codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
            events.push_back({event_type::character, codepoint});
        }
    }

    inline void push_utf16(wchar_t value) {
        if (value >= 0xD800 && value <= 0xDBFF) {
            pending_high_surrogate = value;
            return;
        }

        if (value >= 0xDC00 && value <= 0xDFFF) {
            if (pending_high_surrogate != 0) {
                int codepoint = 0x10000 +
                    ((pending_high_surrogate - 0xD800) << 10) +
                    (value - 0xDC00);
                pending_high_surrogate = 0;
                push_character(codepoint);
            }
            return;
        }

        pending_high_surrogate = 0;
        push_character((int)value);
    }

    inline LRESULT CALLBACK window_proc(
        HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam,
        UINT_PTR, DWORD_PTR) {

        if (message == WM_KEYDOWN && wparam == VK_BACK) {
            events.push_back({event_type::backspace, 0});
            return DefSubclassProc(hwnd, message, wparam, lparam);
        }

        if (message == WM_CHAR || message == WM_SYSCHAR || message == WM_IME_CHAR) {
            if (wparam == 8 || wparam == 0) return 0;
            push_utf16((wchar_t)wparam);
            return 0;
        }

        if (message == WM_UNICHAR) {
            if (wparam == UNICODE_NOCHAR) return TRUE;
            push_character((int)wparam);
            return 0;
        }

        return DefSubclassProc(hwnd, message, wparam, lparam);
    }

    inline bool install() {
        window_handle = (HWND)GetWindowHandle();
        if (window_handle == nullptr) return false;
        return SetWindowSubclass(window_handle, window_proc, subclass_id, 0) != FALSE;
    }

    inline void uninstall() {
        if (window_handle != nullptr) {
            RemoveWindowSubclass(window_handle, window_proc, subclass_id);
            window_handle = nullptr;
        }
        events.clear();
        pending_high_surrogate = 0;
    }

    inline bool pop(input_event& event) {
        if (events.empty()) return false;
        event = events.front();
        events.pop_front();
        return true;
    }
}
#endif

#endif
