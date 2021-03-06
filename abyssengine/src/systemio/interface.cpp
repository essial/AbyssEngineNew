#include "interface.h"
#include "../node/sprite.h"

void AbyssEngine::SystemIO::SetCursorSprite (Sprite* cursorSprite, int offsetX, int offsetY) {
    std::lock_guard<std::mutex> guard(_mutex);
    _cursorSprite = cursorSprite;
    _cursorOffsetX = offsetX;
    _cursorOffsetY = offsetY;
    _cursorSprite->X = _cursorX;
    _cursorSprite->Y = _cursorY;
}
void AbyssEngine::SystemIO::ShowSystemCursor(bool show) {
    std::lock_guard<std::mutex> guard(_mutex);
    _showSystemCursor = show;
}
