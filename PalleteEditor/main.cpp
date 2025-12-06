#include <thread>
#include "UI.h"
#include "Config.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    config::init();
    UI::Render();
    return 0;
}
