#ifndef ABYSS_SPRITE_H
#define ABYSS_SPRITE_H

#include "../common/rectangle.h"
#include "node.h"
#include "../systemio/interface.h"

namespace AbyssEngine {

class Sprite : public Node {
  public:
    enum class PlayMode { Unknown, Paused, Forwards, Backwards };
    struct FramePosition {
        Rectangle Rect;
        int OffsetX;
        int OffsetY;
    };

    void UpdateCallback(uint32_t ticks) final;
    void RenderCallback(int offsetX, int offsetY) final;
    void MouseEventCallback(const MouseEvent& event) final;

    virtual void GetFrameOffset(uint32_t frame, int &offsetX, int &offsetY) = 0;
    virtual void GetFrameSize(uint32_t frame, uint32_t &width, uint32_t &height) = 0;
    virtual uint32_t GetNumberOfAnimations() = 0;
    virtual uint32_t GetFramesPerAnimation() = 0;

  protected:
    virtual void RegenerateAtlas() = 0;

    void Animate(float time_elapsed);
    void AdvanceFrame();

    std::unique_ptr<ITexture> _atlas;
    std::vector<FramePosition> _framePositions;
    bool _bottomOrigin = false;
    uint32_t _cellSizeX = 1;
    uint32_t _cellSizeY = 1;
    PlayMode _playMode = PlayMode::Paused;
    bool _loopAnimation = false;
    float _lastFrameTime = 0;
    float _playLength = 0;
    uint32_t _currentFrame = 0;
    uint32_t _currentAnimation = 0;
};

} // namespace AbyssEngine

#endif // ABYSS_SPRITE_H
