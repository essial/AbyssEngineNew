#ifndef ABYSS_SDL2SYSTEMIO_H
#define ABYSS_SDL2SYSTEMIO_H

#include "../../common/ringbuffer.h"
#include "../interface.h"
#include <SDL2/SDL.h>
#include <string>

namespace AbyssEngine::SDL2 {

class SDL2SystemIO : public SystemIO {
  public:
    SDL2SystemIO();
    ~SDL2SystemIO() override;

    std::string_view Name() final;
    void PauseAudio(bool pause) final;
    void SetFullscreen(bool fullscreen) final;
    void RunMainLoop(Node &rootNode) final;
    void Stop() final;
    std::unique_ptr<ITexture> CreateTexture(ITexture::Format textureFormat, uint32_t width, uint32_t height) final;
    void PushAudioData(std::span<const char> data) final;

  private:
    void InitializeAudio();
    void FinalizeAudio() const;
    static void HandleAudioCallback(void *userData, Uint8 *stream, int length);
    void HandleAudio(uint8_t *stream, int length);
    void HandleSdlEvent(const SDL_Event &sdlEvent, Node &rootNode);

    SDL_Window *_sdlWindow;
    SDL_Renderer *_sdlRenderer;
    bool _runMainLoop = false;
    bool _hasAudio = false;
    SDL_AudioSpec _audioSpec;
    SDL_AudioDeviceID _audioDeviceId = 0;
    RingBuffer _audioBuffer;
};

} // namespace AbyssEngine::SDL2

#endif // ABYSS_SDL2SYSTEMIO_H
