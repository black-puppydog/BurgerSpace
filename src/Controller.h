/*  $Id: Controller.h,v 1.1.2.4 2010/05/15 01:48:55 sarrazip Exp $
    Controller.h - Control of the game's main character

    burgerspace - A hamburger-smashing video game.
    Copyright (C) 2001-2010 Pierre Sarrazin <http://sarrazip.com/>

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

#ifndef _H_Controller
#define _H_Controller

#include <flatzebra/KeyState.h>
#include <flatzebra/Joystick.h>

#include "util.h"


class Controller
{
public:

    Controller(SDLKey pepperKey)
      : startKS(SDLK_SPACE),
        pauseKS(SDLK_p),
        quitKS(SDLK_ESCAPE),
        leftKS(SDLK_LEFT),
        rightKS(SDLK_RIGHT),
        upKS(SDLK_UP),
        downKS(SDLK_DOWN),
        shootKS(pepperKey, SDLK_RCTRL),
        saveKS(SDLK_s),
        loadKS(SDLK_l),
        fullScreenKS(SDLK_F11),
        joystick(),
        startBI(flatzebra::Joystick::START_BTN),
        pauseBI(flatzebra::Joystick::BACK_BTN),
        shootBI(flatzebra::Joystick::B_BTN)
    {
    }

    void check(SDLKey keysym, bool pressed)
    {
        startKS.check(keysym, pressed);
        pauseKS.check(keysym, pressed);
        quitKS.check(keysym, pressed);
        saveKS.check(keysym, pressed);
        loadKS.check(keysym, pressed);
        leftKS.check(keysym, pressed);
        rightKS.check(keysym, pressed);
        upKS.check(keysym, pressed);
        downKS.check(keysym, pressed);
        shootKS.check(keysym, pressed);
        fullScreenKS.check(keysym, pressed);
    }

    bool isShotRequested() const
    {
        return shootKS.justPressed() || joystick.buttonJustPressed(shootBI);
    }

    bool isStartRequested() const
    {
        return startKS.justPressed() || joystick.buttonJustPressed(startBI);
    }

    bool isPauseRequested() const
    {
        return pauseKS.justPressed() || joystick.buttonJustPressed(pauseBI);
    }

    bool isResumeRequested() const
    {
        return pauseKS.justPressed() || joystick.buttonJustPressed(startBI);
    }

    bool isQuitRequested() const
    {
        return quitKS.justPressed();
    }

    bool isSaveRequested() const
    {
        return saveKS.justPressed();
    }

    bool isLoadRequested() const
    {
        return loadKS.justPressed();
    }

    bool isFullScreenToggleRequested() const
    {
        return fullScreenKS.justPressed();
    }

    void update()
    {
        startKS.remember();
        pauseKS.remember();
        quitKS.remember();
        leftKS.remember();
        rightKS.remember();
        upKS.remember();
        downKS.remember();
        shootKS.remember();
        saveKS.remember();
        loadKS.remember();
        fullScreenKS.remember();
        joystick.update();
    }

    void getDesiredDirections(bool desiredDirs[4]) const
    {
        desiredDirs[RIGHT] = rightKS.isPressed() || joystick.getXAxisDisplacement() > 0;
        desiredDirs[UP]    = upKS.isPressed()    || joystick.getYAxisDisplacement() < 0;
        desiredDirs[LEFT]  = leftKS.isPressed()  || joystick.getXAxisDisplacement() < 0;
        desiredDirs[DOWN]  = downKS.isPressed()  || joystick.getYAxisDisplacement() > 0;
    }

private:
    flatzebra::KeyState startKS;
    flatzebra::KeyState pauseKS;
    flatzebra::KeyState quitKS;
    flatzebra::KeyState leftKS;
    flatzebra::KeyState rightKS;
    flatzebra::KeyState upKS;
    flatzebra::KeyState downKS;
    flatzebra::KeyState shootKS;
    flatzebra::KeyState saveKS;
    flatzebra::KeyState loadKS;
    flatzebra::KeyState fullScreenKS;
    flatzebra::Joystick joystick;

    // Joystick button indexes:
    int startBI;
    int pauseBI;
    int shootBI;
};

#endif  /* _H_Controller */
