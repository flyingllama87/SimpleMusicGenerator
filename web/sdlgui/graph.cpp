/*
    sdlgui/graph.cpp -- Simple graph widget for showing a function plot

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/graph.h>
#include <sdlgui/theme.h>

#include "nanovg.h"

NAMESPACE_BEGIN(sdlgui)

Graph::Graph(Widget *parent, const std::string &caption)
    : Widget(parent), mCaption(caption) 
{
    mBackgroundColor = Color(20, 128);
    mForegroundColor = Color(255, 192, 0, 128);
    mTextColor = Color(240, 192);
    _captionTex.dirty = true;
    _headerTex.dirty = true;
}

Vector2i Graph::preferredSize(SDL_Renderer *) const
{
    return Vector2i(180, 45);
}

void Graph::draw(SDL_Renderer *renderer) 
{
    Widget::draw(renderer);

    Vector2i ap = absolutePosition();
    
    SDL_Rect bgRect{ ap.x, ap.y, width(), height() };
    SDL_Color bgclr = mBackgroundColor.toSdlColor();
    SDL_SetRenderDrawColor(renderer, bgclr.r, bgclr.g, bgclr.b, bgclr.a);
    SDL_RenderFillRect(renderer, &bgRect);

    if (mValues.size() >= 2)
    {
      SDL_Color fgclr = mForegroundColor.toSdlColor();
      SDL_SetRenderDrawColor(renderer, fgclr.r, fgclr.g, fgclr.b, fgclr.a);
      
      for (size_t i = 0; i < (size_t)mValues.size() - 1; i++) 
      {
        float value1 = mValues[i];
        float value2 = mValues[i+1];
        float vx1 = ap.x + i * width() / (float)(mValues.size() - 1);
        float vy1 = ap.y + (1 - value1) * height();
        float vx2 = ap.x + (i + 1) * width() / (float)(mValues.size() - 1);
        float vy2 = ap.y + (1 - value2) * height();
        SDL_RenderDrawLine(renderer, (int)vx1, (int)vy1, (int)vx2, (int)vy2);
      }
    }

    if (_captionTex.dirty)
      mTheme->getTexAndRectUtf8(renderer, _captionTex, 0, 0, mCaption.c_str(), "sans", 14, mTextColor);

    if (_headerTex.dirty)
      mTheme->getTexAndRectUtf8(renderer, _headerTex, 0, 0, mHeader.c_str(), "sans", 18, mTextColor);

    if (_footerTex.dirty)
      mTheme->getTexAndRectUtf8(renderer, _footerTex, 0, 0, mFooter.c_str(), "sans", 15, mTextColor);

    SDL_RenderCopy(renderer, _captionTex, ap + Vector2i(3,1) );
    SDL_RenderCopy(renderer, _headerTex, ap + Vector2i(mSize.x - 3 - _headerTex.w(), 1));
    SDL_RenderCopy(renderer, _footerTex, ap + Vector2i(mSize.x - 3 - _footerTex.w(), mSize.y - 1 - _footerTex.h()));
 }

NAMESPACE_END(sdlgui)