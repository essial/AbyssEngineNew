#include "sdl2systemio.h"
#include "config.h"
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_hints.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <spdlog/spdlog.h>

AbyssEngine::SystemIO::SDL2::SDL2SystemIO::SDL2SystemIO() : AbyssEngine::SystemIO::ISystemIO() {
    SPDLOG_TRACE("Creating SDL2 System IO");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) != 0)
        throw std::runtime_error(SDL_GetError());

    _sdlWindow = SDL_CreateWindow(ABYSS_VERSION_STRING, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                  SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    if (_sdlWindow == nullptr)
        throw std::runtime_error(SDL_GetError());

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME, "Abyss Engine");

    _sdlRenderer = SDL_CreateRenderer(_sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (_sdlRenderer == nullptr)
        throw std::runtime_error(SDL_GetError());

    SDL_RendererInfo renderInfo;
    SDL_GetRendererInfo(_sdlRenderer, &renderInfo);

    SPDLOG_INFO("Using '{0}' graphics rendering API", renderInfo.name);
    SPDLOG_INFO("Max texture size: {0}x{1}", renderInfo.max_texture_width, renderInfo.max_texture_height);

    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);

    SPDLOG_INFO("SDL Version: {0}.{1}.{2}", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

    PauseAudio(false);
}

AbyssEngine::SystemIO::SDL2::SDL2SystemIO::~SDL2SystemIO() {
    SPDLOG_TRACE("Destroying SDL2 System IO");

    if (_sdlRenderer != nullptr)
        SDL_DestroyRenderer(_sdlRenderer);

    if (_sdlWindow != nullptr)
        SDL_DestroyWindow(_sdlWindow);

    SDL_Quit();
}

std::string_view AbyssEngine::SystemIO::SDL2::SDL2SystemIO::Name() { return "SDL2"; }

void AbyssEngine::SystemIO::SDL2::SDL2SystemIO::HandleAudio(Uint8 *stream, int len) {
    if (!_runMainLoop)
        return;
}

void AbyssEngine::SystemIO::SDL2::SDL2SystemIO::PauseAudio(bool pause) {}

void AbyssEngine::SystemIO::SDL2::SDL2SystemIO::SetFullscreen(bool fullscreen) {
    SDL_SetWindowFullscreen(_sdlWindow, fullscreen ? SDL_TRUE : SDL_FALSE);
}

void AbyssEngine::SystemIO::SDL2::SDL2SystemIO::RunMainLoop() {
    SPDLOG_TRACE("Starting main loop");

    SDL_Event sdlEvent;

    _runMainLoop = true;
    while (_runMainLoop) {
        while (SDL_PollEvent(&sdlEvent)) {
            HandleSdlEvent(sdlEvent);
        }

        SDL_RenderClear(_sdlRenderer);
        SDL_RenderPresent(_sdlRenderer);
    }

    SPDLOG_TRACE("Leaving main loop");
}

void AbyssEngine::SystemIO::SDL2::SDL2SystemIO::HandleSdlEvent(const SDL_Event &sdlEvent) {
    switch (sdlEvent.type) {
    case SDL_QUIT:
        _runMainLoop = false;
        break;
    }
}
