#ifndef ABYSS_SDL2SYSTEMIO_H
#define ABYSS_SDL2SYSTEMIO_H

#include "../interface.h"
#include <SDL2/SDL.h>
#include <string>

namespace AbyssEngine::SystemIO::SDL2 {

class SDL2SystemIO : public AbyssEngine::SystemIO::ISystemIO {
  public:
    SDL2SystemIO();

    ~SDL2SystemIO() override;

    void HandleAudio(Uint8 *stream, int len);

    std::string_view Name() final;
    void PauseAudio(bool pause) final;
    void SetFullscreen(bool fullscreen) final;
    void RunMainLoop() final;

  private:
    SDL_Window *_sdlWindow;
    SDL_Renderer *_sdlRenderer;
    bool _runMainLoop;

    void HandleSdlEvent(const SDL_Event &sdlEvent);
};

} // namespace AbyssEngine::SystemIO::SDL2

#endif // ABYSS_SDL2SYSTEMIO_H
