#include "sprite.h"

AbyssEngine::Sprite::Sprite() : _atlas(), _framePositions() {}

void AbyssEngine::Sprite::UpdateCallback(uint32_t ticks) {
    if (!Active)
        return;

    Animate((float)ticks / 1000.f);

    Node::UpdateCallback(ticks);
}

void AbyssEngine::Sprite::RenderCallback(int offsetX, int offsetY) {
    if (!Visible || !Active)
        return;

    if (_atlas == nullptr)
        RegenerateAtlas();

    const auto startFrameIdx = _currentFrame;
    const auto totalFrames = GetFramesPerAnimation();
    // const auto totalAnimations = GetNumberOfAnimations();

    uint32_t frameWidth;
    uint32_t frameHeight;
    GetFrameSize(_currentFrame, frameWidth, frameHeight);

    auto posX = X + offsetX;
    auto posY = Y + offsetY;

    if (_bottomOrigin)
        posY -= (int)frameHeight;

    const auto startX = posX;

    for (auto cellOffsetY = 0; cellOffsetY < _cellSizeY; cellOffsetY++) {
        auto lastHeight = 0;

        for (auto cellOffsetX = 0; cellOffsetX < _cellSizeX; cellOffsetX++) {
            const auto cellIndex = startFrameIdx + (cellOffsetX + (cellOffsetY * _cellSizeX));

            int frameOffsetX;
            int frameOffsetY;
            GetFrameOffset(cellIndex, frameOffsetX, frameOffsetY);

            const auto framePos = _framePositions[(_currentAnimation * totalFrames) + cellIndex];

            Rectangle destRect = framePos.Rect;
            destRect.X = framePos.OffsetX + posX;
            destRect.Y = framePos.OffsetY + posY;

            _atlas->Render(framePos.Rect, destRect);

            posX += startX;
            lastHeight = destRect.Height;
        }

        posX = startX;
        posY += lastHeight;
    }

    Node::RenderCallback(offsetX, offsetY);
}

void AbyssEngine::Sprite::MouseEventCallback(AbyssEngine::MouseEventInfo eventInfo) {
    Node::MouseEventCallback(eventInfo);
}

void AbyssEngine::Sprite::Animate(float elapsed) {
    if (_playMode == PlayMode::Unknown || _playMode == PlayMode::Paused)
        return;

    const auto frameCount = GetFramesPerAnimation();
    const auto frameLength = _playLength / (float)frameCount;
    const auto framesAdvanced = (int)(_lastFrameTime / frameLength);
    _lastFrameTime -= (float)framesAdvanced * frameLength;

    for (auto i = 0; i < framesAdvanced; i++)
        AdvanceFrame();
}

void AbyssEngine::Sprite::AdvanceFrame() {
    if (_playMode == PlayMode::Unknown || _playMode == PlayMode::Paused)
        return;

    const auto startIndex = 0;
    const auto endIndex = GetFramesPerAnimation();

    switch (_playMode) {
    case PlayMode::Forwards: {
        _currentFrame++;

        if (_currentFrame < endIndex)
            return;

        _currentFrame = _loopAnimation ? startIndex : endIndex - 1;
    }
        return;
    case PlayMode::Backwards: {
        if (_currentFrame != 0) {
            _currentFrame--;
            return;
        }

        _currentFrame = _loopAnimation ? endIndex - 1 : startIndex;
    }
    default:
        throw std::runtime_error("Unimplemented play mode");
    }
}

