#ifndef ABYSS_SYSTEMIO_INTERFACE_H
#define ABYSS_SYSTEMIO_INTERFACE_H

#include <string>
#include <vector>
#include <span>
#include <mutex>
#include <memory>
#include "../common/rectangle.h"

namespace AbyssEngine {

class Sprite;

class ITexture {
  public:
    virtual ~ITexture() = default;
    virtual void SetPixels(std::span<const uint32_t> pixels) = 0;
    virtual void Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) = 0;
};

class SystemIO {
  public:
    virtual ~SystemIO() = default;
    virtual std::string_view Name() = 0;

    /// Pauses the audio subsystem.
    /// \param pause Pauses the audio if true, resumes if false.
    virtual void PauseAudio(bool pause) = 0;

    /// Sets the window state to full screen.
    /// \param fullscreen Window is set to fullscreen if true, or windowed if false.
    virtual void SetFullscreen(bool fullscreen) = 0;

    /// Starts the main system loop.
    virtual void RunMainLoop() = 0;

    /// Stops the main loop
    virtual void Stop() = 0;

    /// Creates a new texture.
    /// \param width The width of the texture.
    /// \param height The height of the texture.
    /// \return A new texture instance.
    virtual std::unique_ptr<ITexture> CreateTexture(uint32_t width, uint32_t height) = 0;

    void SetCursorSprite (std::shared_ptr<Sprite> cursorSprite, int offsetX, int offsetY);
    void ShowSystemCursor(bool show);

  protected:
    std::shared_ptr<Sprite> _cursorSprite;
    bool _showSystemCursor = false;
    int _cursorX = 0;
    int _cursorY = 0;
    int _cursorOffsetX = 0;
    int _cursorOffsetY = 0;

    std::mutex _mutex;
};

} // namespace AbyssEngine::SystemIO

#endif // ABYSS_SYSTEMIO_INTERFACE_H
