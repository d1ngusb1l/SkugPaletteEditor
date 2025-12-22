#include "pch.h"
#include <atomic>
#include <mutex>
#include <thread>

class EyeDropper {
public:
    // Static method to access the single instance
    static EyeDropper& getInstance() {
        static EyeDropper instance;
        return instance;
    }

    // ������ ������� � ��������� ������
    void StartEyedropper(float col[4]);

    // ��������� �������
    void StopEyedropper();

    // �������� ���������� �������
    bool IsActive() const { return is_active_.load(); }
    bool IsThreadFinished() const { return is_thread_finished_.load(); }
    bool WasCancelled() const { return was_cancelled_.load(); }
    // ��������� ������� ������� ������� (���������������)
    POINT GetCursorPos() const;

    // ��������� �������� ����� ��� �������� (���������������)
    COLORREF GetColorUnderCursor() const;
    ImVec4 GetColorUnderCursorImVec4() const;


    // ��������� HWND ���� ��� ������� ����
    void SetCaptureWindow(HWND hwnd) { capture_hwnd_ = hwnd; }

    void EyeDropperToolTip();
private:
    void MouseHookFunc();
    // Private constructor to prevent direct instantiation
    EyeDropper();

    // Destructor
    ~EyeDropper();

    // Delete copy constructor and assignment operator to prevent copying
    EyeDropper(const EyeDropper&) = delete;
    EyeDropper& operator=(const EyeDropper&) = delete;

    // �������� ���� �������
    void RunEyedropperLoop(float col[4]);

    // ��������� ����� ������� ��� ��������
    COLORREF GetPixelColorAtCursor() const;

    // ����� ��� ������ �������
    std::unique_ptr<std::thread> worker_thread_;

    // ���� ���������� �������
    std::atomic<bool> is_active_;
    std::atomic<bool> is_thread_finished_;
    std::atomic<bool> was_cancelled_;
    // ������� �������
    mutable std::mutex cursor_mutex_;
    POINT cursor_pos_;

    // ���� ��� ��������
    COLORREF color_under_cursor_;

    // ���� ��� ������� ����
    HWND capture_hwnd_;
};