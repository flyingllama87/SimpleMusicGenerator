/*
    sdlgui/button.cpp -- [Normal/Toggle/Radio/Popup] Button widget

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/button.h>
#include <sdlgui/theme.h>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <array>

#include "nanovg.h"

NAMESPACE_BEGIN(sdlgui)

Button::Button(Widget *parent, const std::string &caption, int icon)
    : Widget(parent), mCaption(caption), mIcon(icon),
      mIconPosition(IconPosition::LeftCentered), mPushed(false),
      mFlags(NormalButton), mBackgroundColor(Color(0, 0)),
      mTextColor(Color(0, 0)) 
{
  _captionTex.dirty = true;
  _iconTex.dirty = true;
}

Vector2i Button::preferredSize(SDL_Renderer *ctx) const
{
    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    float tw = const_cast<Button*>(this)->mTheme->getTextWidth("sans-bold", fontSize, mCaption.c_str());
    float iw = 0.0f, ih = fontSize;

    if (mIcon) 
    {
        if (nvgIsFontIcon(mIcon)) 
        {
            ih *= 1.5f;
            iw = const_cast<Button*>(this)->mTheme->getUtf8Width("icons", ih, utf8(mIcon).data())  + mSize.y * 0.15f;
        } 
        else 
        {
            int w, h;
            ih *= 0.9f;
            SDL_QueryTexture((SDL_Texture*)mIcon, nullptr, nullptr, &w, &h);
            iw = w * ih / h;
        }
    }
    return Vector2i((int)(tw + iw) + 20, fontSize + 10);
}

bool Button::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
    Widget::mouseButtonEvent(p, button, down, modifiers);
    /* Temporarily increase the reference count of the button in case the
       button causes the parent window to be destructed */
    ref<Button> self = this;

    if (button ==  SDL_BUTTON_LEFT && mEnabled) 
    {
        bool pushedBackup = mPushed;
        if (down) 
        {
            if (mFlags & RadioButton) 
            {
                if (mButtonGroup.empty()) 
                {
                    for (auto widget : parent()->children()) 
                    {
                        Button *b = dynamic_cast<Button *>(widget);
                        if (b != this && b && (b->flags() & RadioButton) && b->mPushed) 
                        {
                            b->mPushed = false;
                            if (b->mChangeCallback)
                                b->mChangeCallback(false);
                        }
                    }
                } 
                else 
                {
                    for (auto b : mButtonGroup) 
                    {
                        if (b != this && (b->flags() & RadioButton) && b->mPushed) 
                        {
                            b->mPushed = false;
                            if (b->mChangeCallback)
                                b->mChangeCallback(false);
                        }
                    }
                }
            }

            if (mFlags & PopupButton) 
            {
                for (auto widget : parent()->children()) 
                {
                    Button *b = dynamic_cast<Button *>(widget);
                    if (b != this && b && (b->flags() & PopupButton) && b->mPushed) 
                    {
                        b->mPushed = false;
                        if (b->mChangeCallback)
                            b->mChangeCallback(false);
                    }
                }
            }

            if (mFlags & ToggleButton)
                mPushed = !mPushed;
            else
                mPushed = true;
        } 
        else if (mPushed) 
        {
            if (contains(p) && mCallback)
                mCallback();
            if (mFlags & NormalButton)
                mPushed = false;
        }
        if (pushedBackup != mPushed && mChangeCallback)
            mChangeCallback(mPushed);

        if (pushedBackup != mPushed)
        {
          _captionTex.dirty = true;
          _iconTex.dirty = true;
        }
        return true;
    }
    return false;
}

void Button::setTextColor(const Color &textColor) 
{ 
  mTextColor = textColor; 
  _captionTex.dirty = true;
  _iconTex.dirty = true;
}

Color Button::bodyColor()
{
  Color result = mTheme->mButtonGradientTopUnfocused;
  if (mBackgroundColor.a() != 0)
    result = mBackgroundColor;

  if (mPushed)
  {
    if (mBackgroundColor.a() != 0)
    {
      result.b() *= 1.5;
      result.g() *= 1.5;
      result.r() *= 1.5;
    }
    else
      result = mTheme->mButtonGradientTopPushed;
  }
  else if (mMouseFocus && mEnabled)
  {
    if (mBackgroundColor.a() != 0)
    {
      result.b() *= 0.5;
      result.g() *= 0.5;
      result.r() *= 0.5;
    }
    else
      result = mTheme->mButtonGradientTopFocused;
  }

  return result;
}

