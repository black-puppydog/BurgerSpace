/*  $Id: ChefController.h,v 1.3 2007/12/22 23:03:35 sarrazip Exp $
    ChefController.h - Control of the game's main character

    burgerspace - A hamburger-smashing video game.
    Copyright (C) 2001-2007 Pierre Sarrazin <http://sarrazip.com/>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef _H_ChefController
#define _H_ChefController

#include <flatzebra/KeyState.h>

#include "util.h"


class ChefController
{
public:

    ChefController(SDLKey pepperKey)
      : leftKS(SDLK_LEFT),
	rightKS(SDLK_RIGHT),
	upKS(SDLK_UP),
	downKS(SDLK_DOWN),
	shootKS(pepperKey, SDLK_RCTRL)
    {
    }

    void check(SDLKey keysym, bool pressed)
    {
	leftKS.check(keysym, pressed);
	rightKS.check(keysym, pressed);
	upKS.check(keysym, pressed);
	downKS.check(keysym, pressed);
	shootKS.check(keysym, pressed);
    }

    bool isShotRequested() const
    {
	return shootKS.justPressed();
    }

    void remember()
    {
	shootKS.remember();
    }

    void getDesiredDirections(bool desiredDirs[4]) const
    {
	desiredDirs[RIGHT] = rightKS.isPressed();
	desiredDirs[UP]    = upKS.isPressed();
	desiredDirs[LEFT]  = leftKS.isPressed();
	desiredDirs[DOWN]  = downKS.isPressed();
    }

private:
    flatzebra::KeyState leftKS;
    flatzebra::KeyState rightKS;
    flatzebra::KeyState upKS;
    flatzebra::KeyState downKS;
    flatzebra::KeyState shootKS;
};

#endif  /* _H_ChefController */
