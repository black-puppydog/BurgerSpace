/*  $Id: IngredientSprite.h,v 1.9.2.2 2010/05/16 02:25:48 sarrazip Exp $
    IngredientSprite.h - Sprite that is part of an "ingredient"

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

#ifndef _H_IngredientSprite
#define _H_IngredientSprite

#include <flatzebra/Sprite.h>

#include <vector>

class IngredientGroup;


class IngredientSprite : public flatzebra::Sprite
/*  This object does not own the 'IngredientGroup' to which it refers.
*/
{
public:

    typedef std::vector<class IngredientSprite *> List;

    enum { LOWERING_DISTANCE = 4 };

    IngredientSprite(const flatzebra::PixmapArray &pixmapArray,
                        const flatzebra::Couple &pos,
                        IngredientGroup *ig) throw(int)
      : flatzebra::Sprite(pixmapArray, pos,
                flatzebra::Couple(0, 0), flatzebra::Couple(0, 0),
                flatzebra::Couple(0, 5), flatzebra::Couple(24, 1)),
        myGroup(ig),
        lowered(false)
    {
        assert(ig != NULL);
    }

    IngredientGroup *getGroup() const { return myGroup; }

    void lower() { getPos().y += LOWERING_DISTANCE; lowered = true; }
    void setLowered() { lowered = true; }
    void setNormal() { lowered = false; }

    bool isLowered() const { return lowered; }

private:

    IngredientGroup *myGroup;
    bool lowered;

    // Forbidden operations:
    IngredientSprite(const IngredientSprite &);
    IngredientSprite &operator = (const IngredientSprite &);

};


#endif  /* _H_IngredientSprite */
