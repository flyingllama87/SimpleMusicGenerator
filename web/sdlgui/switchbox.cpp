/*
    src/checkbox.cpp -- Two-state check box widget

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/switchbox.h>
#include <sdlgui/theme.h>

#include "nanovg.h"

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
  Vector2i ap = absolutePosition();
  int ww = width();
  int hh = height();

  float startX, startY, widthX, heightY;
  if (mAlign == Alignment::Horizontal)
  {
    startX = hh * 0.1f;
    heightY = hh * 0.8;
    startY = ((hh - heightY) / 2) + 1;
    widthX = (hh * 1.5);
  }
  else
  {
    startX = hh * 0.05f + 1;
    heightY = hh * 0.8;
    startY = ((hh - heightY) / 2);
    widthX = (hh * 0.4f);
  }

  SDL_Rect bodyRect{ ap.x + (int)startX, ap.y + (int)startY, (int)widthX, (int)heightY };
  SDL_Color bgclr = mTheme->mWindowFillUnfocused.toSdlColor();
  
  SDL_SetRenderDrawColor(renderer, bgclr.r, bgclr.g, bgclr.b, bgclr.a);
  SDL_RenderFillRect(renderer, &bodyRect);

  SDL_Color brdclr = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, brdclr.r, brdclr.g, brdclr.b, brdclr.a);
  SDL_RenderDrawRect(renderer, &bodyRect);
}

void SwitchBox::drawKnob(SDL_Renderer *renderer)
{
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

  SDL_Rect knobRect{ knobPos.x - (int)kr, knobPos.y - (int)kr, (int)(kr * 2), (int)(kr * 2) };
  SDL_Color kclr = mTheme->mBorderLight.toSdlColor();
  SDL_SetRenderDrawColor(renderer, kclr.r, kclr.g, kclr.b, kclr.a);
  SDL_RenderFillRect(renderer, &knobRect);
  
  SDL_Color brdclr = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, brdclr.r, brdclr.g, brdclr.b, brdclr.a);
  SDL_RenderDrawRect(renderer, &knobRect);
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