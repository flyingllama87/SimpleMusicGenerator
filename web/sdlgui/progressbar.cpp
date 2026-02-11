/*
    sdlgui/progressbar.cpp -- Standard widget for visualizing progress

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/progressbar.h>
#include <sdlgui/theme.h>

#include "nanovg.h"

NAMESPACE_BEGIN(sdlgui)

ProgressBar::ProgressBar(Widget *parent)
    : Widget(parent), mValue(0.0f) 
{
}

void ProgressBar::setValue(float value) 
{ 
  mValue = value; 
}

Vector2i ProgressBar::preferredSize(SDL_Renderer *) const
{
    return Vector2i(70, 12);
}

void ProgressBar::drawBody(SDL_Renderer* renderer)
{
  Vector2i ap = absolutePosition();
  SDL_Rect bodyRect{ ap.x, ap.y, width(), height() };
  SDL_Color bgclr = mTheme->mWindowFillUnfocused.toSdlColor();
  
  SDL_SetRenderDrawColor(renderer, bgclr.r, bgclr.g, bgclr.b, bgclr.a);
  SDL_RenderFillRect(renderer, &bodyRect);

  SDL_Color brdclr = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, brdclr.r, brdclr.g, brdclr.b, brdclr.a);
  SDL_RenderDrawRect(renderer, &bodyRect);
}

void ProgressBar::drawBar(SDL_Renderer* renderer)
{
  Vector2i ap = absolutePosition();
  float value = std::min(std::max(0.0f, mValue), 1.0f);
  int barPos = (int)std::round((width() - 2) * value);

  SDL_Rect barRect{ ap.x + 1, ap.y + 1, barPos, height() - 2 };
  SDL_Color barclr = Color(220, 100, 128, 100).toSdlColor();
  
  SDL_SetRenderDrawColor(renderer, barclr.r, barclr.g, barclr.b, barclr.a);
  SDL_RenderFillRect(renderer, &barRect);
}

void ProgressBar::draw(SDL_Renderer* renderer)
{
  Widget::draw(renderer);

  drawBody(renderer);

  float value = std::min(std::max(0.0f, mValue), 1.0f);
  if (value > 0)
    drawBar(renderer);
}

NAMESPACE_END(sdlgui)