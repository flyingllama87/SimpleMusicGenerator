/*
    sdlgui/vscrollpanel.cpp -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/vswrap.h>
#include <sdlgui/theme.h>
#include <iostream>
#include <string>

NAMESPACE_BEGIN(sdlgui)

VSWrapper::VSWrapper(Widget *parent)
    : Widget(parent), mChildPreferredHeight(0), mScroll(0.0f) 
{ }

void VSWrapper::draw(SDL_Renderer *renderer) 
{
  for (auto child : mChildren)
  {
    int childYPos = child->getAbsolutePos().y;
    int parentYPosStart = mParent->getAbsoluteCliprect().y1;
    int parentYPosEnd = mParent->getAbsoluteCliprect().y2;
    if (parentYPosStart > childYPos)
        continue;
    if ((parentYPosEnd - 20) < childYPos)
        continue;
    // std::cout << "child y pos: " << std::to_string(temp.y) << "\n";
    if (child->visible())
      child->draw(renderer);
  }
}


SDL_Point VSWrapper::getAbsolutePos() const
{
  return Widget::getAbsolutePos();
}

PntRect VSWrapper::getAbsoluteCliprect() const
{
  return Widget::getAbsoluteCliprect();
}

int VSWrapper::getAbsoluteTop() const
{
  return Widget::getAbsoluteTop();
}

NAMESPACE_END(sdlgui)
