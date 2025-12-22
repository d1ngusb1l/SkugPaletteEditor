#pragma once
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

class EyeDropper {
public:
    // Singleton access
    static EyeDropper& Get() {
        static EyeDropper instance;
        return instance;
    }

    // Call this when the button is clicked to enter "Picking Mode"
    void Start(float* targetColor);

    // Call this every frame (e.g., in UI::Render or Drawing::Draw)
    void Update();

    // Check if we are currently busy picking
    bool IsActive() const { return m_bActive; }

private:
    bool m_bActive = false;
    float* m_TargetColor = nullptr; // Pointer to the float[4] we want to change
    ImVec4 m_PreviewColor;          // The color currently under the cursor
};