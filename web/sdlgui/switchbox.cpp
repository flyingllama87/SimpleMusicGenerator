/*
    sdlgui/switchbox.cpp -- Two-state check box widget

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/switchbox.h>
#include <sdlgui/theme.h>
#include <algorithm>

#include "nanovg.h"
#define NANOVG_RT_IMPLEMENTATION
#define NANORT_IMPLEMENTATION
#include "nanovg_rt.h"

NAMESPACE_BEGIN(sdlgui)

SwitchBox::SwitchBox(Widget *parent, Alignment align, const std::string &caption,
                   const std::function<void(bool) > &callback)
    : CheckBox(parent, caption, callback), mAlign(align) 
{
}

Vector2i SwitchBox::preferredSize(SDL_Renderer *renderer) const 
{
    if (mFixedSize != Vector2i::Zero())
        return mFixedSize;

    int w, h;
    const_cast<SwitchBox*>(this)->theme()->getUtf8Bounds("sans", fontSize(), mCaption.c_str(), &w, &h);
    int knobW = 1.8f * fontSize();
    knobW = std::max<int>(knobW / 32, 1) * 32;

    if (mAlign == Alignment::Horizontal)
      return Vector2i(w + knobW, knobW);
    else
      return Vector2i(w + knobW, 2 * knobW);
}

void SwitchBox::drawBody(SDL_Renderer *renderer)
{
  int id = (0x100) + (mEnabled ? 1 : 0);
  auto it = std::find_if(_txs.begin(), _txs.end(), [id](const std::pair<int, Texture>& p) { return p.first == id; });

  if (it != _txs.end())
  {
    SDL_RenderCopy(renderer, it->second, absolutePosition());
  }
  else
  {
    int ww = width();
    int hh = height();
    NVGcontext *ctx = nvgCreateRT(NVG_DEBUG, ww, hh, 0);

    float pxRatio = 1.0f;
    nvgBeginFrame(ctx, ww, hh, pxRatio);

    float kr, startX, startY, widthX, heightY;
    if (mAlign == Alignment::Horizontal)
    {
      kr = hh * 0.4f;
      startX = hh * 0.1f;
      heightY = hh * 0.8;
      startY = ((hh - heightY) / 2) + 1;
      widthX = (hh * 1.5);
    }
    else
    {
      kr = hh * 0.2f;
      startX = hh * 0.05f + 1;
      heightY = hh * 0.8;
      startY = ((hh - heightY) / 2);
      widthX = (hh * 0.4f);
    }

    NVGpaint bg = nvgBoxGradient(ctx, startX, startY, widthX, heightY, 3, 3,
      Color(0, mEnabled ? 32 : 10).toNvgColor(),
      Color(0, mEnabled ? 128 : 210).toNvgColor());

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, startX, startY, widthX, heightY, kr);
    nvgFillPaint(ctx, bg);

    nvgBeginPath(ctx);
    nvgStrokeWidth(ctx, 1.0f);
    nvgRoundedRect(ctx, startX + 0.5f, startY + 0.5f, widthX - 1, heightY - 1, kr);
    nvgStrokeColor(ctx, mTheme->mBorderLight.toNvgColor());
    nvgStroke(ctx);
    nvgFill(ctx);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, startX + 0.5f, startY + 0.5f, widthX - 1, heightY - 2, kr);
    nvgStrokeColor(ctx, mTheme->mBorderDark.toNvgColor());
    nvgStroke(ctx);

    nvgEndFrame(ctx);

    Texture tex;
    tex.rrect = { 0, 0, ww, hh };
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

void SwitchBox::drawKnob(SDL_Renderer *renderer)
{
  int id = (0x200) + (mEnabled ? 1 : 0);
  auto it = std::find_if(_txs.begin(), _txs.end(), [id](const std::pair<int, Texture>& p) { return p.first == id; });
 
  Vector2i ap = absolutePosition();
  Vector2f center = ap.As<float>() + mSize.As<float>() * 0.5f;
  Vector2i knobPos;
  float kr, startX, startY, widthX, heightY, hh;
  hh = height();
  if (mAlign == Alignment::Horizontal)
  {
    kr = (hh * 0.4f);
    startX = ap.x + hh * 0.1f;
    heightY = hh * 0.8;
    startY = (ap.y + (hh - heightY) / 2) + 1;
    widthX = (hh * 1.5);
    knobPos = Vector2i(startX + kr + path * (widthX - 2 * kr), center.y + 0.5f);
  }
  else
  {
    kr = (hh * 0.2f);
    startX = ap.x + hh * 0.05f + 1;
    heightY = hh * 0.8;
    startY = (ap.y + (hh - heightY) / 2);
    widthX = (hh * 0.4f);
    knobPos = Vector2i(startX + kr, startY + path * (heightY - 2 * kr) + kr);
  }

  if (it != _txs.end())
  {
    SDL_RenderCopy(renderer, it->second, knobPos - Vector2i(it->second.w()/2, it->second.h() / 2));
  }
  else
  {
    int ww = std::min(width(), height());
    int rhh = ww;
    NVGcontext *ctx = nvgCreateRT(NVG_DEBUG, ww, rhh, 0);
    float pxRatio = 1.0f;
    nvgBeginFrame(ctx, ww, rhh, pxRatio);
    Vector2f rcenter(ww/2, rhh/2);
    float rkr = rhh * 0.4f; 
    NVGpaint knob = nvgLinearGradient(ctx, 0, rcenter.y - rkr, 0, rcenter.y + rkr,
      mTheme->mBorderLight.toNvgColor(), mTheme->mBorderMedium.toNvgColor());
    NVGpaint knobReverse = nvgLinearGradient(ctx, 0, rcenter.y - rkr, 0, rcenter.y + rkr,
      mTheme->mBorderMedium.toNvgColor(), mTheme->mBorderLight.toNvgColor());
    nvgBeginPath(ctx);
    nvgCircle(ctx, rcenter.x, rcenter.y, rkr * 0.9);
    nvgStrokeColor(ctx, Color(0, 200).toNvgColor());
    nvgFillPaint(ctx, knob);
    nvgStroke(ctx);
    nvgFill(ctx);
    nvgBeginPath(ctx);
    nvgCircle(ctx, rcenter.x, rcenter.y, rkr * 0.7);
    nvgFillColor(ctx, Color(120, mEnabled ? 255 : 100).toNvgColor());
    nvgStrokePaint(ctx, knobReverse);
    nvgStroke(ctx);
    nvgFill(ctx);
    nvgEndFrame(ctx);

    Texture tex;
    tex.rrect = { 0, 0, ww, rhh };
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
    SDL_RenderCopy(renderer, _txs.back().second, knobPos - Vector2i(tex.w()/2, tex.h() / 2));
  }
}

void SwitchBox::draw(SDL_Renderer *renderer)
{
  if (mChecked)
  {
    if (path < 1.0f)
      path += 0.1f;
  }
  else
  {
    if (path > 0) path -= 0.1f;
    if (path < 0) path = 0;
  }

  drawBody(renderer);
  drawKnob(renderer);

  Widget::draw(renderer);
}

NAMESPACE_END(sdlgui)
