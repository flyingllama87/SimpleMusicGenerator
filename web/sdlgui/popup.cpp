/*
    sdlgui/popup.cpp -- Simple popup widget which is attached to another given
    window (can be nested)

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/popup.h>
#include <sdlgui/theme.h>

#include "nanovg.h"

NAMESPACE_BEGIN(sdlgui)

Popup::Popup(Widget *parent, Window *parentWindow)
    : Window(parent, ""), mParentWindow(parentWindow),
      mAnchorPos(Vector2i::Zero()), mAnchorHeight(30)
{
}

void Popup::performLayout(SDL_Renderer *ctx) 
{
    if (mLayout || mChildren.size() != 1) 
    {
        Widget::performLayout(ctx);
    } 
    else 
    {
        mChildren[0]->setPosition(Vector2i::Zero());
        mChildren[0]->setSize(mSize);
        mChildren[0]->performLayout(ctx);
    }
}

void Popup::refreshRelativePlacement() 
{
    mParentWindow->refreshRelativePlacement();
    mVisible &= mParentWindow->visibleRecursive();
    _pos = mParentWindow->position() + mAnchorPos - Vector2i(0, mAnchorHeight);
}

void Popup::drawBodyTemp(SDL_Renderer* renderer)
{
  int ds = mTheme->mWindowDropShadowSize;
  int cr = mTheme->mWindowCornerRadius;

  /* Draw a drop shadow */
  SDL_Color sh = mTheme->mDropShadow.toSdlColor();
  SDL_Rect shRect{ _pos.x - ds, _pos.y - ds, mSize.x + 2 * ds, mSize.y + 2 * ds };
  SDL_SetRenderDrawColor(renderer, sh.r, sh.g, sh.b, 64);
  SDL_RenderFillRect(renderer, &shRect);

  SDL_Color bg = mTheme->mWindowPopup.toSdlColor();
  SDL_Rect bgRect{ _pos.x, _pos.y, mSize.x, mSize.y };

  SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderFillRect(renderer, &bgRect);

  SDL_Color br = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, br.r, br.g, br.b, br.a);

  SDL_Rect brr{ _pos.x - 1, _pos.y - 1, width() + 2, height() + 2 };
  SDL_RenderDrawLine(renderer, brr.x, brr.y, brr.x + brr.w, brr.y);
  SDL_RenderDrawLine(renderer, brr.x + brr.w, brr.y, brr.x + brr.w, brr.y + brr.h);
  SDL_RenderDrawLine(renderer, brr.x, brr.y + brr.h, brr.x + brr.w, brr.y + brr.h);
  SDL_RenderDrawLine(renderer, brr.x, brr.y, brr.x, brr.y + brr.h);

  // Draw window anchor
  SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
  for (int i = 0; i < 15; i++)
  {
    SDL_RenderDrawLine(renderer, _pos.x - 15 + i, _pos.y + mAnchorHeight - i,
      _pos.x - 15 + i, _pos.y + mAnchorHeight + i);
  }
}

void Popup::drawBody(SDL_Renderer* renderer)
{
  drawBodyTemp(renderer);
}

Vector2i Popup::getOverrideBodyPos()
{
  Vector2i ap = absolutePosition();
  int ds = mTheme->mWindowDropShadowSize;
  return ap - Vector2i(_anchorDx + ds, ds);
}

void Popup::draw(SDL_Renderer* renderer)
{
  refreshRelativePlacement();

  if (!mVisible)
    return;

  drawBody(renderer);
  
  Widget::draw(renderer);
}

NAMESPACE_END(sdlgui)
