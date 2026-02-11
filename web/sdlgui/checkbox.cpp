/*
    sdlgui/checkbox.cpp -- Two-state check box widget

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/checkbox.h>
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <sdlgui/theme.h>
#include <sdlgui/entypo.h>
#include <array>
#include <algorithm>

#include "nanovg.h"
#define NANOVG_RT_IMPLEMENTATION
#define NANORT_IMPLEMENTATION
#include "nanovg_rt.h"

NAMESPACE_BEGIN(sdlgui)

CheckBox::CheckBox(Widget *parent, const std::string &caption,
                   const std::function<void(bool) > &callback)
    : Widget(parent), mCaption(caption), mPushed(false), mChecked(false),
      mCallback(callback) 
{
  _captionTex.dirty = true;
  _pointTex.dirty = true;
}

bool CheckBox::mouseButtonEvent(const Vector2i &p, int button, bool down,
                                int modifiers) 
{
    Widget::mouseButtonEvent(p, button, down, modifiers);
    if (!mEnabled)
        return false;

    if (button == SDL_BUTTON_LEFT) 
    {
        if (down) 
        {
            mPushed = true;
        } 
        else if (mPushed) 
        {
            if (contains(p)) 
            {
                mChecked = !mChecked;
                if (mCallback)
                    mCallback(mChecked);
            }
            mPushed = false;
        }
        return true;
    }
    return false;
}

Vector2i CheckBox::preferredSize(SDL_Renderer *ctx) const
{
    if (mFixedSize != Vector2i::Zero())
        return mFixedSize;

    int w, h;
    const_cast<CheckBox*>(this)->mTheme->getTextBounds("sans", fontSize(), mCaption.c_str(), &w, &h);
    return Vector2i(w + 1.7f * fontSize(),  fontSize() * 1.3f);
}

void CheckBox::drawBody(SDL_Renderer* renderer)
{
  int id = (mPushed ? 0x1 : 0) + (mMouseFocus ? 0x2 : 0) + (mEnabled ? 0x4 : 0);

  auto it = std::find_if(_txs.begin(), _txs.end(), [id](const std::pair<int, Texture>& p) { return p.first == id; });

  if (it != _txs.end())
  {
    SDL_RenderCopy(renderer, it->second, absolutePosition());
  }
  else
  {
    NVGcontext *ctx = nullptr;
    int ww = width();
    int hh = height();
    ctx = nvgCreateRT(NVG_DEBUG, ww + 2, hh + 2, 0);

    float pxRatio = 1.0f;
    nvgBeginFrame(ctx, ww + 2, hh + 2, pxRatio);

    Color b = Color(0, 0, 0, 180);
    Color c = mPushed ? Color(0, 100) : Color(0, 32);
    NVGpaint bg = nvgBoxGradient(ctx, 1.5f, 1.5f, hh - 2.0f, hh - 2.0f, 3, 3, c.toNvgColor(), b.toNvgColor());

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, 1.0f, 1.0f, hh - 2.0f, hh - 2.0f, 3);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    nvgEndFrame(ctx);

    Texture tex;
    tex.rrect = { 0, 0, ww + 2, hh + 2 };
    
    unsigned char *rgba = nvgReadPixelsRT(ctx);
    tex.tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, tex.w(), tex.h());

    int pitch;
    uint8_t *pixels;
    SDL_LockTexture(tex.tex, nullptr, (void **)&pixels, &pitch);
    memcpy(pixels, rgba, sizeof(uint32_t) * tex.w() * tex.h());
    SDL_SetTextureBlendMode(tex.tex, SDL_BLENDMODE_BLEND);
    SDL_UnlockTexture(tex.tex);

    nvgDeleteRT(ctx);
    
    _txs.push_back({id, tex});
    SDL_RenderCopy(renderer, _txs.back().second, absolutePosition());
  }
}


void CheckBox::draw(SDL_Renderer *renderer)
{
  Widget::draw(renderer);

  if (_captionTex.dirty)
  {
    Color tColor = (mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
    mTheme->getTexAndRectUtf8(renderer, _captionTex, 0, 0, mCaption.c_str(), "sans", fontSize(), tColor);
    mTheme->getTexAndRectUtf8(renderer, _pointTex, 0, 0, utf8(ENTYPO_ICON_CHECK).data(), "icons", 1.8 * mSize.y, tColor);
  }
 
  auto ap = absolutePosition();
  SDL_RenderCopy(renderer, _captionTex, ap + Vector2i(1.2f * mSize.y + 5, (mSize.y - _captionTex.h()) * 0.5f) );

  drawBody(renderer);
  
  if (mChecked) 
    SDL_RenderCopy(renderer, _pointTex, ap + Vector2i((mSize.y - _pointTex.w()) * 0.5f + 1,  (mSize.y - _pointTex.h()) * 0.5f));
}

NAMESPACE_END(sdlgui)
