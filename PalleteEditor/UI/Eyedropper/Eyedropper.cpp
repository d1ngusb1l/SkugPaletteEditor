#include "pch.h"
#include "Eyedropper.h"

EyeDropper::EyeDropper()
    : is_active_(false)
    , capture_hwnd_(nullptr)
    , color_under_cursor_(CLR_INVALID)
    , is_thread_finished_(true)
    , was_cancelled_(false)
{
    cursor_pos_ = { 0, 0 };
}

EyeDropper::~EyeDropper() {
    StopEyedropper();
}

void EyeDropper::StartEyedropper(float col[4]) {
    if (is_active_.load()) {
        return;
    }

    StopEyedropper();

    is_thread_finished_.store(false);
    was_cancelled_.store(false);

    is_active_.store(true);

    worker_thread_ = std::make_unique<std::thread>([this, col]() {
        RunEyedropperLoop(col);
        });
}

void EyeDropper::StopEyedropper() {

    is_active_.store(false);


    if (worker_thread_ && worker_thread_->joinable()) {
        if (worker_thread_->get_id() != std::this_thread::get_id()) {
            worker_thread_->join();
            worker_thread_.reset();
        }
        else {
            worker_thread_->detach();
            worker_thread_.reset();
        }
    }

}

POINT EyeDropper::GetCursorPos() const {
    return cursor_pos_;
}

COLORREF EyeDropper::GetColorUnderCursor() const {
    return color_under_cursor_;
}

ImVec4 EyeDropper::GetColorUnderCursorImVec4() const {
    ImVec4 return_value;
    return_value.x = static_cast<float>(GetRValue(color_under_cursor_)) / 255.0f;
    return_value.y = static_cast<float>(GetGValue(color_under_cursor_)) / 255.0f;
    return_value.z = static_cast<float>(GetBValue(color_under_cursor_)) / 255.0f;
    return_value.w = 1.0f; // ��������� �����-�����
    return return_value;
}


COLORREF EyeDropper::GetPixelColorAtCursor() const {
    POINT cursorPos = GetCursorPos();

    // Get a Device Context (DC) for the entire screen
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return CLR_INVALID;

    // Read the COLORREF (0x00BBGGRR format) at the cursor
    COLORREF pixelColor = GetPixel(hdcScreen, cursorPos.x, cursorPos.y);

    ReleaseDC(NULL, hdcScreen); // Clean up the DC
    return pixelColor;
}

void EyeDropper::RunEyedropperLoop(float col[4]) {
    LOG_INFO("Eyedropper active. Move mouse, click to capture, press ESC to cancel.");

    is_thread_finished_.store(false);
    was_cancelled_.store(false);

    if (capture_hwnd_) {
        SetCapture(capture_hwnd_);
        LOG_DEBUG("Mouse captured for window");
    }


    HCURSOR hPrevCursor = GetCursor();

    // ������ ������ �� �������
    HCURSOR hEyeDropperCursor = LoadCursor(NULL, IDC_CROSS);
    if (hEyeDropperCursor) {
        SetCursor(hEyeDropperCursor);
    }

    try {
        while (is_active_.load()) {
            POINT currentPos;
            ::GetCursorPos(&currentPos);  // ���������� WinAPI ��������

            // �������� ���� �������
            HDC hdcScreen = GetDC(NULL);
            COLORREF pixelColor = CLR_INVALID;
            if (hdcScreen) {
                pixelColor = GetPixel(hdcScreen, currentPos.x, currentPos.y);
                ReleaseDC(NULL, hdcScreen);
            }

            // ��������� ��������� (���������������)
            {
                std::lock_guard<std::mutex> lock(cursor_mutex_);
                cursor_pos_ = currentPos;
                color_under_cursor_ = pixelColor;
            }

            // ��������� �������
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                LOG_INFO("Eyedropper cancelled.");
                is_active_.store(false);
                was_cancelled_.store(true);
                break;
            }
            if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                LOG_INFO("Eyedropper cancelled by right click.");
                is_active_.store(false);
                was_cancelled_.store(true);

                break;
            }

            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                COLORREF color = pixelColor;

                // ��������� RGB ����������
                int r = GetRValue(color);
                int g = GetGValue(color);
                int b = GetBValue(color);
                LOG_DEBUG("Color captured: RGB", r, g, b);

                // ������������ � float (0.0 - 1.0)
                col[0] = static_cast<float>(r) / 255.0f;
                col[1] = static_cast<float>(g) / 255.0f;
                col[2] = static_cast<float>(b) / 255.0f;
                col[3] = 1.0f;

                LOG_INFO("Color captured successfully.");
                is_active_.store(false);
                was_cancelled_.store(false);
                Sleep(100);
                break;
            }

            // ��������� ������ ������ ���� ��� ������


            Sleep(10);
        }
    }
    catch (const std::runtime_error& e) {
        LOG_ERROR("Exception in eyedropper loop");
        LOG_ERROR(e.what());
    }

    // ��������������� ������
    if (hPrevCursor) {
        SetCursor(hPrevCursor);
    }

    // ����������� ������ ����
    if (capture_hwnd_) {
        ReleaseCapture();
        LOG_DEBUG("Mouse released");
    }

    LOG_INFO("Eyedropper stopped.");
    is_thread_finished_.store(true);
}

void EyeDropper::EyeDropperToolTip() {
    if (!is_active_) return;
    ImGui::BeginTooltip();
    ImVec4 color = GetColorUnderCursorImVec4();

    ImGui::ColorButton("Eyedropper_Tooltip_Color", color);
    int r = (int)(color.x * 255);
    int g = (int)(color.y * 255);
    int b = (int)(color.z * 255);

    // Десятичные значения
    ImGui::Text("RGB: %d, %d, %d", r, g, b);

    // Шестнадцатеричное значение (опционально)
    ImGui::Text("Hex: #%02X%02X%02X", r, g, b);

    ImGui::Separator();

    ImGui::Text("Press ESC or Right Mouse Button to abort picking.");
    ImGui::EndTooltip();

}