#include "pch.h"
#include "EyeDropper.h"
#include <Windows.h>

void EyeDropper::Start(float* targetColor) {
    m_TargetColor = targetColor;
    m_bActive = true;
    // We don't SetCapture here to avoid breaking window dragging
}

void EyeDropper::Update() {
    if (!m_bActive) return;

    // 1. Hardware-level escape check 
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        m_bActive = false;
        m_TargetColor = nullptr;
        ::ReleaseCapture();
        return;
    }

    // 2. Fast Sample (No laggy screenshots)
    POINT mousePos;
    ::GetCursorPos(&mousePos);
    HDC hDC = ::GetDC(nullptr); // Desktop DC allows sampling outside game window
    COLORREF pixel = ::GetPixel(hDC, mousePos.x, mousePos.y);
    ::ReleaseDC(nullptr, hDC);

    float r = GetRValue(pixel) / 255.0f;
    float g = GetGValue(pixel) / 255.0f;
    float b = GetBValue(pixel) / 255.0f;
    m_PreviewColor = { r, g, b, 1.0f };

    // 3. Rendering to the Desktop-Level Overlay
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImDrawList* draw_list = ImGui::GetForegroundDrawList(vp);

    // Convert Screen Pos to Viewport-local Coordinates
    ImVec2 mouse_in_vp = { (float)mousePos.x - vp->Pos.x, (float)mousePos.y - vp->Pos.y };
    ImVec2 box_pos = { mouse_in_vp.x + 20, mouse_in_vp.y + 20 };
    ImVec2 box_end = { box_pos.x + 40, box_pos.y + 40 };

    // Draw the preview UI (Visible even on second monitors)
    draw_list->AddRect(ImVec2(box_pos.x - 1, box_pos.y - 1), ImVec2(box_end.x + 1, box_end.y + 1), IM_COL32(0, 0, 0, 255), 0.0f, 0, 2.0f);
    draw_list->AddRectFilled(box_pos, box_end, ImGui::ColorConvertFloat4ToU32(m_PreviewColor));
    draw_list->AddRect(box_pos, box_end, IM_COL32(255, 255, 255, 255));

    char hex_str[32];
    snprintf(hex_str, sizeof(hex_str), "#%02X%02X%02X", (int)(r * 255), (int)(g * 255), (int)(b * 255));
    draw_list->AddText({ box_end.x + 10, box_pos.y + 12 }, IM_COL32(255, 255, 255, 255), hex_str);

    // 4. Individual Click Selection
    // We check for the click and immediately terminate the mode
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        if (m_TargetColor) {
            m_TargetColor[0] = r; m_TargetColor[1] = g; m_TargetColor[2] = b;
        }
        m_bActive = false;
        ::ReleaseCapture(); 
    }
}