#include "sdl2texture.h"

AbyssEngine::SystemIO::SDL2::SDL2Texture::SDL2Texture(SDL_Renderer *renderer, uint32_t width, uint32_t height)
    : _texture(nullptr), _renderer(renderer), _width(width), _height(height) {
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, (int)width, (int)height);
}

AbyssEngine::SystemIO::SDL2::SDL2Texture::~SDL2Texture() {
    if (_texture != nullptr)
        SDL_DestroyTexture(_texture);
}

void AbyssEngine::SystemIO::SDL2::SDL2Texture::SetPixels(std::span<const uint32_t> pixels) {
    if (pixels.size() != (_width * _height))
        throw std::runtime_error("Attempted to set pixels on the texture, but the data was the incorrect size.");

    SDL_UpdateTexture(_texture, nullptr, pixels.data(), 4 * (int)_width);
}
void AbyssEngine::SystemIO::SDL2::SDL2Texture::Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) {
    SDL_Rect sr{.x = sourceRect.X, .y = sourceRect.Y, .w = sourceRect.Width, .h = sourceRect.Height};
    SDL_Rect dr{.x = destRect.X, .y = destRect.Y, .w = destRect.Width, .h = destRect.Height};

    SDL_RenderCopy(_renderer, _texture, &sr, &dr);
}