void Button::drawBodyTemp(SDL_Renderer* renderer)
{
  Vector2i ap = absolutePosition();
  SDL_Color bodyclr = bodyColor().toSdlColor();

  SDL_Rect bodyRect{ ap.x + 1, ap.y + 1, width() - 2, height() - 2 };
  SDL_SetRenderDrawColor(renderer, bodyclr.r, bodyclr.g, bodyclr.b, bodyclr.a);
  SDL_RenderFillRect(renderer, &bodyRect);

  SDL_Rect btnRect{ ap.x - 1, ap.y - 1, width() + 2, height() + 1 };
  SDL_Color bl = (mPushed ? mTheme->mBorderDark : mTheme->mBorderLight).toSdlColor();
  SDL_SetRenderDrawColor(renderer, bl.r, bl.g, bl.b, bl.a);
  SDL_Rect blr{ ap.x, ap.y + (mPushed ? 1 : 2), width() - 1, height() - 1 - (mPushed ? 0 : 1) };
  SDL_RenderDrawLine(renderer, blr.x, blr.y, blr.x + blr.w, blr.y);
  SDL_RenderDrawLine(renderer, blr.x, blr.y, blr.x, blr.y + blr.h - 1);

  SDL_Color bd = (mPushed ? mTheme->mBorderLight : mTheme->mBorderDark).toSdlColor();
  SDL_SetRenderDrawColor(renderer, bd.r, bd.g, bd.b, bd.a);
  SDL_Rect bdr{ ap.x, ap.y + 1, width() - 1, height() - 2 };
  SDL_RenderDrawLine(renderer, bdr.x, bdr.y + bdr.h, bdr.x + bdr.w, bdr.y + bdr.h);
  SDL_RenderDrawLine(renderer, bdr.x + bdr.w, bdr.y, bdr.x + bdr.w, bdr.y + bdr.h);

  bd = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, bd.r, bd.g, bd.b, bd.a);
  SDL_RenderDrawRect(renderer, &btnRect);
}


void Button::drawBody(SDL_Renderer* renderer)
{
  drawBodyTemp(renderer);
}

void Button::draw(SDL_Renderer* renderer)
{
  Widget::draw(renderer);

  Vector2i ap = absolutePosition();
  drawBody(renderer);

  int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
  if (_captionTex.dirty)
  {
    Color sdlTextColor = (mTextColor.a() == 0 ? mTheme->mTextColor : mTextColor);
    if (!mEnabled)
      sdlTextColor = mTheme->mDisabledTextColor;

    mTheme->getTexAndRectUtf8(renderer, _captionTex, 0, 0, mCaption.c_str(), "sans-bold", fontSize, sdlTextColor);
  }

  Vector2f center(ap.x + width() * 0.5f, ap.y + height() * 0.5f);
  Vector2i textPos(center.x - _captionTex.w() * 0.5f, center.y - _captionTex.h() * 0.5f - 1);
  
  int offset = mPushed ? 2 : 0;

  if (mIcon) 
  {
    float iw = 0, ih = fontSize;
    auto icon = utf8(mIcon);

    if (_iconTex.dirty)
    {
      Color sdlTextColor = (mTextColor.a() == 0 ? mTheme->mTextColor : mTextColor);

      if (nvgIsFontIcon(mIcon))
      {
        ih *= 1.5f;
        mTheme->getTexAndRectUtf8(renderer, _iconTex, 0, 0, icon.data(), "icons", ih, sdlTextColor);
        iw = _iconTex.w();
      }
      else
      {
        int w, h;
        ih *= 0.9f;
        iw = _iconTex.w() * ih / _iconTex.h();
      }

    }
    if (mCaption != "")
      iw += _pos.y * 0.15f;

    Vector2i iconPos = center.As<int>();
    iconPos.y -= 1;

    if (mIconPosition == IconPosition::LeftCentered) 
    {
      iconPos.x -= _captionTex.w() * 0.5f;
      iconPos.x -= _iconTex.w() * 0.5f;
      textPos.x += _iconTex.w() * 0.5f;// iw * 0.5f;
    }
    else if (mIconPosition == IconPosition::RightCentered) 
    {
      textPos.x -= iw * 0.5f;
      iconPos.x += _captionTex.w() * 0.5f;
    }
    else if (mIconPosition == IconPosition::Left) 
    {
      iconPos.x = getAbsoluteLeft() + 8;
    }
    else if (mIconPosition == IconPosition::Right) 
    {
      iconPos.x = getAbsoluteLeft() + width() - iw - 8;
    }

    if (nvgIsFontIcon(mIcon)) 
      SDL_RenderCopy(renderer, _iconTex, iconPos + getTextOffset() + Vector2i(0, - _iconTex.h() * 0.5f + 1));
    else 
      SDL_RenderCopy(renderer, _iconTex, iconPos + getTextOffset() + Vector2i(0, - ih / 2));
  }

  SDL_RenderCopy(renderer, _captionTex, textPos + getTextOffset());
}

Vector2i Button::getTextOffset() const
{
  int offset = mPushed ? 2 : 0;
  return Vector2i(offset, 1 + offset);
}

NAMESPACE_END(sdlgui)
