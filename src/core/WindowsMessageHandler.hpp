#pragma once

/**
 *
 * ����: windows system message ������ ���� handler
 * 1. ���� SDL_SYSWMEVENT�� deprecated�ǰ� SDL_SetWindowsMessageHook���� ��ŷ ������� ����
 * 2. ���� https://github.com/libsdl-org/SDL/blob/main/docs/README-migration.md
 */

#include <SDL3/SDL.h>

class WindowsMessageHandler 
{
public:
    WindowsMessageHandler() = default;
    ~WindowsMessageHandler() = default;
        
    WindowsMessageHandler(const WindowsMessageHandler&) = delete;
    WindowsMessageHandler& operator=(const WindowsMessageHandler&) = delete;
    WindowsMessageHandler(WindowsMessageHandler&&) = delete;
    WindowsMessageHandler& operator=(WindowsMessageHandler&&) = delete;;

    void SetupWindowsMessageHook();

private:
    static bool SDLCALL MyWindowsMessageHook(void* userdata, MSG* msg);
};
