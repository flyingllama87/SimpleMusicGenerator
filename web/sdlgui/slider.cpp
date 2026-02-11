/*
    sdlgui/slider.cpp -- Fractional slider widget with mouse control

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/slider.h>
#include <sdlgui/theme.h>
#include <sdlgui/entypo.h>
#include <array>

#include "nanovg.h"

NAMESPACE_BEGIN(sdlgui)

Slider::Slider(Widget *parent, float value)
    : Widget(parent), mValue(value), mRange(0.f, 1.f), mHighlightedRange(0.f, 0.f)
{
    mHighlightColor = Color(255, 80, 80, 70);
}

Vector2i Slider::preferredSize(SDL_Renderer *) const
{
    return Vector2i(70, 20);
}

bool Slider::mouseDragEvent(const Vector2i &p, const Vector2i & /* rel */,
                            int /* button */, int /* modifiers */) 
{
    if (!mEnabled)
        return false;
    mValue = std::min(std::max((p.x - _pos.x) / (float) mSize.x, (float) 0.0f), (float) 1.0f);
    if (mCallback) mCallback(mValue);
    if (mObjCallback) mObjCallback(this, mValue);
    return true;
}

bool Slider::mouseButtonEvent(const Vector2i &p, int /* button */, bool down, int /* modifiers */)
{
    if (!mEnabled)
        return false;
    mValue = std::min(std::max((p.x - _pos.x) / (float) mSize.x, (float) 0.0f), (float) 1.0f);
    if (mCallback) mCallback(mValue);
    if (mObjCallback) mObjCallback(this, mValue);
    if (mFinalCallback && !down) mFinalCallback(mValue);
    return true;
}

void Slider::drawBody(SDL_Renderer* renderer)
{
  Vector2i ap = absolutePosition();
  int ww = width();
  int hh = height();
  int rh = hh / 3;
  Vector2f center = mSize.cast<float>() * 0.5f;

  SDL_Rect bodyRect{ ap.x, (int)(ap.y + center.y - rh / 2 + 1), ww, rh };
  SDL_Color bgclr = mTheme->mWindowFillUnfocused.toSdlColor();
  
  SDL_SetRenderDrawColor(renderer, bgclr.r, bgclr.g, bgclr.b, bgclr.a);
  SDL_RenderFillRect(renderer, &bodyRect);

  SDL_Color brdclr = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, brdclr.r, brdclr.g, brdclr.b, brdclr.a);
  SDL_RenderDrawRect(renderer, &bodyRect);
}

void Slider::drawKnob(SDL_Renderer* renderer)
{
  Vector2i ap = absolutePosition();
  int hh = height();
  float kr = (int)(hh * 0.4f);
  Vector2i knobPos(ap.x + mValue * mSize.x, ap.y + height() * 0.5f);

  SDL_Rect knobRect{ (int)(knobPos.x - kr), (int)(knobPos.y - kr), (int)(kr * 2), (int)(kr * 2) };
  SDL_Color kclr = mTheme->mBorderLight.toSdlColor();
  SDL_SetRenderDrawColor(renderer, kclr.r, kclr.g, kclr.b, kclr.a);
  SDL_RenderFillRect(renderer, &knobRect);
  
  SDL_Color brdclr = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, brdclr.r, brdclr.g, brdclr.b, brdclr.a);
  SDL_RenderDrawRect(renderer, &knobRect);
}

void Slider::draw(SDL_Renderer* renderer) 
{
  drawBody(renderer);
  drawKnob(renderer);

  _lastEnabledState = mEnabled;
}

NAMESPACE_END(sdlgui)