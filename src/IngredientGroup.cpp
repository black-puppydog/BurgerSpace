/*  $Id: IngredientGroup.cpp,v 1.8.2.2 2010/05/09 01:18:46 sarrazip Exp $
    IngredientGroup.cpp - Group of sprites representing an "ingredient"

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

#include "IngredientGroup.h"

#include "IngredientSprite.h"
#include "EnemySprite.h"

using namespace flatzebra;


IngredientGroup::IngredientGroup(int yTarget, bool topBun)
  : members(),
    verticalTarget(yTarget),
    carriedEnemies(),
    bounceTime(0),
    stallTime(0),
    state(NORMAL),
    numFloorsToGo(0),
    topBunFlag(topBun)
{
    members[0] = members[1] = members[2] = members[3] = NULL;
}


IngredientGroup::~IngredientGroup()
{
}


void IngredientGroup::setMember(size_t index, IngredientSprite *s)
{
    assert(index < 4);
    assert(s != NULL);
    members[index] = s;
}


bool
IngredientGroup::areAllMembersLowered() const
{
    size_t j;
    for (j = 0; j < 4; j++)
        if (!members[j]->isLowered())
            return false;
    return true;
}


void
IngredientGroup::startFalling(int speedFactor, int numOfFloorsToGo)
{
    for (size_t j = 0; j < 4; j++)
    {
        members[j]->setSpeed(Couple(0, speedFactor));
        if (members[j]->isLowered())
            members[j]->getPos().y -= IngredientSprite::LOWERING_DISTANCE;
        else
            members[j]->setLowered();
    }

    state = FALL1;
    numFloorsToGo = numOfFloorsToGo;
}


void
IngredientGroup::fallToNextFloor(int speedFactor)
{
    for (size_t j = 0; j < 4; j++)
        members[j]->setSpeed(Couple(0, speedFactor));

    state = FALL1;
}


void
IngredientGroup::addCarriedEnemy(EnemySprite *s)
{
    assert(s != NULL);
    carriedEnemies.push_back(s);
    s->setCarryingGroup(this);
}


void
IngredientGroup::bounce(int speedFactor)
{
    for (size_t j = 0; j < 4; j++)
        members[j]->setSpeed(Couple(0, -speedFactor));

    for (SpriteList::iterator it = carriedEnemies.begin();
                                    it != carriedEnemies.end(); it++)
        (*it)->setSpeed(Couple(0, -speedFactor));

    state = BOUNCE;
    bounceTime = 4;
}


void
IngredientGroup::stall()
{
    for (size_t j = 0; j < 4; j++)
        members[j]->getSpeed().zero();

    for (SpriteList::iterator it = carriedEnemies.begin();
                                    it != carriedEnemies.end(); it++)
        (*it)->getSpeed().zero();

    state = STALL;
    stallTime = 2;
}


void
IngredientGroup::fallBack(int speedFactor)
{
    for (size_t j = 0; j < 4; j++)
        members[j]->setSpeed(Couple(0, +speedFactor));

    for (SpriteList::iterator it = carriedEnemies.begin();
                                    it != carriedEnemies.end(); it++)
        (*it)->setSpeed(Couple(0, +speedFactor));

    state = FALL2;
}


void
IngredientGroup::stop()
{
    IngredientSprite *firstMember = members[0];
    for (size_t j = 0; j < 4; j++)
    {
        IngredientSprite *m = members[j];

        // align other members with this one:
        m->getPos().y = firstMember->getPos().y;

        m->setNormal();  // not lowered anymore
        m->getSpeed().zero();
    }

    state = NORMAL;
    numFloorsToGo = 0;
}


Couple IngredientGroup::getCenterPos() const
{
    const Couple groupPos = getMember(0)->getPos();
    const Couple groupLRP = getMember(3)->getLowerRightPos();
    return (groupPos + groupLRP) / 2;
}
