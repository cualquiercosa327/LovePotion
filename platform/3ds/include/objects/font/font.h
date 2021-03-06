#pragma once

#include "objects/font/fontc.h"
#include "common/data.h"

#include <c2d/font.h>
#include <c2d/text.h>

enum class love::common::Font::SystemFontType : uint8_t
{
    TYPE_STANDARD  = CFG_REGION_USA,
    TYPE_CHINESE   = CFG_REGION_CHN,
    TYPE_TAIWANESE = CFG_REGION_TWN,
    TYPE_KOREAN    = CFG_REGION_KOR,
    TYPE_MAX_ENUM
};

struct Rasterizer
{
    int size;
    love::Data * data;

    C2D_Font font;
    C2D_TextBuf buffer;
};

namespace love
{
    class Font : public love::common::Font
    {
        public:
            static constexpr int FONT_BUFFER_SIZE = 0x200;

            Font(const Rasterizer & r, const Texture::Filter & filter);

            ~Font();

            void Print(Graphics * gfx, const std::vector<ColoredString> & text,
                       const Matrix4 & localTransform, const Colorf & color) override;

            void Printf(Graphics * gfx, const std::vector<ColoredString> & text, float wrap, AlignMode align,
                        const Matrix4 & localTransform, const Colorf & color) override;

            int GetWidth(uint32_t prevGlyph, uint32_t codepoint) override;

            using love::common::Font::GetWidth;

            float GetHeight() const override;

            const C2D_Font GetFont();

            void ClearBuffer();

            float GetScale() const;

        private:
            Rasterizer rasterizer;

            std::unordered_map<uint32_t, float> glyphWidths;
    };
}