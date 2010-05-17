/*  $Id: EnemySprite.cpp,v 1.6.2.5 2010/05/09 01:18:46 sarrazip Exp $
    EnemySprite.cpp - Sprite that represents an enemy of the player.

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

#include "EnemySprite.h"

#include <stdio.h>

using namespace flatzebra;


EnemySprite::EnemySprite(const PixmapArray &pa,
                            Couple pos, Couple speed,
                            Couple collBoxPos, Couple collBosSize) throw(int)
  : Sprite(pa, pos, speed, Couple(0, 0), collBoxPos, collBosSize),
    carryingGroup(NULL),
    disappearanceTime(0),
    timeToDie(0),
    lastDirection(-1),
    climbingFromPlate(false)
{
}


EnemySprite::~EnemySprite()
{
}


void
EnemySprite::setCarryingGroup(IngredientGroup *g)
{
    carryingGroup = g;
}


void
EnemySprite::setLastDirection(int dir)
{
    lastDirection = dir;
}


int
EnemySprite::getLastDirection() const
{
    return lastDirection;
}


void
EnemySprite::setClimbingFromPlate(bool climbing)
{
    climbingFromPlate = climbing;
}


bool
EnemySprite::isClimbingFromPlate() const
{
    return climbingFromPlate;
}
