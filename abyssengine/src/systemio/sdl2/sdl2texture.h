#ifndef ABYSS_SDL2TEXTURE_H
#define ABYSS_SDL2TEXTURE_H

#include <SDL2/SDL.h>
#include "../interface.h"

namespace AbyssEngine::SDL2 {

class SDL2Texture : public ITexture {
  public:
    SDL2Texture(SDL_Renderer *renderer, ITexture::Format textureFormat, uint32_t width, uint32_t height);
    ~SDL2Texture() override;
    void SetPixels(std::span<const uint32_t> pixels) final;
    void Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) final;
    void SetYUVData(const Rectangle &rect, std::span<uint8_t> yPlane, int yPitch, std::span<uint8_t> uPlane, int uPitch, std::span<uint8_t> vPlane,
                    int vPitch) final;

  private:
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    const uint32_t _width;
    const uint32_t _height;
    const ITexture::Format _textureFormat;
};

}

#endif // ABYSS_SDL2TEXTURE_H
