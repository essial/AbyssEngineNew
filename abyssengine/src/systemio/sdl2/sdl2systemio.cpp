#include "sdl2systemio.h"
#include "../../hostnotify/hostnotify.h"
#include "../../node/node.h"
#include "../../node/sprite.h"
#include "config.h"
#include "sdl2texture.h"
#include <SDL.h>
#include <SDL_hints.h>
#include <SDL_stdinc.h>
#include <SDL_syswm.h>
#include <spdlog/spdlog.h>
#ifdef __APPLE__
#include "../../engine/engine.h"
#include "../../hostnotify/hostnotify_mac_shim.h"
#include "../../node/video.h"
#endif // __APPLE__

namespace {
const int AudioBufferSize = 1024 * 128;
}

AbyssEngine::SDL2::SDL2SystemIO::SDL2SystemIO()
    : AbyssEngine::SystemIO::SystemIO(), _runMainLoop(false), _audioBuffer(AudioBufferSize), _audioSpec(), _videoMutex(), _videoNode() {
    SPDLOG_TRACE("Creating SDL2 System IO");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        throw std::runtime_error(SDL_GetError());

    _sdlWindow = SDL_CreateWindow(ABYSS_VERSION_STRING, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                  SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    if (_sdlWindow == nullptr)
        throw std::runtime_error(SDL_GetError());

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(_sdlWindow, &wmInfo);
    AbyssEngine::HostNotify::Win32Handle = wmInfo.info.win.window;
#endif // _WIN32
#ifdef __APPLE__
    AbyssHostNotifyInitMac();

#endif // __APPLE__

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME, "Abyss Engine");

    _sdlRenderer = SDL_CreateRenderer(_sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (_sdlRenderer == nullptr)
        throw std::runtime_error(SDL_GetError());

    SDL_RendererInfo renderInfo;
    SDL_GetRendererInfo(_sdlRenderer, &renderInfo);

    SPDLOG_INFO("Using {0} graphics rendering API", renderInfo.name);
    SPDLOG_INFO("Max texture size: {0}x{1}", renderInfo.max_texture_width, renderInfo.max_texture_height);

    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);

    SPDLOG_INFO("SDL Version: {0}.{1}.{2}", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

    SDL_RenderSetLogicalSize(_sdlRenderer, 800, 600);
    SDL_ShowCursor(SDL_FALSE);

    InitializeAudio();

    PauseAudio(false);
}

AbyssEngine::SDL2::SDL2SystemIO::~SDL2SystemIO() {
    SPDLOG_TRACE("Destroying SDL2 System IO");

    FinalizeAudio();

#ifdef __APPLE__
    AbyssHostNotifyFinalizeMac();
#endif // __APPLE__

    if (_sdlRenderer != nullptr)
        SDL_DestroyRenderer(_sdlRenderer);

    if (_sdlWindow != nullptr)
        SDL_DestroyWindow(_sdlWindow);

    SDL_Quit();
}

std::string_view AbyssEngine::SDL2::SDL2SystemIO::Name() { return "SDL2"; }

void AbyssEngine::SDL2::SDL2SystemIO::PauseAudio(bool pause) {
    if (!_hasAudio)
        return;

    SDL_PauseAudioDevice(_audioDeviceId, pause ? SDL_TRUE : SDL_FALSE);
}

void AbyssEngine::SDL2::SDL2SystemIO::SetFullscreen(bool fullscreen) { SDL_SetWindowFullscreen(_sdlWindow, fullscreen ? SDL_TRUE : SDL_FALSE); }

void AbyssEngine::SDL2::SDL2SystemIO::RunMainLoop(Node &rootNode) {
    SPDLOG_TRACE("Starting main loop");

    SDL_Event sdlEvent;

    _runMainLoop = true;
    while (_runMainLoop) {
        while (SDL_PollEvent(&sdlEvent)) {
            HandleSdlEvent(sdlEvent, rootNode);
        }

        if (!_runMainLoop)
            break;

        {
            std::lock_guard<std::mutex> guard(_mutex);

            SDL_RenderClear(_sdlRenderer);

            rootNode.RenderCallback(0, 0);

            if (_showSystemCursor && _cursorSprite != nullptr) {
                _cursorSprite->X = _cursorX;
                _cursorSprite->Y = _cursorY;
                _cursorSprite->RenderCallback(_cursorOffsetX, _cursorOffsetY);
            }

            SDL_RenderPresent(_sdlRenderer);
        }
    }

    SPDLOG_TRACE("Leaving main loop");
}

void AbyssEngine::SDL2::SDL2SystemIO::HandleSdlEvent(const SDL_Event &sdlEvent, Node &rootNode) {
    switch (sdlEvent.type) {
    case SDL_MOUSEMOTION:
        _cursorX = sdlEvent.motion.x;
        _cursorY = sdlEvent.motion.y;

        if (_cursorSprite == nullptr)
            break;

        _cursorSprite->X = _cursorX;
        _cursorSprite->Y = _cursorY;

        break;
    case SDL_QUIT:
        if (_videoNode.get() != nullptr) {
            NotifyVideoFinished();
        }
        _runMainLoop = false;
        break;
    }
}

void AbyssEngine::SDL2::SDL2SystemIO::Stop() {
    std::lock_guard<std::mutex> guard(_mutex);
    _runMainLoop = false;
}

std::unique_ptr<AbyssEngine::ITexture> AbyssEngine::SDL2::SDL2SystemIO::CreateTexture(ITexture::Format textureFormat, uint32_t width,
                                                                                      uint32_t height) {
    return std::make_unique<SDL2Texture>(_sdlRenderer, textureFormat, width, height);
}
void AbyssEngine::SDL2::SDL2SystemIO::InitializeAudio() {
    SDL_AudioSpec requestedAudioSpec{
        .freq = 44100,
        .format = AUDIO_S16LSB,
        .channels = 2,
        .samples = 1024,
        .callback = SDL2SystemIO::HandleAudioCallback,
        .userdata = this,
    };

    _audioDeviceId = SDL_OpenAudioDevice(nullptr, SDL_FALSE, &requestedAudioSpec, &_audioSpec, 0);
    _hasAudio = _audioDeviceId >= 0;

    if (!_hasAudio) {
        SPDLOG_WARN(SDL_GetError());
        return;
    }

    SPDLOG_INFO("Using audio device {0} via {1}", SDL_GetAudioDeviceName(_audioDeviceId, SDL_FALSE), SDL_GetCurrentAudioDriver());
}

void AbyssEngine::SDL2::SDL2SystemIO::HandleAudioCallback(void *userData, Uint8 *stream, int length) {
    auto *source = (SDL2SystemIO *)userData;
    source->HandleAudio(stream, length);
}

void AbyssEngine::SDL2::SDL2SystemIO::HandleAudio(uint8_t *stream, int length) { _audioBuffer.ReadData((char *)stream, length); }
void AbyssEngine::SDL2::SDL2SystemIO::FinalizeAudio() const {
    if (!_hasAudio)
        return;

    SDL_PauseAudioDevice(_audioDeviceId, SDL_TRUE);
    SDL_CloseAudioDevice(_audioDeviceId);
}
void AbyssEngine::SDL2::SDL2SystemIO::PushAudioData(std::span<const char> data) { _audioBuffer.PushData(data); }

void AbyssEngine::SDL2::SDL2SystemIO::PlayVideo(LibAbyss::InputStream stream, bool wait) {
    _videoMutex.lock();
    _waitVideoPlayback = wait;

    _videoNode = std::make_unique<Video>(std::move(stream));
}

void AbyssEngine::SDL2::SDL2SystemIO::WaitForVideoToFinish() {
    if (!_waitVideoPlayback)
        return;

    _videoMutex.lock();
    _videoMutex.unlock();
}

void AbyssEngine::SDL2::SDL2SystemIO::NotifyVideoFinished() {
    _videoNode = nullptr;

    if (!_waitVideoPlayback)
        return;

    _videoMutex.unlock();
}
