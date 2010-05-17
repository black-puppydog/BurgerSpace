/*  $Id: IngredientGroup.h,v 1.11.2.3 2010/05/16 02:25:48 sarrazip Exp $
    IngredientGroup.h - Group of sprites representing an "ingredient"

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

#ifndef _H_IngredientGroup
#define _H_IngredientGroup

#include <flatzebra/Sprite.h>

#include <assert.h>
#include <vector>

class IngredientSprite;
class EnemySprite;


class IngredientGroup
/*  An ingredient is represented as 4 sprites.
    An object of this class stores pointers to Sprite objects
    that belong to the same group.
    They are stored in a vector of Sprite pointers,
    in left-to-right order.
    This object does not own the Sprite pointers that it stores.
*/
{
public:

    typedef std::vector<class IngredientGroup *> List;

    enum State { NORMAL, FALL1, BOUNCE, STALL, FALL2, REST };

    IngredientGroup(int yTarget, bool topBun);
    /*  Constructs an empty group that has the given vertical target.
        The vertical target is the position where the group's member
        sprites must stop falling.
        'topBun' indicates if this group represents a top bun.
        See the method isTopBun().
    */

    ~IngredientGroup();
    /*  Does not delete the member sprites.
    */

    void setMember(size_t index, IngredientSprite *s);
    /*  This object does not own the member sprites.
    */

    IngredientSprite *getMember(size_t index) const
    {
        assert(index < 4);
        assert(members[index] != NULL);
        return members[index];
    }

    size_t getNumMembers() const
    {
        return 4;
    }

    bool areAllMembersLowered() const;

    void startFalling(int speedFactor, int numOfFloorsToGo = 1);
    /*  Gives an downward speed of 'speedFactor' to the sprites of this group
        and to the sprites of the carried enemies.
        Puts this group in the FALL1 state.
        'numOfFloorsToGo' must be the number of floors that must be
        travelled by this group.
    */

    State getState() const { return state; }

    void bounce(int speedFactor);
    /*  Gives an upward speed of 'speedFactor' to the sprites of this group
        and to the sprites of the carried enemies.
        Puts this group in the BOUNCE state and sets the bounce time
        to a positive number of ticks.
    */

    void stall();
    /*  Stops the sprites of this group and the sprites of the carried enemies.
        Puts this group in the STALL state and sets the stall time
        to a positive number of ticks.
    */

    void fallBack(int speedFactor);
    /*  Gives an downward speed of 'speedFactor' to the sprites of this group
        and to the sprites of the carried enemies.
        Puts this group in the FALL2 state.
    */

    void fallToNextFloor(int speedFactor);
    /*  Gives an downward speed of 'speedFactor' to the sprites of this group
        and to the sprites of the carried enemies.
        Puts this group in the FALL1 state.
    */

    void stop();
    /*  Sets the speed of all members to zero.
        Makes all members normal (i.e., not lowered).
        Aligns all members of the vertical position of the first member.
    */

    int  getVerticalTarget() const { return verticalTarget; }

    void addCarriedEnemy(EnemySprite *s);
    /*  Adds 's' to this group's list of carried enemies.
        Tells 's' that this group is its carrying group.
    */

    const flatzebra::SpriteList &getCarriedEnemies() const { return carriedEnemies; }
    flatzebra::SpriteList &getCarriedEnemies() { return carriedEnemies; }
    void clearCarriedEnemies() { carriedEnemies.clear(); }

    int getBounceTime() const { return bounceTime; }
    int decBounceTime()
            { if (bounceTime != 0) bounceTime--; return bounceTime; }
    int getStallTime() const { return stallTime; }
    int decStallTime()
            { if (stallTime != 0) stallTime--; return stallTime; }

    void   setNumFloorsToGo(size_t n) { numFloorsToGo = n; }
    size_t getNumFloorsToGo() const { return numFloorsToGo; }
    size_t decNumFloorsToGo()
            { if (numFloorsToGo != 0) numFloorsToGo--; return numFloorsToGo; }
    flatzebra::Couple getCenterPos() const;

    bool isTopBun() const { return topBunFlag; }
    /*  Indicates if this group represents a top bun.
        When a top bun reaches the plate, the corresponding hamburger
        is done.  When all hamburgers are done, the level is done.
    */

    void restore(int bounceTime, int stallTime,
                            State state, size_t numFloorsToGo)
    {
        this->bounceTime = bounceTime;
        this->stallTime = stallTime;
        this->state = state;
        this->numFloorsToGo = numFloorsToGo;
    }

private:

    IngredientSprite *members[4];
    int verticalTarget;
    flatzebra::SpriteList carriedEnemies;
    int bounceTime;
    int stallTime;
    State state;
    size_t numFloorsToGo;
    bool topBunFlag;

    // Forbidden operations:
    IngredientGroup(const IngredientGroup &);
    IngredientGroup &operator = (const IngredientGroup &);

};


#endif  /* _H_IngredientGroup */
