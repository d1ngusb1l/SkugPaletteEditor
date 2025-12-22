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

    // 1. Exit Logic
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        m_bActive = false;
        ::ReleaseCapture();
        return;
    }

    // 2. Sample Screen
    POINT mousePos;
    ::GetCursorPos(&mousePos);
    HDC hDC = ::GetDC(nullptr);
    COLORREF pixel = ::GetPixel(hDC, mousePos.x, mousePos.y);
    ::ReleaseDC(nullptr, hDC);

    float r = GetRValue(pixel) / 255.0f;
    float g = GetGValue(pixel) / 255.0f;
    float b = GetBValue(pixel) / 255.0f;
    m_PreviewColor = { r, g, b, 1.0f };

    // 3. The "No-Clip" Rendering Logic
    ImGuiIO& io = ImGui::GetIO();
    ImGuiViewport* vp = ImGui::GetMainViewport();
    
    // CRITICAL: We check if the viewport actually has a PlatformWindow.
    // If PlatformWindow == NULL, we are in Fullscreen and MUST draw on the main list.
    ImDrawList* draw_list = (vp->PlatformWindow) ? ImGui::GetForegroundDrawList(vp) : ImGui::GetForegroundDrawList();

    // Map coordinates: If viewports are active, use screen space. If not, use client space.
    ImVec2 draw_pos;
    if (vp->PlatformWindow) {
        draw_pos = { (float)mousePos.x, (float)mousePos.y };
    } else {
        ScreenToClient((HWND)vp->PlatformHandle, &mousePos);
        draw_pos = { (float)mousePos.x, (float)mousePos.y };
    }

    ImVec2 box_at = { draw_pos.x + 20, draw_pos.y + 20 };
    draw_list->AddRectFilled(box_at, {box_at.x + 30, box_at.y + 30}, ImGui::ColorConvertFloat4ToU32(m_PreviewColor));
    draw_list->AddRect(box_at, {box_at.x + 30, box_at.y + 30}, IM_COL32(255,255,255,255));

    // 4. Input Eating Logic (The "Individual Click" handler)
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        if (m_TargetColor) {
            m_TargetColor[0] = r; m_TargetColor[1] = g; m_TargetColor[2] = b;
        }
        m_bActive = false;
        ::ReleaseCapture();
    }
}