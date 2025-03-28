#include "StringTexture.hpp"
#include "../core/manager/FontManager.hpp"
#include "../core/GameApp.hpp"
#include <format>
#include <stdexcept>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_pixels.h>
#include <Windows.h>
#include "../utils/Logger.hpp"

void StringTexture::RenderText(std::string_view text, const SDL_Color& textColor, StringEncoding encoding, FontType fontType)
{
    if (text.empty())
    {
        return;
    }

    try
    {
        Unload();

        auto& fontManager = GAME_APP.GetFontManager();        

        TTF_Font* font = fontManager.GetFont(fontType);
        if (!font)
        {
            throw std::runtime_error("Failed to get font");
        }

        std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> textSurface(
            CreateTextSurface(font, text, textColor, encoding),
            SDL_DestroySurface
        );

        if (!textSurface)
        {
            throw std::runtime_error(std::format("Failed to render text: {}", SDL_GetError()));
        }

        auto renderer = GAME_APP.GetRenderer();
        if (renderer == nullptr)
        {
            throw std::runtime_error("Renderer not available");
        }

        texture_ = SDL_CreateTextureFromSurface(renderer, textSurface.get());
        if (!texture_)
        {
            throw std::runtime_error(std::format("Failed to create texture: {}", SDL_GetError()));
        }

        if (encoding == StringEncoding::UTF8)
        {
            int w, h;
            if (TTF_GetStringSize(font, std::string(text).c_str(), 0, &w, &h) == false)
            {
                throw std::runtime_error(std::format("Failed to get text size: {}", SDL_GetError()));
            }
            width_ = static_cast<float>(w);
            height_ = static_cast<float>(h);
        }

        encoding_ = encoding;
    }
    catch (const std::exception& e)
    {
        SDL_LOG_ERROR(SDL_LOG_CATEGORY_APPLICATION, "Failed to render text: %s", e.what());
    }
}

void StringTexture::RenderUnicode(std::u16string_view text, const SDL_Color& textColor, FontType fontType)
{
    if (text.empty())
    {
        return;
    }

    try
    {
        Unload();

        auto& fontManager = GAME_APP.GetFontManager();        

        TTF_Font* font = fontManager.GetFont(fontType);
        if (!font)
        {
            throw std::runtime_error("Failed to get font");
        }

        // UTF-16 ���ڿ��� UTF-8�� ��ȯ
        std::string utf8str;
        {
            // UTF-16���� UTF-8�� ��ȯ�ϴµ� �ʿ��� ���� ũ�� ���
            int size = WideCharToMultiByte(
                CP_UTF8,                    // UTF-8 ���
                0,                          // �÷���
                reinterpret_cast<const wchar_t*>(text.data()), // UTF-16 ���ڿ�
                static_cast<int>(text.length()),               // ���ڿ� ����
                nullptr,                    // ��� ���� (ũ�� ����)
                0,                          // ��� ���� ũ�� (ũ�� ����)
                nullptr,                    // �⺻ ����
                nullptr                     // �⺻ ���� ��� ����
            );

            if (size == 0)
            {
                throw std::runtime_error("Failed to calculate buffer size for UTF-8 conversion");
            }

            // ���� �Ҵ� �� ���� ��ȯ ����
            utf8str.resize(size);
            if (WideCharToMultiByte(
                CP_UTF8,
                0,
                reinterpret_cast<const wchar_t*>(text.data()),
                static_cast<int>(text.length()),
                utf8str.data(),
                size,
                nullptr,
                nullptr
            ) == 0)
            {
                throw std::runtime_error("Failed to convert UTF-16 to UTF-8");
            }
        }

        SDL_Surface* textTexture = TTF_RenderText_Blended(font, utf8str.c_str(), 0, textColor);
        std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> textSurface(textTexture, SDL_DestroySurface);

        if (!textSurface)
        {
            throw std::runtime_error(std::format("Failed to render Unicode text: {}", SDL_GetError()));
        }

        auto* renderer = GAME_APP.GetRenderer();
        if (!renderer)
        {
            throw std::runtime_error("Renderer not available");
        }

        texture_ = SDL_CreateTextureFromSurface(renderer, textSurface.get());
        if (!texture_)
        {
            throw std::runtime_error(std::format("Failed to create texture: {}", SDL_GetError()));
        }

        int w, h;
        if (TTF_GetStringSize(font, utf8str.c_str(), 0, &w, &h) == false)
        {
            throw std::runtime_error(std::format("Failed to get text size: {}", SDL_GetError()));
        }
        width_ = static_cast<float>(w);
        height_ = static_cast<float>(h);

        encoding_ = StringEncoding::Unicode;
    }
    catch (const std::exception& e)
    {
        SDL_LOG_ERROR(SDL_LOG_CATEGORY_APPLICATION, "Failed to render Unicode text: %s", e.what());
    }
}

SDL_Surface* StringTexture::CreateTextSurface(TTF_Font* font, std::string_view text, const SDL_Color& color, StringEncoding encoding)
{
    if (encoding == StringEncoding::UTF8)
    {
        return TTF_RenderText_Blended(font, std::string(text).c_str(), 0, color);
    }
    return nullptr;
}