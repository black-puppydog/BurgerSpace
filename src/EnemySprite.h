/*  $Id: EnemySprite.h,v 1.8 2007/12/22 23:03:35 sarrazip Exp $
    EnemySprite.h - Sprite that represents an enemy of the player.

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

#ifndef _H_EnemySprite
#define _H_EnemySprite

#include <flatzebra/Couple.h>
#include <flatzebra/Sprite.h>

class IngredientGroup;


class EnemySprite : public flatzebra::Sprite
/*  Sprite that represents an enemy of the player.

    The "time to live" for this sprite is used to represent the time
    of paralysis due to the pepper.

    The member 'disappearanceTime' is used when the sprite has just
    finished being carried by an ingredient group.
*/
{
public:

    EnemySprite(const flatzebra::PixmapArray &pa,
		flatzebra::Couple pos,
		flatzebra::Couple speed,
		flatzebra::Couple collBoxPos,
		flatzebra::Couple collBosSize) throw(int);
    /*  Calls Sprite constructor.
    */

    virtual ~EnemySprite();
    /*  Nothing interesting.
    */

    void setCarryingGroup(IngredientGroup *g);
    /*  Sets the ingredient group that is currently carrying this enemy.
	'g' can be null, to indicate that the enemy is not carried.

	This object never owns an IngredientGroup object.
    */

    IngredientGroup *getCarryingGroup() const;
    /*  Returns a pointer to the ingredient group that is currently
	carrying this enemy.
	This pointer can be null, to indicate that the enemy is not carried.
    */

    void setDisappearanceTime(int t);
    int getDisappearanceTime() const;
    int decDisappearanceTime();

    void setTimeToDie(int t);
    int  getTimeToDie() const;
    int  decTimeToDie();

private:

    IngredientGroup *carryingGroup;
	/*  Designates the ingredient group that is carrying this enemy,
	    if applicable; NULL normally.
	*/

    int disappearanceTime;
	/*  Decreasing counter of the number of ticks for which this
	    sprite will be insivible.
	    Used typically when the sprite has just finished being
	    carried by an ingredient group.
	*/

    int timeToDie;

};


inline IngredientGroup *EnemySprite::getCarryingGroup() const
{
    return carryingGroup;
}
inline void EnemySprite::setDisappearanceTime(int t)
{
    disappearanceTime = t;
}
inline int EnemySprite::getDisappearanceTime() const
{
    return disappearanceTime;
}
inline int EnemySprite::decDisappearanceTime()
{
    if (disappearanceTime != 0)
	disappearanceTime--;
    return disappearanceTime;
}
inline void EnemySprite::setTimeToDie(int t)
{
    timeToDie = t;
}
inline int EnemySprite::getTimeToDie() const
{
    return timeToDie;
}
inline int EnemySprite::decTimeToDie()
{
    if (timeToDie != 0)
	timeToDie--;
    return timeToDie;
}


#endif  /* _H_EnemySprite */
