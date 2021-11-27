#ifndef ABYSS_SDL2TEXTURE_H
#define ABYSS_SDL2TEXTURE_H

#include <SDL2/SDL.h>
#include "../interface.h"

namespace AbyssEngine::SystemIO::SDL2 {

class SDL2Texture : public ITexture {
  public:
    SDL2Texture(SDL_Renderer *renderer, uint32_t width, uint32_t height);
    ~SDL2Texture() override;
    void SetPixels(std::span<const uint32_t> pixels) final;
    void Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) final;

  private:
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    uint32_t _width;
    uint32_t _height;
};

}

#endif // ABYSS_SDL2TEXTURE_H
