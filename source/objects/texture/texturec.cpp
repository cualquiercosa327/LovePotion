#include "objects/texture/texture.h"

using namespace love::common;

love::Type Texture::type("Texture", &Drawable::type);

Texture::Filter Texture::defaultFilter;
Texture::FilterMode Texture::defaultMipmapFilter = Texture::FILTER_LINEAR;
float Texture::defaultMipmapSharpness = 0.0f;

Texture::Texture(TextureType texType) : texType(texType),
                                        width(0),
                                        height(0),
                                        filter(defaultFilter),
                                        wrap()
{}

Texture::~Texture()
{}

void Texture::InitQuad()
{
    Quad::Viewport v = {0, 0, (double)this->width, (double)this->height};
    this->quad.Set(new love::Quad(v, this->width, this->height), Acquire::NORETAIN);
}

Texture::TextureType Texture::GetTextureType() const
{
    return this->texType;
}

int Texture::GetWidth(int mip) const
{
    return std::max(this->width >> mip, 1);
}

int Texture::GetHeight(int mip) const
{
    return std::max(this->height >> mip, 1);
}

const Texture::Filter & Texture::GetFilter() const
{
    return this->filter;
}

const Texture::Wrap & Texture::GetWrap() const
{
    return this->wrap;
}

love::Quad * Texture::GetQuad() const
{
    return this->quad;
}

bool Texture::GetConstant(const char * in, TextureType & out)
{
    return texTypes.Find(in, out);
}

bool Texture::GetConstant(TextureType in, const char *& out)
{
    return texTypes.Find(in, out);
}

std::vector<std::string> Texture::GetConstants(TextureType)
{
    return texTypes.GetNames();
}

bool Texture::GetConstant(const char * in, FilterMode & out)
{
    return filterModes.Find(in, out);
}

bool Texture::GetConstant(FilterMode in, const char *& out)
{
    return filterModes.Find(in, out);
}

std::vector<std::string> Texture::GetConstants(FilterMode)
{
    return filterModes.GetNames();
}

bool Texture::GetConstant(const char * in, WrapMode & out)
{
    return wrapModes.Find(in, out);
}

bool Texture::GetConstant(WrapMode in, const char *& out)
{
    return wrapModes.Find(in, out);
}

std::vector<std::string> Texture::GetConstants(WrapMode)
{
    return wrapModes.GetNames();
}

StringMap<Texture::TextureType, Texture::TEXTURE_MAX_ENUM>::Entry Texture::texTypeEntries[] =
{
    { "2d", TEXTURE_2D }
};

StringMap<Texture::TextureType, Texture::TEXTURE_MAX_ENUM> Texture::texTypes(Texture::texTypeEntries, sizeof(Texture::texTypeEntries));

StringMap<Texture::FilterMode, Texture::FILTER_MAX_ENUM>::Entry Texture::filterModeEntries[] =
{
    { "nearest", FILTER_NEAREST },
    { "linear",  FILTER_LINEAR  },
    { "none",    FILTER_NONE    },
};

StringMap<Texture::FilterMode, Texture::FILTER_MAX_ENUM> Texture::filterModes(Texture::filterModeEntries, sizeof(Texture::filterModeEntries));

StringMap<Texture::WrapMode, Texture::WRAP_MAX_ENUM>::Entry Texture::wrapModeEntries[] =
{
    { "clamp",          WRAP_CLAMP           },
    { "clampzero",      WRAP_CLAMP_ZERO      },
    { "repeat",         WRAP_REPEAT          },
    { "mirroredrepeat", WRAP_MIRRORED_REPEAT },
};

StringMap<Texture::WrapMode, Texture::WRAP_MAX_ENUM> Texture::wrapModes(Texture::wrapModeEntries, sizeof(Texture::wrapModeEntries));
