#include <citro2d.h>

#include "objects/texture/texture.h"
#include "modules/graphics/graphics.h"

using namespace love;

Texture::Texture(TextureType type) : common::Texture(TEXTURE_2D)
{}

void Texture::Draw(Graphics * gfx, const Matrix4 & localTransform)
{
    this->Draw(gfx, this->quad, localTransform);
}

bool Texture::SetWrap(const Wrap & wrap)
{
    C3D_TexSetWrap(this->texture.tex, (GPU_TEXTURE_WRAP_PARAM)wrap.s, (GPU_TEXTURE_WRAP_PARAM)wrap.t);
    return true;
}

void Texture::SetFilter(const Filter & filter)
{
    C3D_TexSetFilter(this->texture.tex, (GPU_TEXTURE_FILTER_PARAM)filter.mag, (GPU_TEXTURE_FILTER_PARAM)filter.min);
}

void Texture::Draw(Graphics * gfx, love::Quad * quad, const Matrix4 & localTransform)
{
    Tex3DS_SubTexture tv = quad->GetTex3DSViewport();
    this->texture.subtex = &tv;

    Quad::Viewport v = quad->GetViewport();

    // Multiply the current and local transforms
    Matrix4 t(gfx->GetTransform(), localTransform);

    C2D_DrawParams params;

    params.pos = {0.0f, 0.0f, (float)v.w, (float)v.h};
    params.depth = Graphics::CURRENT_DEPTH;
    params.angle = 0.0f;
    params.center = {0.0f, 0.0f};

    // Appy the new Matrix4 C3D_Mtx
    C2D_ViewRestore(&t.GetElements());

    C2D_ImageTint tint;
    Colorf color = gfx->GetColor();

    C2D_AlphaImageTint(&tint, color.a);

    C2D_DrawImage(this->texture, &params, &tint);
}