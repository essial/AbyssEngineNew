#ifndef ABYSS_SPRITEFONT_H
#define ABYSS_SPRITEFONT_H

#include "../common/rectangle.h"
#include "../systemio/interface.h"
#include "libabyss/dc6.h"
#include "libabyss/palette.h"
#include <string>

namespace AbyssEngine {

class SpriteFont {
  public:
    struct Glyph {
        uint16_t FrameIndex;
        uint8_t Width;
        uint8_t Height;
    };

    struct FramePosition {
        Rectangle rect;
        int OffsetX;
        int OffsetY;
    };

    SpriteFont(std::string_view filePath, std::string_view paletteName);

  private:
    void RegenerateAtlas();
    std::unique_ptr<LibAbyss::DC6> _dc6;
    std::unique_ptr<ITexture> _atlas;
    std::vector<Glyph> _glyphs;
    std::vector<FramePosition> _frameRects;
    const LibAbyss::Palette &_palette;
};

} // namespace AbyssEngine

#endif // ABYSS_SPRITEFONT_H
