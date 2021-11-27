#ifndef ABYSS_SDL2SYSTEMIO_H
#define ABYSS_SDL2SYSTEMIO_H

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

        void RunMainLoop() final;

        void Stop() final;

        std::unique_ptr<ITexture> CreateTexture(uint32_t width, uint32_t height) final;

      private:
        SDL_Window *_sdlWindow;
        SDL_Renderer *_sdlRenderer;
        bool _runMainLoop;

        void HandleSdlEvent(const SDL_Event &sdlEvent);
    };

} // namespace AbyssEngine::SystemIO::SDL2

#endif // ABYSS_SDL2SYSTEMIO_H
