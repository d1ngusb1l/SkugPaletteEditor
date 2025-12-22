#include "pch.h"
#include "UI.h"
#include "DirectXWrapper/common.h"
#include "Drawing.h"
#include "SetStyleImGui.h"
#include "DreamOrphans.cpp"
#include "EyeDropper.h"

// Static members initialization
bool UI::s_initialized = false;
bool UI::s_visible = true;
HWND UI::s_hwnd = nullptr;
IDirect3DDevice9* UI::s_device = nullptr;
WNDPROC UI::s_originalWndProc = nullptr;

auto UILogger = LOGGER::createLocal("UI", LogLevel::DEBUG_LOG);

bool UI::Initialize(HWND hwnd, IDirect3DDevice9* device)
{
    if (s_initialized)
        return true;

    if (!hwnd || !device)
        return false;

    s_hwnd = hwnd;
    s_device = device;

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = NULL;
    //io.Fonts->AddFontDefault();



    //ImFont* font = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
    //    DreamOrphans_compressed_data_base85,
    //    13.0f
    //);

    // �����: ���������� ����������� ����� ��� ��������
    //io.FontDefault = font;
    // Set style
    //CherryTheme();
    // Initialize platform/renderer backends
    if (!ImGui_ImplWin32_Init(hwnd))
    {
        LOG_LOCAL_CRITICAL(UILogger, "Failed to initialize ImGui Win32 backend");
        return false;
    }

    if (!ImGui_ImplDX9_Init(device))
    {
        LOG_LOCAL_CRITICAL(UILogger, "Failed to initialize ImGui DX9 backend");
        ImGui_ImplWin32_Shutdown();
        return false;
    }


    // Hook WndProc
    s_originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

    s_initialized = true;
    LOG_LOCAL_INFO(UILogger, "ImGui UI initialized successfully");
    return true;
}

void UI::Shutdown()
{
    if (!s_initialized)
        return;

    // Restore original WndProc
    if (s_hwnd && s_originalWndProc)
        SetWindowLongPtr(s_hwnd, GWLP_WNDPROC, (LONG_PTR)s_originalWndProc);

    // Cleanup ImGui
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    s_initialized = false;
    s_hwnd = nullptr;
    s_device = nullptr;
    s_originalWndProc = nullptr;

    LOG_LOCAL_INFO(UILogger, "ImGui UI shutdown");
}

void UI::Render()
{
    if (!s_initialized || !s_visible)
        return;

    // ���������, ������� �� ����������
    if (s_device && SUCCEEDED(s_device->TestCooperativeLevel()))
    {
        BeginFrame();
        Drawing::Draw();
        EndFrame();
    }
}

void UI::BeginFrame()
{
    if (!s_initialized)
        return;

    // Start the ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void UI::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        // FULLSCREEN CRASH GUARD:
        // DX9 will crash if we try to RenderPlatformWindows while in Exclusive Fullscreen.
        WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
        GetWindowPlacement(s_hwnd, &placement);
        bool isFullscreen = (placement.showCmd == SW_SHOWMAXIMIZED); // Simplified check

        if (!isFullscreen) 
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            s_device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
        }
    }
}

LRESULT CALLBACK UI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (EyeDropper::Get().IsActive()) {
        // Force capture so we get mouse move/clicks outside the window
        if (::GetCapture() != hWnd) ::SetCapture(hWnd);

        // Update ImGui's internal state so the picker knows where the mouse is
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

        // THE FIX: Return 0 for ALL mouse and keyboard messages.
        // This makes the picker "modal" and prevents the game/UI from reacting.
        if ((msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) || (msg >= WM_KEYFIRST && msg <= WM_KEYLAST)) {
            return 0; 
        }
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    return CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
}



// LRESULT CALLBACK UI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
// {   
//     if (msg == WM_SIZE)
//     {
//         // ��� ��������� ������� ���� ������������ ����������
//         // ��� �������� ���������� ������� Reset()
//         if (UI::IsInitialized() && s_device != NULL)
//         {
//             // ����� �������� �������������� ������ ����� ���� �����
//         }
//     }
    
    
    
//     //// ���� ������� �������, ��������� ��� ��������� �� ���� � ����������
//     //if (!EyeDropper::getInstance().IsThreadFinished())
//     //{
//     //    // ��������� ��� ��������� ���� � ����������
//     //    if ((msg >= WM_LBUTTONDOWN && msg <= WM_MOUSELAST) ||
//     //        (msg >= WM_KEYFIRST && msg <= WM_KEYLAST))
//     //    {
//     //        // ���������� 0, ����� �������, ��� ��������� ����������
//     //        return 0;
//     //    }

//     //    // ��� ������ ��������� ���������� ������� ���������
//     //    // (��������, WM_PAINT, WM_SIZE � �.�.)
//     //}

//     // ������ �������� ��������� � ImGui ��� ���������
//     ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);



//     // �������� ��������� ImGui
//     ImGuiIO& io = ImGui::GetIO();

//     // ��������� ��������� ����, ���� ImGui ����� �� ����������
//     if (((msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) && io.WantCaptureMouse) && s_visible)
//     {
//         return true;
//     }

//     // ��������� ��������� ����������, ���� ImGui ����� �� ����������
//     if (((msg >= WM_KEYFIRST && msg <= WM_KEYLAST) && io.WantCaptureKeyboard) && s_visible)
//     {
//         return true;
//     }

//     return CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
// }