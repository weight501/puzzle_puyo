#pragma once
#include <SDL3/SDL.h>

/**
 *
 * ����: ������Ʈ Event �߻�ȭ Interface
 * 1. SDL_Event ������ ���� ó���� �ʿ��� Ŭ������ �� Interface�� ��� ����
 *
 */


class IEventHandler 
{

public:
    virtual ~IEventHandler() = default;
    virtual void HandleEvent(const SDL_Event& event) = 0;
};