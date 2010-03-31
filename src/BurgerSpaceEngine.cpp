/*  $Id: BurgerSpaceEngine.cpp,v 1.40 2010/03/21 15:58:30 sarrazip Exp $
    BurgerSpaceEngine.cpp - Main engine

    burgerspace - A hamburger-smashing video game.
    Copyright (C) 2001-2009 Pierre Sarrazin <http://sarrazip.com/>

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

#include "BurgerSpaceEngine.h"

#include "EnemySprite.h"

#include <flatzebra/PixmapLoadError.h>

#include <assert.h>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>


/*  Tiles:
*/
#include "images/empty.xpm"
#include "images/floor-and-ladder.xpm"
#include "images/floor.xpm"
#include "images/ladder.xpm"
#include "images/plate.xpm"


/*  Sprite images:
*/
#include "images/chef-front0.xpm"
#include "images/chef-front1.xpm"
#include "images/chef-front2.xpm"
#include "images/chef-back0.xpm"
#include "images/chef-back1.xpm"
#include "images/chef-back2.xpm"
#include "images/chef-left0.xpm"
#include "images/chef-left1.xpm"
#include "images/chef-left2.xpm"
#include "images/chef-right0.xpm"
#include "images/chef-right1.xpm"
#include "images/chef-right2.xpm"
#include "images/chef-dead0.xpm"
#include "images/chef-dead1.xpm"
#include "images/chef-dead2.xpm"
#include "images/chef-dead3.xpm"
#include "images/chef-dead4.xpm"
#include "images/chef-dead5.xpm"

#include "images/egg-front0.xpm"
#include "images/egg-front1.xpm"
#include "images/egg-back0.xpm"
#include "images/egg-back1.xpm"
#include "images/egg-left0.xpm"
#include "images/egg-left1.xpm"
#include "images/egg-right0.xpm"
#include "images/egg-right1.xpm"
#include "images/egg-dead0.xpm"
#include "images/egg-dead1.xpm"
#include "images/egg-dead2.xpm"
#include "images/egg-dead3.xpm"
#include "images/egg-peppered0.xpm"
#include "images/egg-peppered1.xpm"

#include "images/hotdog-front0.xpm"
#include "images/hotdog-front1.xpm"
#include "images/hotdog-back0.xpm"
#include "images/hotdog-back1.xpm"
#include "images/hotdog-left0.xpm"
#include "images/hotdog-left1.xpm"
#include "images/hotdog-right0.xpm"
#include "images/hotdog-right1.xpm"
#include "images/hotdog-dead0.xpm"
#include "images/hotdog-dead1.xpm"
#include "images/hotdog-dead2.xpm"
#include "images/hotdog-dead3.xpm"
#include "images/hotdog-peppered0.xpm"
#include "images/hotdog-peppered1.xpm"

#include "images/pickle-front0.xpm"
#include "images/pickle-front1.xpm"
#include "images/pickle-back0.xpm"
#include "images/pickle-back1.xpm"
#include "images/pickle-left0.xpm"
#include "images/pickle-left1.xpm"
#include "images/pickle-right0.xpm"
#include "images/pickle-right1.xpm"
#include "images/pickle-dead0.xpm"
#include "images/pickle-dead1.xpm"
#include "images/pickle-dead2.xpm"
#include "images/pickle-dead3.xpm"
#include "images/pickle-peppered0.xpm"
#include "images/pickle-peppered1.xpm"

#include "images/top-bun0.xpm"
#include "images/top-bun1.xpm"
#include "images/top-bun2.xpm"
#include "images/top-bun3.xpm"
#include "images/lettuce0.xpm"
#include "images/lettuce1.xpm"
#include "images/lettuce2.xpm"
#include "images/lettuce3.xpm"
#include "images/redstuff0.xpm"
#include "images/redstuff1.xpm"
#include "images/redstuff2.xpm"
#include "images/redstuff3.xpm"
#include "images/yellowstuff0.xpm"
#include "images/yellowstuff1.xpm"
#include "images/yellowstuff2.xpm"
#include "images/yellowstuff3.xpm"
#include "images/meat0.xpm"
#include "images/meat1.xpm"
#include "images/meat2.xpm"
#include "images/meat3.xpm"
#include "images/bottom-bun0.xpm"
#include "images/bottom-bun1.xpm"
#include "images/bottom-bun2.xpm"
#include "images/bottom-bun3.xpm"

#include "images/pepper0.xpm"
#include "images/pepper1.xpm"

#include "images/icecream.xpm"
#include "images/coffee.xpm"
#include "images/fries.xpm"

#include "images/digit0.xpm"
#include "images/digit1.xpm"
#include "images/digit2.xpm"
#include "images/digit3.xpm"
#include "images/digit4.xpm"
#include "images/digit5.xpm"
#include "images/digit6.xpm"
#include "images/digit7.xpm"
#include "images/digit8.xpm"
#include "images/digit9.xpm"

using namespace std;
using namespace flatzebra;

#define __(s) (s)  /* Gettext-like macro */

#ifdef _MSC_VER
	#define VERSION ""
	#define snprintf _snprintf
#endif


///////////////////////////////////////////////////////////////////////////////
//
// CONSTANTS
//

static const int
    FRAMES_PER_SECOND = 20,
    TILE_SIDE = 32,
    FLOOR_POS = TILE_SIDE - 6,  // position of the floor's surface, in a tile
    INGREDIENT_FALL_SPEED = 8,
    PLAYER_SPEED_FACTOR = 4,
    ENEMY_SPEED_FACTOR = 3,
    SCORE_TTL = 20,
    NUM_INIT_LIVES = 3,
    NUM_LEVELS = 6,
    CORNER_TURN_TOLERANCE = 8;


///////////////////////////////////////////////////////////////////////////////
//
// UTILITY FUNCTIONS
//

inline
int
Rnd(int lowerLimit, int upperLimit)
{
    return rand() % (upperLimit - lowerLimit + 1) + lowerLimit;
}


static
void
removeNullElementsFromSpriteList(SpriteList &slist)
{
    SpriteList::iterator it =
		    remove(slist.begin(), slist.end(), (Sprite *) NULL);
	/*  remove() has "packed" the remaining elements at the beginning
	    of the sequence, but has not shortened the list.  This must
	    be done by a call to the erase() method.  Doesn't this seem
	    unintuitive?  I thought remove() removed stuff.
	    @sarrazip 20010501
	*/
    slist.erase(it, slist.end());
}


static
void
deleteSprite(Sprite *p)
{
    delete p;
}


static
void
deleteIngredientGroup(IngredientGroup *p)
{
    delete p;
}


template <class Container>
static
void
deleteSprites(Container &c)
{
    for_each(c.begin(), c.end(), deleteSprite);
    c.clear();
}


static
string
itoa(unsigned long integer)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%ld", integer);
    return buffer;
}


static
Couple
getCoupleFromDirection(int direction, int amplitude)
{
    switch (direction)
    {
	case RIGHT:  return Couple(+amplitude, 0);
	case UP   :  return Couple(0, -amplitude);
	case LEFT :  return Couple(-amplitude, 0);
	case DOWN :  return Couple(0, +amplitude);
	default: assert(false); return Couple(0, 0);
    }
}


static
int
getDirectionFromCouple(Couple v)
{
    assert((v.x != 0) ^ (v.y != 0));  // either x or y must be zero, not both

    if (v.x == 0)
	return (v.y < 0 ? UP : DOWN);
    return (v.x < 0 ? LEFT : RIGHT);
}


int
makeDivisibleByInt(int n, int divisor)
/*  Round 'n' to the lowest multiple of divisor.
    Works also if 'n' is negative.
*/
{
    if (n >= 0)
	return n / divisor * divisor;
    return -((-n + divisor - 1) / divisor * divisor);
}


const char *
getDirectionName(int d)
{
    switch (d)
    {
	case RIGHT: return "right";
	case UP:    return "up";
	case LEFT:  return "left";
	case DOWN:  return "down";
	case -1:    return "none";
	default:    return "unknown";
    }
}


static
string
getDir(const char *defaultValue, const char *envVarName)
/*
    Makes sure that the returned directory name ends with a slash.
*/
{
    string dir;
    const char *s = getenv(envVarName);
    if (s != NULL)
	dir = s;
    else
	dir = defaultValue;
    
    if (!dir.empty() && dir[dir.length() - 1] != '/')
	dir += '/';

    return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
// STATIC DATA MEMBERS
//


/*static*/
const char *
BurgerSpaceEngine::levelDescriptor1[] =
// Description of the first level's setting.
{
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeetffftftftffftfffteee",
    "eeeleeeleleleeeleeeleee",
    "eeefftfteletftftfffteee",
    "eeeeeletfffteleleeeleee",
    "eeetftfteeeleletftffeee",
    "eeeleletffftfffteleeeee",
    "eeeleleleeeleeetftfteee",
    "eeetffftffftfffteleleee",
    "eeeleeeleeeleeeleleleee",
    "eeefffffffffffffffffeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeepppepppepppepppeeee",
    NULL  // marks the end
};


/*static*/
const BurgerSpaceEngine::IngInit
BurgerSpaceEngine::tableIngredientsLevel1[] =
{
    // 1st column:
    {  4, 11, 15,  1, IngInit::BOTTOM_BUN },
    {  4,  9, 15,  2, IngInit::MEAT       },
    {  4,  6, 15,  3, IngInit::LETTUCE    },
    {  4,  4, 15,  4, IngInit::TOP_BUN    },

    // 2nd column:
    {  8, 11, 15,  1, IngInit::BOTTOM_BUN },
    {  8,  9, 15,  2, IngInit::MEAT       },
    {  8,  7, 15,  3, IngInit::LETTUCE    },
    {  8,  2, 15,  4, IngInit::TOP_BUN    },

    // 3rd column:
    { 12, 11, 15,  1, IngInit::BOTTOM_BUN },
    { 12,  7, 15,  2, IngInit::MEAT       },
    { 12,  4, 15,  3, IngInit::LETTUCE    },
    { 12,  2, 15,  4, IngInit::TOP_BUN    },

    // 4th column:
    { 16,  8, 15,  1, IngInit::BOTTOM_BUN },
    { 16,  6, 15,  2, IngInit::MEAT       },
    { 16,  4, 15,  3, IngInit::LETTUCE    },
    { 16,  2, 15,  4, IngInit::TOP_BUN    },

    {  0,  0,  0,  0, IngInit::BOTTOM_BUN }  // rank zero marks the end
};


const char *
BurgerSpaceEngine::levelDescriptor2[] =
// Description of the second level's setting.
{
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeetftftftftftftftfteee",
    "eeetftftftfteleleleleee",
    "eeetftfteletftftftfteee",
    "eeeleletftfteletftfteee",
    "eeefffffftftftffffffeee",
    "eeeeeeeeeletftffeeeeeee",
    "eeeeeeefftfteleeeeeeeee",
    "eeeeeeeeeletftffeeeeeee",
    "eeeeeeefffftffffeeeeeee",
    "eeeepppeeeeleeeepppeeee",
    "eeeeeeeeeeefeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeepppepppeeeeeeee",
    NULL  // marks the end
};


/*static*/ const BurgerSpaceEngine::IngInit
BurgerSpaceEngine::tableIngredientsLevel2[] =
{
    // 1st column:
    {  4,  6, 11,  1, IngInit::BOTTOM_BUN },
    {  4,  4, 11,  2, IngInit::MEAT       },
    {  4,  3, 11,  3, IngInit::LETTUCE    },
    {  4,  2, 11,  4, IngInit::TOP_BUN    },

    // 2nd column:
    {  8, 10, 15,  1, IngInit::BOTTOM_BUN },
    {  8,  5, 15,  2, IngInit::MEAT       },
    {  8,  3, 15,  3, IngInit::LETTUCE    },
    {  8,  2, 15,  4, IngInit::TOP_BUN    },

    // 3rd column:
    { 12, 10, 15,  1, IngInit::BOTTOM_BUN },
    { 12,  9, 15,  2, IngInit::MEAT       },
    { 12,  7, 15,  3, IngInit::LETTUCE    },
    { 12,  2, 15,  4, IngInit::TOP_BUN    },

    // 4th column:
    { 16,  6, 11,  1, IngInit::BOTTOM_BUN },
    { 16,  5, 11,  2, IngInit::MEAT       },
    { 16,  4, 11,  3, IngInit::LETTUCE    },
    { 16,  2, 11,  4, IngInit::TOP_BUN    },

    {  0,  0,  0,  0, IngInit::BOTTOM_BUN }  // rank zero marks the end
};


const char *
BurgerSpaceEngine::levelDescriptor3[] =
// Description of the second level's setting.
{
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeetffftftffftfffffteee",
    "eeetffftffftffftfffteee",
    "eeetffffftfteeetfffteee",
    "eeefffftftftftffffffeee",
    "eeeeeeeteletfffteeeeeee",
    "eeeeeeetfffffffteeeeeee",
    "eeeepppleeeeeeelpppeeee",
    "eeetftfteeeeeeetfffteee",
    "eeetfffteeeeeeetftfteee",
    "eeefffftpppeppptffffeee",
    "eeeeeeetftftftfteeeeeee",
    "eeeeeeefffffffffeeeeeee",
    "eeeepppeeeeeeeeepppeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    NULL  // marks the end
};


/*static*/ const BurgerSpaceEngine::IngInit
BurgerSpaceEngine::tableIngredientsLevel3[] =
{
    // 1st column, upper hamburger:
    {  4,  5,  8,  1, IngInit::BOTTOM_BUN },
    {  4,  4,  8,  2, IngInit::MEAT       },
    {  4,  2,  8,  3, IngInit::TOP_BUN    },

    // 1st column, lower hamburger:
    {  4, 11, 14,  1, IngInit::BOTTOM_BUN },
    {  4, 10, 14,  2, IngInit::MEAT       },
    {  4,  9, 14,  3, IngInit::TOP_BUN    },

    // 2nd column:
    {  8,  5, 11,  1, IngInit::BOTTOM_BUN },
    {  8,  3, 11,  2, IngInit::MEAT       },
    {  8,  2, 11,  3, IngInit::TOP_BUN    },

    // 3rd column:
    { 12,  5, 11,  1, IngInit::BOTTOM_BUN },
    { 12,  3, 11,  2, IngInit::MEAT       },
    { 12,  2, 11,  3, IngInit::TOP_BUN    },

    // 4th column, upper hamburger:
    { 16,  5,  8,  1, IngInit::BOTTOM_BUN },
    { 16,  4,  8,  2, IngInit::MEAT       },
    { 16,  2,  8,  3, IngInit::TOP_BUN    },

    // 4th column, lower hamburger:
    { 16, 11, 14,  1, IngInit::BOTTOM_BUN },
    { 16, 10, 14,  2, IngInit::MEAT       },
    { 16,  9, 14,  3, IngInit::TOP_BUN    },

    {  0,  0,  0,  0, IngInit::BOTTOM_BUN }  // rank zero marks the end
};


const char *
BurgerSpaceEngine::levelDescriptor4[] =
// Description of the second level's setting.
{
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeetffftffftffftfffteee",
    "eeefftffftffftffftffeee",
    "eeetffftffftffftfffteee",
    "eeefftffftftftffftffeee",
    "eeetffftffftffftfffteee",
    "eeetftffftffftffftfteee",
    "eeetffftffftffftfffteee",
    "eeefffffffffffffffffeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeepppepppepppepppeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    NULL  // marks the end
};


/*static*/ const BurgerSpaceEngine::IngInit
BurgerSpaceEngine::tableIngredientsLevel4[] =
{
    // 1st column:
    {  4,  9, 14,  1, IngInit::BOTTOM_BUN   },
    {  4,  8, 14,  2, IngInit::RED_STUFF    },
    {  4,  7, 14,  3, IngInit::LETTUCE      },
    {  4,  6, 14,  4, IngInit::MEAT         },
    {  4,  5, 14,  5, IngInit::LETTUCE      },
    {  4,  4, 14,  6, IngInit::RED_STUFF    },
    {  4,  3, 14,  7, IngInit::YELLOW_STUFF },
    {  4,  2, 14,  8, IngInit::TOP_BUN      },

    // 2nd column:
    {  8,  9, 14,  1, IngInit::BOTTOM_BUN   },
    {  8,  8, 14,  2, IngInit::LETTUCE      },
    {  8,  7, 14,  3, IngInit::YELLOW_STUFF },
    {  8,  6, 14,  4, IngInit::RED_STUFF    },
    {  8,  5, 14,  5, IngInit::MEAT         },
    {  8,  4, 14,  6, IngInit::RED_STUFF    },
    {  8,  3, 14,  7, IngInit::LETTUCE      },
    {  8,  2, 14,  8, IngInit::TOP_BUN      },

    // 3rd column:
    { 12,  9, 14,  1, IngInit::BOTTOM_BUN   },
    { 12,  8, 14,  2, IngInit::LETTUCE      },
    { 12,  7, 14,  3, IngInit::YELLOW_STUFF },
    { 12,  6, 14,  4, IngInit::MEAT         },
    { 12,  5, 14,  5, IngInit::RED_STUFF    },
    { 12,  4, 14,  6, IngInit::LETTUCE      },
    { 12,  3, 14,  7, IngInit::RED_STUFF    },
    { 12,  2, 14,  8, IngInit::TOP_BUN      },

    // 4th column:
    { 16,  9, 14,  1, IngInit::BOTTOM_BUN   },
    { 16,  8, 14,  2, IngInit::RED_STUFF    },
    { 16,  7, 14,  3, IngInit::LETTUCE      },
    { 16,  6, 14,  4, IngInit::RED_STUFF    },
    { 16,  5, 14,  5, IngInit::MEAT         },
    { 16,  4, 14,  6, IngInit::LETTUCE      },
    { 16,  3, 14,  7, IngInit::YELLOW_STUFF },
    { 16,  2, 14,  8, IngInit::TOP_BUN      },

    {  0,  0,  0,  0, IngInit::BOTTOM_BUN }  // rank zero marks the end
};


const char *
BurgerSpaceEngine::levelDescriptor5[] =
// Description of the second level's setting.
{
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeetffffffffffffftfteee",
    "eeefffffffftffftftfteee",
    "eeetffffffffffffeleleee",
    "eeetfffffffffffteleleee",
    "eeetffftffffffffftfteee",
    "eeetfffffffffffteleleee",
    "eeetfffffffffffteleleee",
    "eeetffffffffffffeleleee",
    "eeeleeeeeeeeeeetfffteee",
    "eeeleeeeeeeeeeeleeeleee",
    "eeeleeeeeeeeeeeleeeleee",
    "eeefeeeeeeeeeeefffffeee",
    "eeeeeeeepppepppeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    NULL  // marks the end
};


/*static*/ const BurgerSpaceEngine::IngInit
BurgerSpaceEngine::tableIngredientsLevel5[] =
{
    // 1st column:
    {  8,  9, 14,  1, IngInit::BOTTOM_BUN   },
    {  8,  8, 14,  2, IngInit::RED_STUFF    },
    {  8,  7, 14,  3, IngInit::MEAT         },
    {  8,  6, 14,  4, IngInit::LETTUCE      },
    {  8,  5, 14,  5, IngInit::MEAT         },
    {  8,  4, 14,  6, IngInit::LETTUCE      },
    {  8,  3, 14,  7, IngInit::RED_STUFF    },
    {  8,  2, 14,  8, IngInit::TOP_BUN      },

    // 2nd column:
    { 12,  9, 14,  1, IngInit::BOTTOM_BUN   },
    { 12,  8, 14,  2, IngInit::LETTUCE      },
    { 12,  7, 14,  3, IngInit::RED_STUFF    },
    { 12,  6, 14,  4, IngInit::MEAT         },
    { 12,  5, 14,  5, IngInit::LETTUCE      },
    { 12,  4, 14,  6, IngInit::MEAT         },
    { 12,  3, 14,  7, IngInit::RED_STUFF    },
    { 12,  2, 14,  8, IngInit::TOP_BUN      },

    {  0,  0,  0,  0, IngInit::BOTTOM_BUN }  // rank zero marks the end
};


const char *
BurgerSpaceEngine::levelDescriptor6[] =
// Description of the second level's setting.
{
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeetftffeeetffffeee",
    "eeefffftelefftffeeeeeee",
    "eeeeeeetftfteletffffeee",
    "eeefffffeletfffteeeeeee",
    "eeeeeeetfffteeetffffeee",
    "eeeffffteeefftffeeeeeee",
    "eeeeeeetftfteletffffeee",
    "eeefffffelefftfteeeeeee",
    "eeeeeeetfffteletffffeee",
    "eeefffffeeefffffeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeepppepppepppepppeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    NULL  // marks the end
};


/*static*/ const BurgerSpaceEngine::IngInit
BurgerSpaceEngine::tableIngredientsLevel6[] =
{
    // 1st column:
    {  4,  9, 14,  1, IngInit::BOTTOM_BUN   },
    {  4,  7, 14,  2, IngInit::MEAT         },
    {  4,  5, 14,  3, IngInit::YELLOW_STUFF },
    {  4,  3, 14,  4, IngInit::TOP_BUN      },

    // 2nd column:
    {  8, 10, 14,  1, IngInit::BOTTOM_BUN   },
    {  8,  8, 14,  2, IngInit::YELLOW_STUFF },
    {  8,  6, 14,  3, IngInit::MEAT         },
    {  8,  4, 14,  4, IngInit::YELLOW_STUFF },
    {  8,  2, 14,  5, IngInit::TOP_BUN      },

    // 3rd column:
    { 12, 11, 14,  1, IngInit::BOTTOM_BUN   },
    { 12,  9, 14,  2, IngInit::MEAT         },
    { 12,  7, 14,  3, IngInit::YELLOW_STUFF },
    { 12,  5, 14,  4, IngInit::YELLOW_STUFF },
    { 12,  3, 14,  5, IngInit::TOP_BUN      },

    // 4th column:
    { 16,  8, 14,  1, IngInit::BOTTOM_BUN   },
    { 16,  6, 14,  2, IngInit::MEAT         },
    { 16,  4, 14,  3, IngInit::YELLOW_STUFF },
    { 16,  2, 14,  4, IngInit::TOP_BUN      },

    {  0,  0,  0,  0, IngInit::BOTTOM_BUN }  // rank zero marks the end
};



const char **
BurgerSpaceEngine::levelDescriptorTable[] =
{
    0,  // unused
    levelDescriptor1,  // cheap system, indeed
    levelDescriptor2,
    levelDescriptor3,
    levelDescriptor4,
    levelDescriptor5,
    levelDescriptor6,
    0,  // safety padding
    0,  // safety padding
    0,  // safety padding
    0,  // safety padding
};



const BurgerSpaceEngine::IngInit *
BurgerSpaceEngine::tableOfTablesOfIngredientsLevel[] =
{
    NULL,  // unused
    tableIngredientsLevel1,
    tableIngredientsLevel2,
    tableIngredientsLevel3,
    tableIngredientsLevel4,
    tableIngredientsLevel5,
    tableIngredientsLevel6,
    NULL,  // safety padding
    NULL,  // safety padding
    NULL,  // safety padding
    NULL,  // safety padding
};


const BurgerSpaceEngine::IntQuad
BurgerSpaceEngine::enemyStartingHeights[] =
/*  The component at index L (>= 1) has 4 integer elements:
    'first' and 'second' are heights in tiles where the enemies can appear
    when at level L, and coming from the left.
    'third' and 'fourth' are heights in tiles where the enemies can appear
    when at level L, and coming from the right.
*/
{
    {  0,  0,  0,  0 },  // index 0 not used
    {  2, 11,  2, 11 },  // level 1
    {  2,  6,  2,  6 },  // level 2
    {  2, 11,  2, 11 },  // level 3
    {  2,  9,  2,  9 },  // level 4
    {  2, 13,  2, 13 },  // level 5
    {  3, 11,  2, 10 },  // level 6
    {  0,  0,  0,  0 },  // safety padding
    {  0,  0,  0,  0 },  // safety padding
    {  0,  0,  0,  0 },  // safety padding
    {  0,  0,  0,  0 },  // safety padding
};


const BurgerSpaceEngine::IntPair
BurgerSpaceEngine::playerStartingPos[] =
{
    { 11,  0 },  // index 0 not used
    { 11, 11 },  // level 1
    { 11, 12 },  // level 2
    { 11, 13 },  // level 3
    { 11,  9 },  // level 4
    { 15, 13 },  // level 5
    { 11, 11 },  // level 6
    {  0,  0 },  // safety padding
    {  0,  0 },  // safety padding
    {  0,  0 },  // safety padding
    {  0,  0 },  // safety padding
};


///////////////////////////////////////////////////////////////////////////////


BurgerSpaceEngine::Level::Level()
{
    levelNo = 1;
    xpmMatrix = NULL;
    tileMatrix = NULL;
    sizeInTiles = sizeInPixels = positionInPixels = Couple(0, 0);
}


BurgerSpaceEngine::Level::~Level()
{
    // NOTE:  We do not free the pixmaps.  The program is dying anyway.

    delete [] xpmMatrix;
    delete [] tileMatrix;
}


void
BurgerSpaceEngine::Level::init(int no, int nCols, int nRows, Couple posInPixels)
/*  nCols and nRows must be the number of columns and rows
    of _tiles_, not of pixels.
    posInPixels must be the offset in pixels from the origin
    of the drawing pixmap.
*/
{
    assert(no > 0);
    assert(nCols > 0);
    assert(nRows > 0);

    levelNo = no;

    sizeInTiles = Couple(nCols, nRows);
    sizeInPixels = sizeInTiles * TILE_SIDE;

    positionInPixels = posInPixels;

    delete [] xpmMatrix;
    xpmMatrix = new XPM[nRows * nCols];
    memset(xpmMatrix, 0, nRows * nCols * sizeof(XPM));

    delete [] tileMatrix;
    tileMatrix = new SDL_Surface *[nRows * nCols];
    memset(tileMatrix, 0, nRows * nCols * sizeof(SDL_Surface *));
}


void
BurgerSpaceEngine::Level::setLevelNo(int no)
{
    assert(no >= 1);
    levelNo = no;
}


int
BurgerSpaceEngine::Level::getLevelNo() const
{
    assert(levelNo >= 1);
    return levelNo;
}


void
BurgerSpaceEngine::Level::setTileMatrixEntry(
			int colNo, int rowNo, XPM xpm, SDL_Surface *pixmap)
{
    assert(colNo < sizeInTiles.x);
    assert(rowNo < sizeInTiles.y);
    assert(pixmap);

    xpmMatrix[rowNo * sizeInTiles.x + colNo] = xpm;
    tileMatrix[rowNo * sizeInTiles.x + colNo] = pixmap;
}


inline SDL_Surface **
BurgerSpaceEngine::Level::getTileMatrixRow(int rowNo)
{
    return &tileMatrix[rowNo * sizeInTiles.x];
}


BurgerSpaceEngine::XPM
BurgerSpaceEngine::Level::getXPMAtPixel(Couple pos) const
/*  Returns a pointer to the XPM variable that corresponds to
    the tile that contains the pixels at 'pos'.
    Returns NULL if 'pos' is in no tile.
*/
{
    pos -= positionInPixels;
    if (pos.x < 0 || pos.x >= sizeInPixels.x ||
	    pos.y < 0 || pos.y >= sizeInPixels.y)
	return NULL;

    pos /= TILE_SIDE;
    return xpmMatrix[pos.y * sizeInTiles.x + pos.x];
}


///////////////////////////////////////////////////////////////////////////////


BurgerSpaceEngine::BurgerSpaceEngine(const string &windowManagerCaption,
					int initLevelNumber,
					bool _useSound,
					bool fullScreen,
					bool useOldMotionMode,
					bool processActiveEvent,
					SDLKey pepperKey)
						throw(int, string)
  : GameEngine(Couple(672, 520), windowManagerCaption, fullScreen, processActiveEvent),
  		// may throw string exception

    initLevelNo(1),
    cumulLevelNo(1),
    paused(false),
    tickCount(0),
    tilePixmaps(),

    initPlayerPos(),
    playerPA(18),
    playerSprite(NULL),
    lastPlayerDirection(-1),
    oldMotionMode(useOldMotionMode),

    pepperPA(2),
    pepperSprites(),

    timeForNewEnemy(0),

    eggPA(14),
    hotdogPA(14),
    picklePA(14),
    enemySprites(),

    topBunPA(4),
    lettucePA(4),
    meatPA(4),
    redStuffPA(4),
    yellowStuffPA(4),
    bottomBunPA(4),
    ingredientSprites(),
    ingredientGroups(),

    treatPA(3),
    treatSprites(),
    timeForTreat(0),

    digitPA(10),
    scoreSprites(),

    startKS(SDLK_SPACE),
    quitKS(SDLK_ESCAPE),
    theChefController(pepperKey),
    pauseKS(SDLK_p),
    saveGameKS(SDLK_s),
    loadGameKS(SDLK_l),
    lastKeyPressed(SDLK_UNKNOWN),

    numHamburgersToDo(0),
    thePeakScore(0),

    theScore(0),

    celebrationMode(false),

    numLives(0),
    numAvailablePeppers(0),

    theCurrentLevel(),

    inQuitDialog(false),
    inSaveDialog(false),
    inLoadDialog(false),

    theSoundMixer(NULL),
    useSound(_useSound)
{
    assert(initLevelNumber >= 1);
    initLevelNo = initLevelNumber;
    cumulLevelNo = initLevelNo;

    try
    {
	try
	{
	    loadPixmaps();
	}
	catch (PixmapLoadError &e)
	{
	    string msg = __("Could not load pixmap ") + e.getFilename();
	    throw msg;
	}

	initializeMisc();

	loadLevel(initLevelNo);
	initializeSprites();
    }
    catch (string &msg)
    {
	displayErrorMessage(msg);
	throw -1;
    }
}


void
BurgerSpaceEngine::displayErrorMessage(const string &msg) throw()
{
    fprintf(stderr, "%s\n", msg.c_str());
}


BurgerSpaceEngine::~BurgerSpaceEngine()
{
    delete playerSprite;
}


void
BurgerSpaceEngine::initializeSprites() throw(PixmapLoadError)
/*  Initializes the sprites that appear at the beginning of a level,
    like the player and the ingredients.

    Assumes that 'playerSprite' is a valid pointer (may be null).

    Assumes that all pixmap arrays have been loaded.
*/
{
    /*	Player:
    */
    delete playerSprite;
    Couple playerSize = playerPA.getImageSize();
    int sx = playerStartingPos[theCurrentLevel.getLevelNo()].first;
    assert(sx != 0);
    int sy = playerStartingPos[theCurrentLevel.getLevelNo()].second;
    assert(sy != 0);
    initPlayerPos = theCurrentLevel.positionInPixels +
		Couple(sx * TILE_SIDE + 1, sy * TILE_SIDE - playerSize.y);
    playerSprite = new Sprite(playerPA, initPlayerPos,
				    Couple(0, 0), Couple(0, 0),
				    Couple(4, 4), playerSize - Couple(8, 8));


    /*	Ingredients:
    */
    deleteSprites(ingredientSprites);
    for_each(ingredientGroups.begin(), ingredientGroups.end(),
					    deleteIngredientGroup);
    ingredientGroups.clear();

    const Couple size = bottomBunPA.getImageSize();
    const IngInit *tableIngredients =
		tableOfTablesOfIngredientsLevel[theCurrentLevel.getLevelNo()];
    assert(tableIngredients != NULL);

    numHamburgersToDo = 0;

    for (size_t j = 0; tableIngredients[j].rank != 0; j++)
    {
	const IngInit &ii = tableIngredients[j];
	int yTarget = theCurrentLevel.positionInPixels.y +
				ii.yTargetTile * TILE_SIDE - size.y * ii.rank;
	PixmapArray *pm = NULL;
	switch (ii.type)
	{
	    case IngInit::BOTTOM_BUN:   pm = &bottomBunPA;   break;
	    case IngInit::MEAT:         pm = &meatPA;        break;
	    case IngInit::LETTUCE:      pm = &lettucePA;     break;
	    case IngInit::RED_STUFF:    pm = &redStuffPA;    break;
	    case IngInit::YELLOW_STUFF: pm = &yellowStuffPA; break;
	    case IngInit::TOP_BUN:  pm = &topBunPA; numHamburgersToDo++; break;
	    default:                   assert(false);
	}

	IngredientGroup *ig =
		    new IngredientGroup(yTarget, ii.type == IngInit::TOP_BUN);
	for (size_t i = 0; i < 4; i++)
	{
	    Couple pos = theCurrentLevel.positionInPixels +
				Couple(ii.xInitTile * TILE_SIDE + i * size.x,
					ii.yInitTile * TILE_SIDE - size.y + 6);
	    IngredientSprite *s = new IngredientSprite(*pm, pos, ig);
	    ig->setMember(i, s);
	    s->currentPixmapIndex = i;

	    ingredientSprites.push_back(s);
	}
	ingredientGroups.push_back(ig);
    }
}


void
BurgerSpaceEngine::initializeMisc() throw(string)
/*  Initializes things that need to be initialized once, but not at the
    beginning of each level.

    Throws an error message in a 'string' if an error occurs.
*/
{
    scoreAreaSize = Couple(160, 16);
    scoreAreaPos = Couple(4, 4);
    numAvailablePeppersAreaSize = Couple(80, 16);
    numAvailablePeppersAreaPos =
	    Couple(getScreenWidthInPixels() - numAvailablePeppersAreaSize.x, 4);
    numLivesAreaSize = Couple(160, 16);
    numLivesAreaPos =
	    Couple(4, getScreenHeightInPixels() - numLivesAreaSize.y - 4);
    levelNoAreaSize = Couple(66, 16);
    levelNoAreaPos =
	Couple(getScreenWidthInPixels() - levelNoAreaSize.x, numLivesAreaPos.y);

    initTimeForTreat();

    /*  Sound effects:
    */
    if (useSound)
    {
	try
	{
	    theSoundMixer = NULL;
	    theSoundMixer = new SoundMixer(16);  // may throw string
	}
	catch (const SoundMixer::Error &e)
	{
	    return;
	}

	string d = getDir(PKGSOUNDDIR, "PKGSOUNDDIR");

	try
	{
	    ingredientBouncesSound.init(d + "ingredient-bounces.wav");
	    ingredientInPlateSound.init(d + "ingredient-in-plate.wav");
	    ingredientFallsSound  .init(d + "ingredient-falls.wav");
	    ingredientLoweredSound.init(d + "ingredient-lowered.wav");
	    enemyCatchesChefSound .init(d + "enemy-catches-chef.wav");
	    enemyParalyzedSound   .init(d + "enemy-paralyzed.wav");
	    enemySmashedSound     .init(d + "enemy-smashed.wav");
	    chefThrowsPepperSound .init(d + "chef-throws-pepper.wav");
	    chefGetsTreatSound    .init(d + "chef-gets-treat.wav");
	    chefShootsBlanksSound .init(d + "chef-shoots-blanks.wav");
	    newGameStartsSound    .init(d + "new-game-starts.wav");
	    levelFinishedSound    .init(d + "level-finished.wav");
	    treatAppearsSound     .init(d + "treat-appears.wav");
	    treatDisappearsSound  .init(d + "treat-disappears.wav");
	}
	catch (const SoundMixer::Error &e)
	{
	    throw e.what();
	}
    }
}


void
BurgerSpaceEngine::showInstructions()
{
    displayStartMessage(true);
}


void
BurgerSpaceEngine::initGameParameters()
{
    loadLevel(initLevelNo);

    celebrationMode = false;
    theScore = 0;
    numAvailablePeppers = 5;
    numLives = 0;

    initTimeForTreat();
}


///////////////////////////////////////////////////////////////////////////////


/*virtual*/
void
BurgerSpaceEngine::processKey(SDLKey keysym, bool pressed)
{
    startKS.check(keysym, pressed);
    quitKS.check(keysym, pressed);
    theChefController.check(keysym, pressed);
    pauseKS.check(keysym, pressed);
    saveGameKS.check(keysym, pressed);
    loadGameKS.check(keysym, pressed);
    if (pressed)
	lastKeyPressed = keysym;
}


/*virtual*/
void
BurgerSpaceEngine::processActivation(bool appActive)
{
    // If the app is now inactive, put the game in paused mode.
    // If the app is now active, stay in paused mode so the user sees
    // the resume prompt and can choose when to resume playing.
    //
    if (!appActive)
    {
	paused = true;
	displayPauseMessage(true);
    }
}


/*virtual*/
bool
BurgerSpaceEngine::tick()
{
    if (quitKS.justPressed())
    {
	drawQuitDialog();
	inQuitDialog = true;
    }

    if (saveGameKS.justPressed())
    {
	drawSaveDialog();
	inSaveDialog = true;
    }

    if (loadGameKS.justPressed())
    {
	drawLoadDialog();
	inLoadDialog = true;
    }

    if (inQuitDialog)
    {
	if (!doQuitDialog())
	    return false;
    }
    else if (inSaveDialog)
    {
	doSaveDialog();
    }
    else if (inLoadDialog)
    {
	doLoadDialog();
    }
    else if (paused)
    {
	displayPauseMessage(true);
	if (pauseKS.justPressed())
	    paused = false;
    }
    else
    {
	tickCount++;

	if (!animatePlayer())
	    return false;

	if (paused)
	    displayPauseMessage(true);
	else
	{
	    animateAutomaticCharacters();
	    restoreBackground();
	    drawSprites();
	    detectCollisions();
	}
    }

    startKS.remember();
    theChefController.remember();
    pauseKS.remember();
    saveGameKS.remember();

    return true;
}


Couple
BurgerSpaceEngine::getDistanceToPerfectPos(const Sprite &s) const
/*  DEFINITION: a "perfect position" for a sprite is a position where
    the sprite's center is horizontally at the center of a tile,
    and where the bottom of the sprite is flush with the bottom of a tile.

    In this game, this means _for example_ that a sprite is (horizontally)
    centered on a ladder and (vertically) resting on a floor.
*/
{
    int dx = TILE_SIDE / 2 - s.getCenterPos().x % TILE_SIDE;
    int dy = s.getLowerRightPos().y % TILE_SIDE;
    dy = - (dy >= TILE_SIDE / 2 ? dy - TILE_SIDE : dy);
    return Couple(dx, dy);
}


void
BurgerSpaceEngine::putSprite(const Sprite &s)
{
    copySpritePixmap(s, s.currentPixmapIndex, s.getPos());
}


void
BurgerSpaceEngine::initNextLevel(int levelNo /*= 0*/) throw(int)
/*  Initialize the next level.  Increments the current level number if
    levelNo is zero, or uses levelNo is it is positive.
*/
{
    assert(levelNo >= 0);

    celebrationMode = false;

    try
    {   
	if (levelNo == 0)
	    cumulLevelNo++;
	else
	    cumulLevelNo = levelNo;
	loadLevel(cumulLevelNo);
	initializeSprites();
	initTimeForTreat();
    }
    catch (string &msg)
    {
	displayErrorMessage(msg);
	throw -1;
    }
}


void
BurgerSpaceEngine::startNewLife()
{
    deleteSprites(pepperSprites);
    playerSprite->setPos(initPlayerPos);

    deleteSprites(enemySprites);

    timeForNewEnemy = 0;
}


static const struct
{
    unsigned char num;      // number of alternatives
    unsigned char alts[4];  // alternatives for the key
}
directionTable[16] =
{
    {  0, {  0,  0,  0,  0 } },    //  0
    {  1, {  1,  0,  0,  0 } },    //  1
    {  1, {  2,  0,  0,  0 } },    //  2
    {  2, {  1,  2,  0,  0 } },    //  3
    {  1, {  4,  0,  0,  0 } },    //  4
    {  2, {  4,  1,  0,  0 } },    //  5
    {  2, {  4,  2,  0,  0 } },    //  6
    {  3, {  4,  2,  1,  0 } },    //  7
    {  1, {  8,  0,  0,  0 } },    //  8
    {  2, {  8,  1,  0,  0 } },    //  9
    {  2, {  8,  2,  0,  0 } },    // 10
    {  3, {  8,  2,  1,  0 } },    // 11
    {  2, {  8,  4,  0,  0 } },    // 12
    {  3, {  8,  4,  1,  0 } },    // 13
    {  3, {  8,  4,  2,  0 } },    // 14
    {  4, {  8,  4,  2,  1 } },    // 15
};



void
BurgerSpaceEngine::chooseDirectionAmongMany(bool directions[4]) const
/*  On input, the elements of directions[] must be set.  There may be
    zero, one or more true elements.
    If there are no true elements, this method does nothing.
    Otherwise, it chooses one of the true elements; that is, directions[]
    will only have one true element left upon return.
    The choice is random.
*/
{
    int key =
	    ( (directions[RIGHT] << RIGHT)
	    | (directions[UP]    << UP)
	    | (directions[LEFT]  << LEFT)
	    | (directions[DOWN]  << DOWN)
	    );
    if (key == 0)
	return;
    unsigned char num = directionTable[key].num;
    unsigned index = unsigned(rand()) % num;
    unsigned char choice = directionTable[key].alts[index];

    directions[RIGHT] = ((choice & (1 << RIGHT)) != 0);
    directions[UP]    = ((choice & (1 << UP))    != 0);
    directions[LEFT]  = ((choice & (1 << LEFT))  != 0);
    directions[DOWN]  = ((choice & (1 << DOWN))  != 0);
}


int
BurgerSpaceEngine::chooseDirectionTowardTarget(
				    Couple startPos,
				    Couple targetPos,
				    int speedFactor,
				    const bool allowedDirections[4]) const
/*  Determines the direction that should lead to the position of 'target'
    from the starting position 'startPos'.
    'speedFactor' must be the speed of the sprite that is at the
    starting position.
    'allowedDirections' must indicate which directions are possible.

    Returns RIGHT, UP, LEFT, DOWN, or -1 if no decision was possible.
*/
{
    int dir = -1;  // should contain RIGHT, UP, LEFT or DOWN

    // Choose "preferred" directions depending on the target's position:
    const Couple toTarget = targetPos - startPos;
    int prefHorizDir = (toTarget.x >= speedFactor ? RIGHT :
			(toTarget.x <= -speedFactor ? LEFT : -1));
    int prefVertDir  = (toTarget.y >= speedFactor ? DOWN :
			(toTarget.y <= -speedFactor ? UP : -1));
    if (prefHorizDir != -1 && !allowedDirections[prefHorizDir])
	prefHorizDir =  -1;
    if (prefVertDir  != -1 && !allowedDirections[prefVertDir])
	prefVertDir  =  -1;

    if (prefHorizDir != -1 && prefVertDir != -1)
	dir = (rand() % 2 ? prefHorizDir : prefVertDir);
    else if (prefHorizDir != -1)
	dir = prefHorizDir;
    else if (prefVertDir != -1)
	dir = prefVertDir;

    return dir;
}


void
BurgerSpaceEngine::avoidEnemies(bool desiredDirs[4]) const
/*  Tries to avoids directions that probably lead to a collision
    with an enemy.
*/
{
    Couple playerPos = playerSprite->getPos();

    SpriteList::const_iterator it;
    for (it = enemySprites.begin(); it != enemySprites.end(); it++)
    {
	const Sprite *enemy = *it;

	// Consider only enemies that are "close" to the player:
	Couple enemyPos = enemy->getPos();
	Couple toPlayer = playerPos - enemyPos;
	if (abs(toPlayer.x) > 2 * TILE_SIDE || abs(toPlayer.y) > 2 * TILE_SIDE)
	    continue;


	// What is the direction from this enemy to the player?
	int dir = chooseDirectionTowardTarget(enemyPos, playerPos,
					    PLAYER_SPEED_FACTOR, desiredDirs);
	
	if (dir == -1)  // if no decision, never mind
	    continue;
	
	/*  The player must avoid the direction opposite to 'dir'.
	    For example, if the enemy would have to go up to get to the
	    player, then the player must not go down.
	*/
	desiredDirs[dir ^ 2] = false;
    }
}


void
BurgerSpaceEngine::getPlayerDesiredDirections(bool desiredDirs[4]) const
/*  Determines the directions in which the player wants to go.
    Stores values in 'desiredDirs' at the RIGHT, UP, LEFT, DOWN indices.
    This method isolates the way to determine this.  That will eventually
    allow us to have the chef be controlled by the computer.
*/
{
    theChefController.getDesiredDirections(desiredDirs);
}


bool
BurgerSpaceEngine::animatePlayer()
/*  Returns true if the game must continue, or false to have it stop.
*/
{
    if (pauseKS.justPressed())
    {
	paused = true;
	return true;
    }


    /*  If player is agonizing/winning:
    */
    if (playerSprite->getTimeToLive() != 0)
    {
	unsigned long ttl = playerSprite->getTimeToLive();
	/*  The "time to live" is used as a decremented counter that
	    indicates where we are in the animation...
	*/
	int playerSpriteNo = -1;
	if (celebrationMode)
	{
	    int t = (tickCount >> 2) & 1;
	    playerSpriteNo = 12 * t;  // front0 or dead0
	}
	else  // agony:
	{
	    if (ttl > 52)
		playerSpriteNo = 12;
	    else if (ttl > 48)
		playerSpriteNo = 13;
	    else if (ttl > 44)
		playerSpriteNo = 14;
	    else if (ttl > 40)
		playerSpriteNo = 15;
	    else
	    {
		int t = (tickCount >> 2) & 1;
		playerSpriteNo = 16 + t;
	    }
	}

	assert(playerSpriteNo != -1);
	playerSprite->currentPixmapIndex = playerSpriteNo;
	    
	if (playerSprite->decTimeToLive() == 0)
	{
	    startNewLife();

	    if (celebrationMode)  // if player just finished the level
	    {
		initNextLevel();  // must come after call to startNewLife()
	    }
	    else  // player has finished dying
	    {
		addToNumLives(-1);
		assert(numLives >= 0);
	    }
	}

	return true;
    }


    if (numLives == 0)
    {
	/*  In the demo mode, the Space bar starts a new game.
	*/
	if (startKS.justPressed())
	{
	    playSoundEffect(newGameStartsSound);

	    initGameParameters();
	    startNewLife();
	    initNextLevel(initLevelNo);

	    theScore = 0;
	    thePeakScore = 0;
	    numLives = NUM_INIT_LIVES;

	    addToScore(0);
	    addToNumLives(0);
	}
	return true;
    }


    Couple &playerPos = playerSprite->getPos();
    Couple &playerSpeed = playerSprite->getSpeed();


    /*  Shoot if requested:
    */
    static const bool infinitePepper = (getenv("INFINITEPEPPER") != NULL);
    if (theChefController.isShotRequested())
    {
	if (infinitePepper || numAvailablePeppers > 0)
	{
	    const Couple size = pepperPA.getImageSize();
	    const Couple plsize = playerSprite->getSize();
	    Couple pos = playerSprite->getPos();
	    int dir;

	    if (lastPlayerDirection == -1)
		dir = UP;
	    else
		dir = lastPlayerDirection;
	    numAvailablePeppers--;

	    switch (dir)
	    {
		case UP   :  pos += Couple(0, -size.y);  break;
		case DOWN :  pos += Couple(0, plsize.y); break;
		case LEFT :  pos += Couple(-size.x, 0);  break;
		case RIGHT:  pos += Couple(plsize.x, 0); break;
		default:     assert(false);
	    }

	    playSoundEffect(chefThrowsPepperSound);

	    Sprite *s = new Sprite(pepperPA, pos, Couple(0, 0),
				    Couple(0, 0), Couple(0, 0), size);
	    s->setTimeToLive(12);
	    pepperSprites.push_back(s);
	}
	else
	{
	    playSoundEffect(chefShootsBlanksSound);
	}
    }


    /*  Attempt a move:
    */
    bool desiredDirs[4];
    getPlayerDesiredDirections(desiredDirs);
    playerSpeed = attemptMove(*playerSprite,
				desiredDirs[LEFT], desiredDirs[RIGHT],
				desiredDirs[UP], desiredDirs[DOWN],
				PLAYER_SPEED_FACTOR);

    playerSprite->currentPixmapIndex = 0;
    if (playerSpeed.isZero())
    {
	if (!oldMotionMode
		&& (desiredDirs[RIGHT] || desiredDirs[UP]
		    || desiredDirs[LEFT] || desiredDirs[DOWN]))
	{
	    /*
		If the user is pressing at least one arrow, then try to
		reuse the last non zero direction.
		This can be is useful to turn corners automatically.
	    */
	    bool backupDirs[4] = { false, false, false, false };
	    backupDirs[lastPlayerDirection] = true;
	    playerSpeed = attemptMove(*playerSprite,
				    backupDirs[LEFT], backupDirs[RIGHT],
				    backupDirs[UP], backupDirs[DOWN],
				    PLAYER_SPEED_FACTOR);
	    if (playerSpeed.isZero())
		return true;
	}
	else
	    return true;
    }


    /*  Determine the sprite direction and image according to 
        the player's speed:
    */
    int dir = -1;
    if (playerSpeed.x == 0)  // if vertical movement
    {
	if (playerSpeed.y > 0)  // if downwards
	{
	    playerSprite->currentPixmapIndex = (tickCount & 2 ? 1 : 2);
	    dir = DOWN;
	}
	else
	{
	    playerSprite->currentPixmapIndex = (tickCount & 2 ? 4 : 5);
	    dir = UP;
	}
    }
    else  // horizontal movement
    {
	static const char table[4] = { 0, 1, 0, 2 };

	int t = (tickCount >> 1) & 3;
	t = table[t];
	if (playerSpeed.x < 0)  // if left
	{
	    playerSprite->currentPixmapIndex = 6 + t;
	    dir = LEFT;
	}
	else
	{
	    playerSprite->currentPixmapIndex = 9 + t;
	    dir = RIGHT;
	}
    }

    assert(dir != -1);
    assert(playerSprite->currentPixmapIndex < playerPA.getNumImages());


    /*	Apply the speed to the position:
    */
    assert(playerSpeed.isNonZero());
    playerPos += playerSpeed;
    lastPlayerDirection = dir;

    return true;
}


Couple
BurgerSpaceEngine::determineAllowedDirections(const Sprite &s,
					    int speedFactor, int tolerance,
					    bool allowedDirections[4]) const
/*  Determines in what directions a move from the described parameters
    would be allowed.
    Stores boolean values in allowedDirections[], indexed by the integer
    constants RIGHT, UP, LEFT and DOWN.

    'speedFactor' must be the length of the move to try.
    'tolerance' must be the number of pixels of distance between the
    sprite and the perfect position to take a ladder when on a floor,
    or to take a floor when on a ladder.

    Returns the distance between the sprite's position and a "perfect"
    position, as computed by the getDistanceToPerfectPos() method.
*/
{
    Couple pos = s.getPos();
    Couple size = s.getSize();

    Couple delta = getDistanceToPerfectPos(s);
    bool xOK = (abs(delta.x) <= tolerance);
    bool yOK = (abs(delta.y) <= tolerance);

    Couple newPos = pos + Couple(-speedFactor, delta.y);
    allowedDirections[LEFT] = yOK && positionAllowsLeftMovement(newPos, size);

    newPos = pos + Couple(+speedFactor, delta.y);
    allowedDirections[RIGHT] = yOK && positionAllowsRightMovement(newPos, size);

    allowedDirections[UP]   = xOK && spriteBottomCenterIsOnLadder(s);
    allowedDirections[DOWN] = xOK && spriteBottomCenterIsOverLadder(s);

    return delta;
}


Couple
BurgerSpaceEngine::attemptMove(const Sprite &s,
				bool attemptLeft, bool attemptRight,
				bool attemptUp, bool attemptDown,
				int speedFactor) const
/*  Attempts a move described by the parameters.
    Returns a non-zero speed if the attempt succeeds.
    Returns a zero speed if no direction is allowed.

    's' must be the sprite that attempts to move from its current position.
    The four 'attempt*' boolean parameters indicate which directions
    are to be attempted.
    'speedFactor' must be the length of the move to try.
*/
{
    Couple speed(0, 0);


    bool allowedDirections[4];
    Couple delta = determineAllowedDirections(
		    s, speedFactor, CORNER_TURN_TOLERANCE, allowedDirections);

    if (attemptLeft && allowedDirections[LEFT])
    {
	speed.x = -speedFactor;
	speed.y = delta.y;
	return speed;
    }
    if (attemptRight && allowedDirections[RIGHT])
    {
	speed.x = +speedFactor;
	speed.y = delta.y;
	return speed;
    }
    if (attemptUp && allowedDirections[UP])
    {
	speed.y = -speedFactor;
	speed.x = delta.x;
	return speed;
    }
    if (attemptDown && allowedDirections[DOWN])
    {
	speed.y = +speedFactor;
	speed.x = delta.x;
	return speed;
    }

    return speed;
}


bool
BurgerSpaceEngine::positionAllowsLeftMovement(Couple pos, Couple size) const
/*  Determines if the proposed position 'pos' for a sprite that would
    have the given 'size' would allow movement to the left.
    Looks at the tiles at the given position, which must be floor tiles
    or floor-and-ladder tiles.
*/
{
    Couple posBotLeft = pos + Couple(0, size.y);
    if (posBotLeft.y % TILE_SIDE != 0)
	return false;

    XPM botLeftXPM = theCurrentLevel.getXPMAtPixel(posBotLeft);
    return (botLeftXPM == floor_xpm || botLeftXPM == floor_and_ladder_xpm);
}


bool
BurgerSpaceEngine::positionAllowsRightMovement(Couple pos, Couple size) const
{
    Couple posBotRight = pos + size + Couple(-1, 0);
    if (posBotRight.y % TILE_SIDE != 0)
	return false;

    XPM botRightXPM = theCurrentLevel.getXPMAtPixel(posBotRight);
    return (botRightXPM == floor_xpm || botRightXPM == floor_and_ladder_xpm);
}


bool
BurgerSpaceEngine::spriteBottomCenterIsOnLadder(const Sprite &s) const
/*  Determines if the sprite's bottom center touches a ladder.
    This method does NOT judge whether a sprite's horizontal position
    is close enough to a ladder.
    This method can be used to determine if a sprite can climb up a ladder.
*/
{
    const Couple &centerPos = s.getCenterPos();
    const Couple &lowerRightPos = s.getLowerRightPos();
    XPM botXPM = theCurrentLevel.getXPMAtPixel(
				     Couple(centerPos.x, lowerRightPos.y - 1));
    return (botXPM == ladder_xpm || botXPM == floor_and_ladder_xpm);
}


bool
BurgerSpaceEngine::spriteBottomCenterIsOverLadder(const Sprite &s) const
/*  Determines if the pixel UNDER the sprite's bottom center touches a ladder.
    This method does NOT judge whether a sprite's horizontal position
    is close enough to a ladder.
    This method can be used to determine if a sprite can climb down a ladder.
*/
{
    const Couple &centerPos = s.getCenterPos();
    const Couple &lowerRightPos = s.getLowerRightPos();
    XPM botXPM = theCurrentLevel.getXPMAtPixel(
				     Couple(centerPos.x, lowerRightPos.y));
    return (botXPM == ladder_xpm || botXPM == floor_and_ladder_xpm);
}


void
BurgerSpaceEngine::killSpritesInList(SpriteList &sl)
{
    for (SpriteList::iterator its = sl.begin(); its != sl.end(); its++)
    {
	Sprite *s = *its;
	assert(s != NULL);
	delete s;
    }

    sl.clear();
    assert(sl.size() == 0);
}


void
BurgerSpaceEngine::animateAutomaticCharacters()
{
    /*  Animate score sprites:
    */
    animateTemporarySprites(scoreSprites);


    /*  Everything freezes else if the player is agonizing:
    */
    if (playerSprite->getTimeToLive() != 0)
	return;


    /*  If there is not the minimum number of enemies in the play,
	schedule the creation of another one some time in the near future.
    */
    size_t totalNumEnemies = enemySprites.size();
    const size_t requiredNumEnemies = 6 + 3 * ((cumulLevelNo - 1) / NUM_LEVELS);
    if (totalNumEnemies < requiredNumEnemies && timeForNewEnemy == 0)
	timeForNewEnemy = tickCount + 40;

    static bool noEnemiesFlag = (getenv("NOENEMIES") != NULL);
    if (noEnemiesFlag)
	timeForNewEnemy = 0;

    if (tickCount >= timeForNewEnemy && timeForNewEnemy != 0)
    {
	timeForNewEnemy = 0;

	assert(theCurrentLevel.getLevelNo() >= 1);
	const IntQuad &sh = enemyStartingHeights[theCurrentLevel.getLevelNo()];
	assert(sh.first != 0 && sh.second != 0 &&
		    sh.third != 0 && sh.fourth != 0);

	bool fromLeft = (rand() % 2 != 0);
	int x = (fromLeft ? 0 : theCurrentLevel.sizeInTiles.x - 1);
	int y;
	if (fromLeft)
	    y = (rand() % 2 ? sh.first : sh.second);
	else
	    y = (rand() % 2 ? sh.third : sh.fourth);
	Couple posInTiles(x, y);

	Couple size = eggPA.getImageSize();  // we assume all enemies same size
	Couple pos = theCurrentLevel.positionInPixels +
			TILE_SIDE * posInTiles - Couple(0, size.y);
	Couple speed(ENEMY_SPEED_FACTOR, 0);

	static int typeCounter = 0;
	int type = typeCounter;
	typeCounter = (typeCounter + 1) % 3;

	switch (type)
	{
	    case 0:  // egg
	    {
		Sprite *s = new EnemySprite(eggPA, pos, speed,
					    Couple(3, 3), size - Couple(6, 6));
		enemySprites.push_back(s);
	    }
	    break;

	    case 1:  // hotdog
	    {
		Sprite *s = new EnemySprite(hotdogPA, pos, speed,
					    Couple(3, 3), size - Couple(6, 6));
		enemySprites.push_back(s);
	    }
	    break;

	    case 2:  // pickle
	    {
		Sprite *s = new EnemySprite(picklePA, pos, speed,
					    Couple(3, 3), size - Couple(6, 6));
		enemySprites.push_back(s);
	    }
	    break;

	    default:
		assert(false);
	}
    }


    moveEnemyList(enemySprites, ENEMY_SPEED_FACTOR);


    /*  Pepper:
    */
    SpriteList::iterator it;
    for (it = pepperSprites.begin(); it != pepperSprites.end(); it++)
	if ((*it)->decTimeToLive() == 0)
	{
	    delete *it;
	    *it = NULL;
	}
    removeNullElementsFromSpriteList(pepperSprites);


    /*  Ingredients:
    */
    for (IngredientSprite::List::iterator iti = ingredientSprites.begin();
				    iti != ingredientSprites.end(); iti++)
	(*iti)->addSpeedToPos();


    /*  Treats:
    */
    for (it = treatSprites.begin(); it != treatSprites.end(); it++)
	if ((*it)->decTimeToLive() == 0)
	{
	    delete *it;
	    *it = NULL;

	    if (numLives > 0)
		playSoundEffect(treatDisappearsSound);
	}
    removeNullElementsFromSpriteList(treatSprites);

    if (timeForTreat == 0)
    {
	initTimeForTreat();
	Couple size = treatPA.getImageSize();
	Couple pos = theCurrentLevel.positionInPixels +
		Couple(11 * TILE_SIDE + 1, 5 * TILE_SIDE - size.y);

	Couple collBoxPos(0, 0);
	Couple collBoxSize = size;

	int spriteNo = rand() % 3;

	if (spriteNo == 0)  // ice cream is thinner and centered
	    collBoxPos.x += 4, collBoxSize.x -= 8;
	else if (spriteNo == 1)  // coffee cup: exclude handle at the right
	    collBoxPos.y++, collBoxSize -= Couple(4, 2);

	Sprite *s = new Sprite(treatPA,
				pos, Couple(0, 0), Couple(0, 0),
				collBoxPos, collBoxSize);
	s->currentPixmapIndex = spriteNo;
	s->setTimeToLive(5 * 20);
	treatSprites.push_back(s);

	if (numLives > 0)
	    playSoundEffect(treatAppearsSound);
    }
    else
	timeForTreat--;
}


IngredientGroup *
BurgerSpaceEngine::findIngredientGroupRightBelow(
				    const IngredientGroup &upperGroup)
/*  
    Searches in 'ingredientGroups' to see if there is an ingredient whose
    left-most sprite is just below (i.e., touching) the left-most sprite
    of 'upperGroup'.

    Returns NULL if no such ingredient group is found.
*/
{
    Couple targetPos = upperGroup.getMember(0)->getPos();
    targetPos.y += upperGroup.getMember(0)->getSize().y;

    for (IngredientGroup::List::iterator it = ingredientGroups.begin();
					    it != ingredientGroups.end(); it++)
    {
	IngredientGroup *lowerGroup = *it;
	assert(lowerGroup != NULL);

	const IngredientSprite *lowerGroupMember0 = lowerGroup->getMember(0);
	Couple posLowerGroupMember0 = lowerGroupMember0->getPos();
	if (lowerGroupMember0->isLowered())
	    posLowerGroupMember0.y -= IngredientSprite::LOWERING_DISTANCE;

	/*  This comparison may not always work if INGREDIENT_FALL_SPEED
	    ever becomes a number that is not a factor of TILE_SIDE...
	*/
	if (posLowerGroupMember0 == targetPos)
	    return lowerGroup;
    }

    return NULL;
}


bool
BurgerSpaceEngine::isIngredientSpriteOnFloor(const IngredientSprite &s) const
{
    Couple size = s.getSize();
    Couple pos = s.getPos();
    Couple p(pos.x, pos.y + size.y - 6);
	    /*  The -6 represents the fact that an ingredient is supposed
		to be a bit sunken in the floor.
	    */
    if (p.y % TILE_SIDE != 0)
	return false;
    XPM xpm = theCurrentLevel.getXPMAtPixel(p);
    bool onFloor = (xpm == floor_xpm || xpm == floor_and_ladder_xpm);
    return onFloor;
}


size_t
BurgerSpaceEngine::carryEnemies(IngredientGroup &g)
/*  Carries enemies that touch 'g'.  See carryEnemiesInList().
    Returns the number of sprites that are carried.
*/
{
    return carryEnemiesInList(g, enemySprites);
}


size_t
BurgerSpaceEngine::carryEnemiesInList(
				IngredientGroup &g, SpriteList &slist)
/*  All sprites in 'slist' that are on a floor, and touch the ingredient
    represented by 'g', and are visible, are carried.
    Returns the number of sprites in 'slist' that are carried.
*/
{
    size_t numCarriedEnemies = 0;

    const Couple groupPos = g.getMember(0)->getPos();
    const Couple groupLRP = g.getMember(3)->getLowerRightPos();
    const Couple groupSize = groupLRP - groupPos;

    for (SpriteList::iterator it = slist.begin(); it != slist.end(); it++)
    {
	EnemySprite *enemy = dynamic_cast<EnemySprite *>(*it);
	assert(enemy != NULL);

	// Enemies that are not on a floor are not carried:
	Couple lrp = enemy->getLowerRightPos();
	if (lrp.y % TILE_SIDE != 0)
	    continue;
	if (lrp.y / TILE_SIDE != groupLRP.y / TILE_SIDE)
	    continue;
	XPM xpm = theCurrentLevel.getXPMAtPixel(lrp);
	if (xpm != floor_xpm && xpm != floor_and_ladder_xpm)
	    continue;

	if (enemy->getDisappearanceTime() != 0)
	    continue;
	
	if (ingredientGroupCollidesWithSprite(groupPos, groupSize, *enemy))
	{
	    g.addCarriedEnemy(enemy);

	    // Send the enemy downwards:
	    enemy->setSpeed(Couple(0, INGREDIENT_FALL_SPEED));

	    numCarriedEnemies++;
	}
    }

    return numCarriedEnemies;
}


size_t
BurgerSpaceEngine::releaseCarriedEnemies(IngredientGroup &g)
/*  Returns the number of released enemies.
*/
{
    size_t numReleasedEnemies = 0;

    SpriteList &slist = g.getCarriedEnemies();
    numReleasedEnemies = slist.size();
    for (SpriteList::iterator it = slist.begin(); it != slist.end(); it++)
    {
	assert(*it != NULL);
	EnemySprite *enemy = dynamic_cast<EnemySprite *>(*it);
	assert(enemy != NULL);
	enemy->setCarryingGroup(NULL);
	enemy->setDisappearanceTime(60 + rand() % 10);
	enemy->setTimeToLive(0);  // eliminate effects of pepper, if applicable
	enemy->setSpeed(Couple(0, -ENEMY_SPEED_FACTOR));
    }
    g.clearCarriedEnemies();

    if (numReleasedEnemies > 0)
    {
	long score;
	switch (numReleasedEnemies)
	{
	    case 1:   score =  500; break;
	    case 2:   score = 1000; break;
	    default:  score = 2000;
	}

	createScoreSprites(score, g.getCenterPos());
    }

    return numReleasedEnemies;
}


void
BurgerSpaceEngine::moveEnemyList(SpriteList &slist, int speedFactor)
/*  Moves the enemies in 'slist' with the given speed factor.
    If an enemy that was dying finishes dying, some points are given
    to the player.
*/
{
    const Couple plpos = playerSprite->getPos();

    for (SpriteList::iterator it = slist.begin(); it != slist.end(); it++)
    {
	EnemySprite *s = dynamic_cast<EnemySprite *>(*it);
	assert(s != NULL);
	if (s->decDisappearanceTime() != 0)
	    continue;

	// If the enemy is being carried by a falling ingredient:
	if (s->getCarryingGroup() != NULL)
	{
	    s->addSpeedToPos();
	    continue;
	}

	// If enemy is dying:
	if (s->getTimeToDie() != 0)
	{
	    if (s->decTimeToDie() != 0)
		continue;

	    long score = 0;
	    const PixmapArray *pa = s->getPixmapArray();
	    if (pa == &eggPA)
		score = 300;
	    else if (pa == &picklePA)
		score = 200;
	    else if (pa == &hotdogPA)
		score = 100;
	    else
		assert(false);

	    // Enemy has finished dying:
	    createScoreSprites(score, s->getCenterPos());
	    delete s;
	    *it = NULL;
	    continue;
	}

	if (s->decTimeToLive() != 0)  // if enemy is paralyzed
	    continue;


	/*  Determine the directions that are allowed.
	*/
	bool allowedDirections[4];
	Couple delta = determineAllowedDirections(
			*s, speedFactor, speedFactor - 1, allowedDirections);


	/*  If the current direction is allowed, then disallow the
	    opposite direction.
	*/
	int currentDir = getDirectionFromCouple(s->getSpeed());
	if (allowedDirections[currentDir])
	    allowedDirections[currentDir ^ 2] = false;


	/*  Enemy sprites are created at a position to the left or right
	    of the floor and ladder structure, so we have to make an exception
	    to allow the left or right directions so that they can join
	    the structure.
	*/
	Couple pos = s->getPos() - theCurrentLevel.positionInPixels;
	if (pos.x / TILE_SIDE < 3)
	    allowedDirections[RIGHT] = true;
	else if (pos.x / TILE_SIDE >= theCurrentLevel.sizeInTiles.x - 3)
	    allowedDirections[LEFT] = true;


	/*  Count the number of allowed directions.  If none are allowed,
	    we assume that the enemy is below the structure because it has
	    just been carried down into the plate.  Thus, we allow it to go up.
	*/
	{
	    size_t numDirectionsAllowed = 0;
	    for (size_t j = 0; j < 4; j++)
		if (allowedDirections[j])
		    numDirectionsAllowed++;
	    if (numDirectionsAllowed == 0)
		allowedDirections[UP]++;
	}


	const Couple spos = s->getPos();

	int dir = -1;

	/*  To make the enemies less predictable, they will choose their
	    next direction at random once in a while, instead of always
	    looking at where the player is.
	    Also, the enemies move randomly if the game is in demo mode.
	*/
	int stupidityFactor = rand() % 4;
	if (stupidityFactor != 0 && numLives != 0)  // if smart mode:
	{
	    dir = chooseDirectionTowardTarget(spos, plpos,
	    				speedFactor, allowedDirections);
	}


	if (dir == -1)  // if still undecided:
	{
	    // Find a random true element in allowedDirections[]:
	    dir = rand() % 4;
	    while (!allowedDirections[dir])
		dir = (dir + 1) % 4;
	}

	// Add a correction to get to a "perfect" position:
	if (dir == RIGHT || dir == LEFT)
	    delta.x = 0;
	else if (dir == UP || dir == DOWN)
	    delta.y = 0;
	s->getPos() += delta;

	// Convert the direction (RIGHT/UP/LEFT/DOWN) into a speed couple:
	Couple speed = getCoupleFromDirection(dir, speedFactor);
	s->setSpeed(speed);
	s->addSpeedToPos();
    }

    removeNullElementsFromSpriteList(slist);
}


void
BurgerSpaceEngine::animateTemporarySprites(SpriteList &slist) const
/*  'slist' must be a list of sprites that die when their "time to live"
    expires.  This method removes sprites from 'slist' when they die.
    Sprites that live are advanced by adding their speed to their position.
*/
{
    for (SpriteList::iterator it = slist.begin(); it != slist.end(); it++)
    {
        Sprite *s = *it;
        assert(s != NULL);
        if (s->getTimeToLive() == 0)
        {
            delete s;
            *it = NULL;  // mark list element for deletion
        }
        else
        {
            s->decTimeToLive();
            s->addSpeedToPos();
        }
    }

    removeNullElementsFromSpriteList(slist);
}


void
BurgerSpaceEngine::givePlayerPepper()
{
    numAvailablePeppers++;
}


void
BurgerSpaceEngine::makePlayerWin()
{
    playSoundEffect(levelFinishedSound);

    playerSprite->setTimeToLive(80);
    releaseAllCarriedEnemies();

    celebrationMode = true;

    givePlayerPepper();
}


void
BurgerSpaceEngine::makePlayerDie()
{
    playerSprite->setTimeToLive(60);
	// The player will agonize for this number of ticks.

    releaseAllCarriedEnemies();
}


void
BurgerSpaceEngine::releaseAllCarriedEnemies()
{
    /*  Tell all ingredient groups to release all carried enemies.
	This must be done before we destroy the enemy sprites.
    */
    for (IngredientGroup::List::iterator it = ingredientGroups.begin();
    					it != ingredientGroups.end(); it++)
    {
	assert(*it != NULL);
	releaseCarriedEnemies(**it);
    }
}


void
BurgerSpaceEngine::detectCollisions()
{
    if (playerSprite->getTimeToLive() != 0)  // if player is agonizing
	return;

    bool playIngredientLoweredSound = false;

    assert(ingredientGroups.size() > 0);

    for (IngredientGroup::List::iterator it = ingredientGroups.begin();
					it != ingredientGroups.end(); it++)
    {
	IngredientGroup *aGroup = *it;
	assert(aGroup != NULL);
	IngredientSprite *member0 = aGroup->getMember(0);
	assert(member0 != NULL);


	/*  If this group is not moving, check if the player is walking
	    over some of its members.  If so, lower those members.
	    If all members of this group are lowered, make the group
	    start falling.
	*/
	if (aGroup->getState() == IngredientGroup::NORMAL)
	{
	    assert(aGroup->getNumMembers() > 0);

	    for (size_t j = 0; j < aGroup->getNumMembers(); j++)
	    {
		IngredientSprite *ing = aGroup->getMember(j);
		if (!ing->isLowered() && playerSprite->collidesWithSprite(*ing))
		{
		    playIngredientLoweredSound = true;

		    ing->lower();
		    if (aGroup->areAllMembersLowered())
		    {
			size_t numCarriedEnemies = carryEnemies(*aGroup);
			size_t numFloorsToGo;
			switch (numCarriedEnemies)
			{
			    case 0:   numFloorsToGo = 1; break;
			    case 1:   numFloorsToGo = 3; break;
			    default:  numFloorsToGo = 5;
			}

			playSoundEffect(ingredientFallsSound);
			aGroup->startFalling(
					INGREDIENT_FALL_SPEED, numFloorsToGo);
		    }
		}
	    }

	    continue;
	}


	/*  Check if the ingredient has reached its vertical target:
	    stop this ingredient.
	*/
	Couple &pos = member0->getPos();
	int yTarget = aGroup->getVerticalTarget();
	if (pos.y >= yTarget)
	{
	    playSoundEffect(ingredientInPlateSound);
	    pos.y = yTarget;
	    aGroup->stop();
	    releaseCarriedEnemies(*aGroup);
	    addToScore(50);
	    if (aGroup->isTopBun() && --numHamburgersToDo == 0)
		makePlayerWin();

	    continue;
	}


	/*  If the ingredient is bouncing up:
	    see if the bounce is over; if so, make the group stall.
	*/
	if (aGroup->getState() == IngredientGroup::BOUNCE)
	{
	    if (aGroup->decBounceTime() == 0)
		aGroup->stall();
	    continue;
	}

	/*  If the ingredient is stalling:
	    see if the stall is over; if so, start the second fall.
	*/
	if (aGroup->getState() == IngredientGroup::STALL)
	{
	    if (aGroup->decStallTime() == 0)
		aGroup->fallBack(INGREDIENT_FALL_SPEED);
	    continue;
	}

	/*  Landing of an ingredient on the one below it:
	    make the latter start falling.
	*/
	IngredientGroup *touchedIngredient =
				findIngredientGroupRightBelow(*aGroup);
	if (touchedIngredient != NULL &&
		    touchedIngredient->getState() == IngredientGroup::NORMAL)
	{
	    size_t numFloorsToGo = aGroup->getNumFloorsToGo();
	    touchedIngredient->startFalling(
				    INGREDIENT_FALL_SPEED, numFloorsToGo);
	    continue;
	}


	/*  Landing of an ingredient on a floor:
	    stop this ingredient.
	*/
	if (isIngredientSpriteOnFloor(*member0))
	{
	    if (aGroup->getState() == IngredientGroup::FALL1)
	    {
		aGroup->bounce(INGREDIENT_FALL_SPEED);
		playSoundEffect(ingredientBouncesSound);
	    }
	    else
	    {
		assert(aGroup->getState() == IngredientGroup::FALL2);
		if (aGroup->decNumFloorsToGo() == 0)
		{
		    aGroup->stop();
		    releaseCarriedEnemies(*aGroup);
		}
		else
		    aGroup->fallToNextFloor(INGREDIENT_FALL_SPEED);

		addToScore(50);
		//createScoreSprites(50, aGroup->getCenterPos());
		// Too many "50" sprites -- becomes annoying
	    }
	    continue;
	}

	detectCollisionBetweenIngredientGroupAndEnemyList(*aGroup,enemySprites);
    }

    if (playIngredientLoweredSound)
	playSoundEffect(ingredientLoweredSound);


    /*  Enemies:
    */
    detectEnemyCollisions(enemySprites);


    /*  Treats:
    */
    for (SpriteList::iterator its = treatSprites.begin();
					its != treatSprites.end(); its++)
    {
	Sprite *treat = *its;
	assert(treat != NULL);
	if (treat->collidesWithSprite(*playerSprite))
	{
	    long score = (treat->currentPixmapIndex + 1) * 500;
	    createScoreSprites(score, treat->getCenterPos());

	    *its = NULL;
	    delete treat;

	    playSoundEffect(chefGetsTreatSound);
	    givePlayerPepper();
	}
    }

    removeNullElementsFromSpriteList(treatSprites);
}


void
BurgerSpaceEngine::detectCollisionBetweenIngredientGroupAndEnemyList(
			    const IngredientGroup &aGroup, SpriteList &enemies)
/*  Enemies in the designated list that collide with the ingredient
    are destroyed.
*/
{
    const Couple groupPos = aGroup.getMember(0)->getPos();
    const Couple groupLRP = aGroup.getMember(3)->getLowerRightPos();
    const Couple groupSize = groupLRP - groupPos;

    for(SpriteList::iterator ite = enemies.begin(); ite != enemies.end(); ite++)
    {
	EnemySprite *anEnemy = dynamic_cast<EnemySprite *>(*ite);
	assert(anEnemy != NULL);

	if (anEnemy->getCarryingGroup() != NULL ||
				    anEnemy->getTimeToDie() != 0 ||
				    anEnemy->getDisappearanceTime() != 0)
	    continue;  // carried, dying or invisible enemies are immune

	if (ingredientGroupCollidesWithSprite(groupPos, groupSize, *anEnemy))
	{
	    playSoundEffect(enemySmashedSound);
	    anEnemy->setTimeToDie(20);
	    break;
	}
    }

    removeNullElementsFromSpriteList(enemies);
}


void
BurgerSpaceEngine::detectEnemyCollisions(SpriteList &slist)
/*  Detects collisions between enemy sprites and the player or the peppers.
*/
{
    for (SpriteList::iterator its = slist.begin(); its != slist.end(); its++)
    {
	EnemySprite *s = dynamic_cast<EnemySprite *>(*its);
	assert(s != NULL);

	if (s->getTimeToLive() != 0 ||
		    s->getTimeToDie() != 0 ||
		    s->getCarryingGroup() != NULL ||
		    s->getDisappearanceTime() != 0)
	    continue;

	// Paralyze the enemy if it touches pepper.
	// The enemy's "time to live" attribute is used for this.
	for (SpriteList::const_iterator itp = pepperSprites.begin();
					itp != pepperSprites.end(); itp++)
	    if (s->collidesWithSprite(**itp))
	    {
		playSoundEffect(enemyParalyzedSound);
		s->setTimeToLive(60 + rand() % 10);
	    }

	if (s->getTimeToLive() != 0)  // if enemy just paralyzed by pepper
	    continue;

	if (numLives != 0 && s->collidesWithSprite(*playerSprite))
	{
	    playSoundEffect(enemyCatchesChefSound);
	    makePlayerDie();
	}
    }

    removeNullElementsFromSpriteList(slist);
}


bool
BurgerSpaceEngine::ingredientGroupCollidesWithSprite(
				const Couple groupPos, const Couple groupSize,
				const Sprite &s) const
{
    const Couple pos1  = s.getPos() + s.getCollBoxPos();
    const Couple size1 = s.getCollBoxSize();
    const Couple pos2  = groupPos;
    const Couple size2 = groupSize;

    if (pos1.x + size1.x <= pos2.x)  // s1 at the left of s2
        return false;
    if (pos1.y + size1.y <= pos2.y)  // s1 above s2
        return false;
    if (pos2.x + size2.x <= pos1.x)  // s1 at the right of s2
        return false;
    if (pos2.y + size2.y <= pos1.y)  // s1 below s2
        return false;
    
    return true;
}


void
BurgerSpaceEngine::createScoreSprites(long n, Couple center)
{
    if (n < 0)
	n = -n;

    addToScore(n);

    char number[64];
    snprintf(number, sizeof(number), "%ld", n);
    size_t numDigits = strlen(number);

    Couple digitSize = digitPA.getImageSize();
    Couple totalSize((digitSize.x + 2) * numDigits - 2, digitSize.y);
    Couple scorePos = center - totalSize / 2;

    for (size_t i = 0; i < numDigits; i++)
    {
        int digit = number[i] - '0';
        Sprite *s = new Sprite(digitPA,
			    scorePos + i * Couple(digitSize.x + 2, 0),
			    Couple(0, -1), Couple(),
			    Couple(), Couple());
        s->setTimeToLive(SCORE_TTL);
	s->currentPixmapIndex = digit;
        scoreSprites.push_back(s);
    }
}


void
BurgerSpaceEngine::loadPixmaps() throw(PixmapLoadError)
{
    /*  Tiles:
    */
    Couple pmSize;
    loadPixmap(empty_xpm,            tilePixmaps[0], pmSize);
    loadPixmap(floor_xpm,            tilePixmaps[1], pmSize);
    loadPixmap(floor_and_ladder_xpm, tilePixmaps[2], pmSize);
    loadPixmap(ladder_xpm,           tilePixmaps[3], pmSize);
    loadPixmap(plate_xpm,            tilePixmaps[4], pmSize);


    /*  Load the player pixmaps:

	WARNING: The order in which the sprites are placed in the PixmapArray
	playerPA is significant.  Do not change it unless all the code that
	refers to the contents of playerPA is properly reviewed and adapted.
    */
    loadPixmap(chef_front0_xpm, playerPA,  0);
    loadPixmap(chef_front1_xpm, playerPA,  1);
    loadPixmap(chef_front2_xpm, playerPA,  2);
    loadPixmap(chef_back0_xpm,  playerPA,  3);
    loadPixmap(chef_back1_xpm,  playerPA,  4);
    loadPixmap(chef_back2_xpm,  playerPA,  5);
    loadPixmap(chef_left0_xpm,  playerPA,  6);
    loadPixmap(chef_left1_xpm,  playerPA,  7);
    loadPixmap(chef_left2_xpm,  playerPA,  8);
    loadPixmap(chef_right0_xpm, playerPA,  9);
    loadPixmap(chef_right1_xpm, playerPA, 10);
    loadPixmap(chef_right2_xpm, playerPA, 11);
    loadPixmap(chef_dead0_xpm,  playerPA, 12);
    loadPixmap(chef_dead1_xpm,  playerPA, 13);
    loadPixmap(chef_dead2_xpm,  playerPA, 14);
    loadPixmap(chef_dead3_xpm,  playerPA, 15);
    loadPixmap(chef_dead4_xpm,  playerPA, 16);
    loadPixmap(chef_dead5_xpm,  playerPA, 17);


    /*  Egg:
    */
    loadPixmap(egg_front0_xpm,    eggPA,  0);
    loadPixmap(egg_front1_xpm,    eggPA,  1);
    loadPixmap(egg_back0_xpm,     eggPA,  2);
    loadPixmap(egg_back1_xpm,     eggPA,  3);
    loadPixmap(egg_left0_xpm,     eggPA,  4);
    loadPixmap(egg_left1_xpm,     eggPA,  5);
    loadPixmap(egg_right0_xpm,    eggPA,  6);
    loadPixmap(egg_right1_xpm,    eggPA,  7);
    loadPixmap(egg_dead0_xpm,     eggPA,  8);
    loadPixmap(egg_dead1_xpm,     eggPA,  9);
    loadPixmap(egg_dead2_xpm,     eggPA, 10);
    loadPixmap(egg_dead3_xpm,     eggPA, 11);
    loadPixmap(egg_peppered0_xpm, eggPA, 12);
    loadPixmap(egg_peppered1_xpm, eggPA, 13);


    /*  Hot dog:
    */
    loadPixmap(hotdog_front0_xpm,    hotdogPA,  0);
    loadPixmap(hotdog_front1_xpm,    hotdogPA,  1);
    loadPixmap(hotdog_back0_xpm,     hotdogPA,  2);
    loadPixmap(hotdog_back1_xpm,     hotdogPA,  3);
    loadPixmap(hotdog_left0_xpm,     hotdogPA,  4);
    loadPixmap(hotdog_left1_xpm,     hotdogPA,  5);
    loadPixmap(hotdog_right0_xpm,    hotdogPA,  6);
    loadPixmap(hotdog_right1_xpm,    hotdogPA,  7);
    loadPixmap(hotdog_dead0_xpm,     hotdogPA,  8);
    loadPixmap(hotdog_dead1_xpm,     hotdogPA,  9);
    loadPixmap(hotdog_dead2_xpm,     hotdogPA, 10);
    loadPixmap(hotdog_dead3_xpm,     hotdogPA, 11);
    loadPixmap(hotdog_peppered0_xpm, hotdogPA, 12);
    loadPixmap(hotdog_peppered1_xpm, hotdogPA, 13);


    /*  Pickle:
    */
    loadPixmap(pickle_front0_xpm,    picklePA,  0);
    loadPixmap(pickle_front1_xpm,    picklePA,  1);
    loadPixmap(pickle_back0_xpm,     picklePA,  2);
    loadPixmap(pickle_back1_xpm,     picklePA,  3);
    loadPixmap(pickle_left0_xpm,     picklePA,  4);
    loadPixmap(pickle_left1_xpm,     picklePA,  5);
    loadPixmap(pickle_right0_xpm,    picklePA,  6);
    loadPixmap(pickle_right1_xpm,    picklePA,  7);
    loadPixmap(pickle_dead0_xpm,     picklePA,  8);
    loadPixmap(pickle_dead1_xpm,     picklePA,  9);
    loadPixmap(pickle_dead2_xpm,     picklePA, 10);
    loadPixmap(pickle_dead3_xpm,     picklePA, 11);
    loadPixmap(pickle_peppered0_xpm, picklePA, 12);
    loadPixmap(pickle_peppered1_xpm, picklePA, 13);


    /*  Pepper:
    */
    loadPixmap(pepper0_xpm, pepperPA, 0);
    loadPixmap(pepper1_xpm, pepperPA, 1);


    /*  Ingredients:
    */
    loadPixmap(top_bun0_xpm, topBunPA, 0);
    loadPixmap(top_bun1_xpm, topBunPA, 1);
    loadPixmap(top_bun2_xpm, topBunPA, 2);
    loadPixmap(top_bun3_xpm, topBunPA, 3);
    loadPixmap(lettuce0_xpm, lettucePA, 0);
    loadPixmap(lettuce1_xpm, lettucePA, 1);
    loadPixmap(lettuce2_xpm, lettucePA, 2);
    loadPixmap(lettuce3_xpm, lettucePA, 3);
    loadPixmap(meat0_xpm, meatPA, 0);
    loadPixmap(meat1_xpm, meatPA, 1);
    loadPixmap(meat2_xpm, meatPA, 2);
    loadPixmap(meat3_xpm, meatPA, 3);
    loadPixmap(redstuff0_xpm, redStuffPA, 0);
    loadPixmap(redstuff1_xpm, redStuffPA, 1);
    loadPixmap(redstuff2_xpm, redStuffPA, 2);
    loadPixmap(redstuff3_xpm, redStuffPA, 3);
    loadPixmap(yellowstuff0_xpm, yellowStuffPA, 0);
    loadPixmap(yellowstuff1_xpm, yellowStuffPA, 1);
    loadPixmap(yellowstuff2_xpm, yellowStuffPA, 2);
    loadPixmap(yellowstuff3_xpm, yellowStuffPA, 3);
    loadPixmap(bottom_bun0_xpm, bottomBunPA, 0);
    loadPixmap(bottom_bun1_xpm, bottomBunPA, 1);
    loadPixmap(bottom_bun2_xpm, bottomBunPA, 2);
    loadPixmap(bottom_bun3_xpm, bottomBunPA, 3);


    /*  Treats:
    */
    loadPixmap(icecream_xpm, treatPA, 0);
    loadPixmap(coffee_xpm,   treatPA, 1);
    loadPixmap(fries_xpm,    treatPA, 2);


    /*  Digits:
    */
    loadPixmap(digit0_xpm, digitPA, 0);
    loadPixmap(digit1_xpm, digitPA, 1);
    loadPixmap(digit2_xpm, digitPA, 2);
    loadPixmap(digit3_xpm, digitPA, 3);
    loadPixmap(digit4_xpm, digitPA, 4);
    loadPixmap(digit5_xpm, digitPA, 5);
    loadPixmap(digit6_xpm, digitPA, 6);
    loadPixmap(digit7_xpm, digitPA, 7);
    loadPixmap(digit8_xpm, digitPA, 8);
    loadPixmap(digit9_xpm, digitPA, 9);
}


void
BurgerSpaceEngine::loadLevel(int levelNo) throw(string)
/*  Affects the data member 'theCurrentLevel', which must already be
    initialized to defined values.
    'levelNo' may exceed NUM_LEVELS.

    Throws an error message string if an error occurs.
*/
{
    levelNo = (levelNo - 1) % NUM_LEVELS + 1;

    const char **levelDesc = levelDescriptorTable[levelNo];
    assert(levelDesc != NULL);
    assert(levelDesc[0] != NULL);
    assert(levelDesc[0][0] != '\0');

    // Count the number of rows and columns:
    size_t numRows = 0, numColumns = 0;
    size_t rowNo;
    for (rowNo = 0; levelDesc[rowNo] != NULL; rowNo++)
    {
	size_t thisRowsLength = strlen(levelDesc[rowNo]);
	if (thisRowsLength == 0)
	    throw __("Row # ") + itoa(rowNo) + __(" is empty");
	if (numColumns == 0)
	    numColumns = thisRowsLength;
	else if (thisRowsLength != numColumns)
	    throw __("Row # ") + itoa(rowNo) + __(" has wrong length");
    }
    numRows = rowNo;

    Couple levelSize = TILE_SIDE * Couple(numColumns, numRows);  // in pixels
    Couple levelPos =
	    (theScreenSizeInPixels - levelSize) / 2;
    levelPos.x = makeDivisibleByInt(levelPos.x, TILE_SIDE);
    levelPos.y = makeDivisibleByInt(levelPos.y, TILE_SIDE);
		    /*  We round the levelPos to the lowest multiple
			of TILE_SIDE; this simplifies computations that do
			modulos with TILE_SIDE;
			do not tamper with this, or there shall be wailing
			and gnashing of teeth.
		    */
    theCurrentLevel.init(levelNo, numColumns, numRows, levelPos);


    for (rowNo = 0; levelDesc[rowNo] != NULL; rowNo++)
    {
	for (size_t colNo = 0; levelDesc[rowNo][colNo] != '\0'; colNo++)
	{
	    XPM xpm = NULL;
	    SDL_Surface *pixmap = NULL;
	    switch (levelDesc[rowNo][colNo])
	    {
		case 'e':  xpm = empty_xpm;  pixmap = tilePixmaps[0]; break;
		case 'f':  xpm = floor_xpm;  pixmap = tilePixmaps[1]; break;
		case 't':  xpm = floor_and_ladder_xpm;
					     pixmap = tilePixmaps[2]; break;
		case 'l':  xpm = ladder_xpm; pixmap = tilePixmaps[3]; break;
		case 'p':  xpm = plate_xpm;  pixmap = tilePixmaps[4]; break;

		default:  throw string(__("Level character '")) +
				    levelDesc[rowNo][colNo] + __("' unknown");
	    }

	    assert(xpm != NULL);
	    assert(pixmap);
	    theCurrentLevel.setTileMatrixEntry(colNo, rowNo, xpm, pixmap);
	}
    }
}


void
BurgerSpaceEngine::restoreBackground()
{
    for (int rowNo = 0; rowNo < theCurrentLevel.sizeInTiles.y; rowNo++)
    {
	SDL_Surface **pixmapRow = theCurrentLevel.getTileMatrixRow(rowNo);

	for (int colNo = 0; colNo < theCurrentLevel.sizeInTiles.x; colNo++)
	{
	    Couple pos = theCurrentLevel.positionInPixels +
			    TILE_SIDE * Couple(colNo, rowNo);
	    copyPixmap(pixmapRow[colNo], pos);
	}
    }
}


void
BurgerSpaceEngine::drawSprites()
{
    int oscBit;  // "oscillating" bit used in selecting pixmap array images
    if (playerSprite->getTimeToLive() == 0)
	oscBit = (tickCount >> 1) & 1;
    else
	oscBit = 0;


    /*  Ingredients:
    */
    for (IngredientSprite::List::const_iterator iti = ingredientSprites.begin();
					iti != ingredientSprites.end(); iti++)
    {
	const IngredientSprite &is = **iti;
	putSprite(is);
    }


    /*  Treats:
    */
    SpriteList::const_iterator it;
    for (it = treatSprites.begin(); it != treatSprites.end(); it++)
	putSprite(**it);


    /*  "Complex" enemies (those with the normal number of images):
    */
    drawComplexEnemySprites(enemySprites,    oscBit);


    /*  Player:
    */
    assert(playerSprite->currentPixmapIndex < playerPA.getNumImages());

    if (numLives > 0)
	putSprite(*playerSprite);


    /*  Peppers:
    */
    for (it = pepperSprites.begin(); it != pepperSprites.end(); it++)
    {
	(*it)->currentPixmapIndex = oscBit;
	putSprite(**it);
    }


    /*  Score sprites:
    */
    for (it = scoreSprites.begin(); it != scoreSprites.end(); it++)
	putSprite(**it);


    /*  Score, number of lives and number of available peppers:
    */
    char s[64];
    snprintf(s, sizeof(s), "%s:%10ld", __("Score"), theScore);
    writeString(s, scoreAreaPos);
    snprintf(s, sizeof(s), "%s:%3d", __("Lives"), numLives);
    writeString(s, numLivesAreaPos);
    snprintf(s, sizeof(s), "%s:%3d", __("Peppers"), numAvailablePeppers);
    writeString(s, numAvailablePeppersAreaPos);
    snprintf(s, sizeof(s), "%s:%3d", __("Level"), cumulLevelNo);
    writeString(s, levelNoAreaPos);

    if (numLives == 0)
	showInstructions();

    const char *pressS = "[S] save game; [L] load game";
    static const Couple fontdim = getFontDimensions();
    int x = (theScreenSizeInPixels.x - strlen(pressS) * fontdim.x) / 2;
    writeString(pressS, Couple(x, numLivesAreaPos.y));
}


void
BurgerSpaceEngine::drawComplexEnemySprites(const SpriteList &slist, int oscBit)
/*  'oscBit' is an "oscillating" bit.  See drawSprites().
*/
{
    for (SpriteList::const_iterator it = slist.begin();
    					it != slist.end(); it++)
    {
	EnemySprite *enemy = dynamic_cast<EnemySprite *>(*it);
	assert(enemy != NULL);

	if (enemy->getDisappearanceTime() != 0)
	    continue;

	int spriteNo = -1;
	if (enemy->getCarryingGroup() != NULL)
	{
	    // don't touch enemy->currentPixmapIndex
	}
	else if (enemy->getTimeToDie() != 0)   // if dying
	{
	    int ttd = enemy->getTimeToDie();
	    assert(ttd >= 0 && ttd <= 20);
	    spriteNo = 11 - (ttd - 1) / 5;
	}
	else if (enemy->getTimeToLive() == 0)  // if not peppered
	{
	    int dir = getDirectionFromCouple(enemy->getSpeed());
	    switch (dir)
	    {
		case DOWN :  spriteNo = 0; break;
		case UP   :  spriteNo = 2; break;
		case LEFT :  spriteNo = 4; break;
		case RIGHT:  spriteNo = 6; break;
		default:     assert(false);
	    }
	    spriteNo |= oscBit;
	}
	else  // peppered:
	{
	    spriteNo = 12 | oscBit;
	}
	
	if (spriteNo != -1)
	    enemy->currentPixmapIndex = spriteNo;

	putSprite(*enemy);
    }
}


void
BurgerSpaceEngine::addToScore(long n)
{
    theScore += n;

    if (theScore > thePeakScore)
    {
	const long newLifeScoreInterval = 10000;
        long before = thePeakScore / newLifeScoreInterval;
        thePeakScore = theScore;
        long after = thePeakScore / newLifeScoreInterval;
        if (before != after)
            addToNumLives(+1);
    }
}


void
BurgerSpaceEngine::addToNumLives(int n)
{
    numLives += n;
}


void
BurgerSpaceEngine::initTimeForTreat()
{
    timeForTreat = Rnd(20 * 20, 45 * 20);
}


void
BurgerSpaceEngine::displayPauseMessage(bool display)
{
    if (display)
	displayMessage(5, "PAUSED -- press P to resume");
}


void
BurgerSpaceEngine::displayStartMessage(bool display)
/*  Displays the start message if 'display' is true, or erases the
    corresponding region if 'display' is false.
*/
{
    if (display)
    {
	displayMessage(0, "BurgerSpace " VERSION " - by Pierre Sarrazin");
	displayMessage(2, "Move with arrow keys - shoot pepper with Ctrl key");
	displayMessage(3, "Press SPACE to start - press Escape to quit");

	if (useSound && theSoundMixer == NULL)
	    displayMessage(5, "*** Sound device not available ***");
    }
}


void
BurgerSpaceEngine::displayMessage(int row, const char *msg)
{
    static const Couple fontdim = getFontDimensions();

    const size_t len = strlen(msg);
    int x = (theScreenSizeInPixels.x - len * fontdim.x) / 2;
    int y = theScreenSizeInPixels.y + (row - 10) * fontdim.y;

    writeString(msg, Couple(x, y));
}


void
BurgerSpaceEngine::playSoundEffect(SoundMixer::Chunk &chunk)
{
    if (theSoundMixer != NULL)
    {
	try
	{
	    theSoundMixer->playChunk(chunk);
	}
	catch (const SoundMixer::Error &e)
	{
	    fprintf(stderr, "playSoundEffect: %s (chunk at %p)\n",
			e.what().c_str(), &chunk);
	}
    }
}


void
BurgerSpaceEngine::drawQuitDialog()
{
    paused = true;
    if (numLives > 0)
	displayPauseMessage(true);
    showDialogBox("QUIT GAME: are you sure? (Y=yes, N=no) ");
}


bool
BurgerSpaceEngine::doQuitDialog()
{
    if (lastKeyPressed == SDLK_y)
	return false;
    if (lastKeyPressed == SDLK_n)
    {
	lastKeyPressed = SDLK_UNKNOWN;
	showDialogBox("Game NOT quit. Press P to resume.");
	inQuitDialog = false;
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////
//
// SAVING AND LOADING GAMES
//


int BurgerSpaceEngine::savedGameFormatVersion = 1;


void
BurgerSpaceEngine::drawSaveDialog()
{
    paused = true;
    if (numLives > 0)
	displayPauseMessage(true);
    showDialogBox("SAVE GAME: which slot? (1-9, Q=cancel) ");
    showSlotDirectory();
}


void
BurgerSpaceEngine::drawLoadDialog()
{
    paused = true;
    if (numLives > 0)
	displayPauseMessage(true);
    showDialogBox("LOAD GAME: which slot? (1-9, Q=cancel) ");
    showSlotDirectory();
}


int
BurgerSpaceEngine::getSlotNumber()
{
    if (lastKeyPressed >= SDLK_1 && lastKeyPressed <= SDLK_9)
    {
	int slotNum = int(lastKeyPressed - SDLK_1) + 1;
	lastKeyPressed = SDLK_UNKNOWN;
	return slotNum;
    }

    if (lastKeyPressed == SDLK_q)
    {
	lastKeyPressed = SDLK_UNKNOWN;
	return -2;
    }

    return -1;
}


void
BurgerSpaceEngine::doSaveDialog()
{
    int slotNum = getSlotNumber();

    if (slotNum > 0)
    {
	char temp[512];
	snprintf(temp, sizeof(temp), "Saving game in slot %d...", slotNum);
	showDialogBox(temp);

	string dir = getSavedGamesDir();
	if (dir.empty())
	{
	    snprintf(temp, sizeof(temp),
				"No usable directory for saved games.");
	}
	else
	{
	    string filename = dir + "/" + formSavedGameFilename(slotNum);
	    ofstream out(filename.c_str());
	    if (!out)
	    {
		int e = errno;
		snprintf(temp, sizeof(temp), "Could not create slot %d.",
								    slotNum);
		fprintf(stderr, "%s: could not write to %s: %s\n",
					PACKAGE, filename.c_str(), strerror(e));
	    }
	    else
	    {
		saveGame(out);
		snprintf(temp, sizeof(temp),
			"Game saved in slot %d. Press P to resume.", slotNum);
	    }
	}
	showDialogBox(temp);
	inSaveDialog = false;
	return;
    }

    if (slotNum == -2)
    {
	lastKeyPressed = SDLK_UNKNOWN;
	showDialogBox("Game NOT saved. Press P to resume.");
	inSaveDialog = false;
	return;
    }
}


void
BurgerSpaceEngine::doLoadDialog()
{
    int slotNum = getSlotNumber();

    if (slotNum > 0)
    {
	char temp[512];
	snprintf(temp, sizeof(temp), "Loading game from slot %d...", slotNum);
	showDialogBox(temp);


	string dir = getSavedGamesDir();
	if (dir.empty())
	{
	    snprintf(temp, sizeof(temp),
				"No usable directory. Press P to resume.");
	}
	else
	{
	    string filename = dir + "/" + formSavedGameFilename(slotNum);
	    ifstream in(filename.c_str());
	    if (!in)
	    {
		snprintf(temp, sizeof(temp),
		    "No game saved in slot %d. Press P to resume.", slotNum);
	    }
	    else
	    {
		int errorCode = loadGame(in);
		if (errorCode != 0)
		    snprintf(temp, sizeof(temp), "Load error: %d\n", errorCode);
		else
		{
		    // Show the new game situation:
		    restoreBackground();
		    drawSprites();
		    displayPauseMessage(true);

		    snprintf(temp, sizeof(temp),
			    "Game loaded from slot %d. Press P to resume.",
								    slotNum);
		}
	    }
	}
	showDialogBox(temp);
	inLoadDialog = false;
	return;
    }

    if (slotNum == -2)
    {
	lastKeyPressed = SDLK_UNKNOWN;
	showDialogBox("No game loaded. Press P to resume.");
	inLoadDialog = false;
	return;
    }
}


void
BurgerSpaceEngine::showDialogBox(const string &msg)
{ 
    static const Couple fontdim = getFontDimensions();
    static const size_t maxlen = 50;
    static const char *spaces = 
		    "                                                      ";

    string line = msg.substr(0, maxlen);
    size_t margin = (maxlen + 4 - line.length()) / 2;
    string blanks(spaces, margin);
    line = blanks + line + blanks;

    Couple pos((theScreenSizeInPixels.x - (maxlen + 4) * fontdim.x) / 2,
		8 * fontdim.y);
    writeString(spaces, pos + Couple(0, 0 * fontdim.y));
    writeString(line,   pos + Couple(0, 1 * fontdim.y));
    writeString(spaces, pos + Couple(0, 2 * fontdim.y));
}


string
BurgerSpaceEngine::serialize(Couple c) const
{
    char temp[512];
    snprintf(temp, sizeof(temp), "%d %d", c.x, c.y);
    return temp;
}


int
BurgerSpaceEngine::encodePixmapArrayPointer(const PixmapArray *ptr) const
{
    if (ptr == &playerPA)           return  0;
    if (ptr == &pepperPA)           return  1;
    if (ptr == &eggPA)              return  2;
    if (ptr == &hotdogPA)           return  3;
    if (ptr == &picklePA)           return  4;
    if (ptr == &topBunPA)           return  5;
    if (ptr == &lettucePA)          return  6;
    if (ptr == &meatPA)             return  7;
    if (ptr == &redStuffPA)         return  8;
    if (ptr == &yellowStuffPA)      return  9;
    if (ptr == &bottomBunPA)        return 10;
    if (ptr == &treatPA)            return 11;
    if (ptr == &digitPA)            return 12;
    return -1;
}

const PixmapArray *
BurgerSpaceEngine::decodePixmapArrayCode(int code) const
{
    if (code ==  0) return &playerPA;
    if (code ==  1) return &pepperPA;
    if (code ==  2) return &eggPA;
    if (code ==  3) return &hotdogPA;
    if (code ==  4) return &picklePA;
    if (code ==  5) return &topBunPA;
    if (code ==  6) return &lettucePA;
    if (code ==  7) return &meatPA;
    if (code ==  8) return &redStuffPA;
    if (code ==  9) return &yellowStuffPA;
    if (code == 10) return &bottomBunPA;
    if (code == 11) return &treatPA;
    if (code == 12) return &digitPA;
    return NULL;
}


string
BurgerSpaceEngine::serialize(const Sprite &s) const
{
    char temp[512];
    Couple pos = s.getPos();
    Couple speed = s.getSpeed();
    Couple collBoxPos = s.getCollBoxPos();
    Couple collBoxSize = s.getCollBoxSize();
    snprintf(temp, sizeof(temp), "%d %d %d %d %d %d %d %d %d %d",
			    pos.x, pos.y, 
			    speed.x, speed.y,
			    collBoxPos.x, collBoxPos.y,
			    collBoxSize.x, collBoxSize.y,
			    int(s.currentPixmapIndex),
			    encodePixmapArrayPointer(s.getPixmapArray()));
    return temp;
}


string
BurgerSpaceEngine::serialize(const EnemySprite &es) const
{
    string s = serialize((Sprite &) es);
    char temp[512];
    snprintf(temp, sizeof(temp), " %d %d",
			es.getDisappearanceTime(), es.getTimeToDie());
    return s + temp;
}


void
BurgerSpaceEngine::serializeSpriteList(ostream &out,
					const SpriteList &list) const
// The sprites are serialized in the order in which they appear in 'list'.
{
    out << list.size() << "\n";
    for (SpriteList::const_iterator it = list.begin(); it != list.end(); it++)
	out << serialize(**it) << "\n";
}


void
BurgerSpaceEngine::serializeEnemySpriteList(ostream &out,
					    const SpriteList &list) const
// The sprites are serialized in the order in which they appear in 'list'.
{
    out << list.size() << "\n";
    for (SpriteList::const_iterator it = list.begin(); it != list.end(); it++)
	out << serialize(**it) << "\n";
}


string
BurgerSpaceEngine::serialize(const IngredientSprite &is) const
{
    char temp[512];
    snprintf(temp, sizeof(temp), "%d %d %d %d %d %d",
		encodePixmapArrayPointer(is.getPixmapArray()),
		is.getPos().x,
		is.getPos().y,
		is.getSpeed().x,
		is.getSpeed().y,
		is.isLowered());
    return temp;
}


void
BurgerSpaceEngine::saveGame(ostream &out)
{
    out << savedGameFormatVersion << "\n";
    out << "\n";

    out
	<< initLevelNo << "\n"
	<< cumulLevelNo << "\n"
	<< tickCount << "\n"
	<< serialize(initPlayerPos) << "\n"
	<< serialize(*playerSprite) << "\n"
	<< lastPlayerDirection << "\n"
	<< oldMotionMode << "\n"
	;

    serializeSpriteList(out, pepperSprites);

    out << timeForNewEnemy << "\n";

    serializeEnemySpriteList(out, enemySprites);

    out << "\n";
    out << ingredientGroups.size() << "\n";
    for (IngredientGroup::List::const_iterator it = ingredientGroups.begin();
					    it != ingredientGroups.end(); it++)
    {
	const IngredientGroup &group = **it;
	out << "\n";

	out << group.getVerticalTarget() << " "
	    << group.getBounceTime() << " "
	    << group.getStallTime() << " "
	    << (int) group.getState() << " "
	    << group.getNumFloorsToGo() << " "
	    << group.isTopBun() << "\n";

	// Write indexes in enemySprites for each carried enemy:
	const SpriteList &enemies = group.getCarriedEnemies();
	out << enemies.size();
	for (SpriteList::const_iterator jt = enemies.begin();
						jt != enemies.end(); jt++)
	{
	    SpriteList::iterator kt = find(
				enemySprites.begin(), enemySprites.end(), *jt);
	    assert(kt != enemySprites.end());
	    size_t index = size_t(distance(enemySprites.begin(), kt));
	    out << " " << index;
	}
	out << "\n";

	for (size_t i = 0; i < 4; i++)
	    out << serialize(*group.getMember(i)) << "\n";
    }
    out << "\n";

    serializeSpriteList(out, treatSprites);
    out << timeForTreat << "\n";

    serializeSpriteList(out, scoreSprites);

    out
	<< numHamburgersToDo << "\n"
	<< thePeakScore << "\n"
	<< theScore << "\n"
	<< celebrationMode << "\n"
	<< numLives << "\n"
	<< numAvailablePeppers << "\n"
	;
}


bool
BurgerSpaceEngine::deserialize(istream &in, Couple &c) const
{
    in >> c.x >> c.y;
    return in.good();
}


Sprite *
BurgerSpaceEngine::deserializeSprite(istream &in, bool enemy) const
{
    Couple pos, speed, collBoxPos, collBoxSize;
    int pixmapIndex, pixmapArrayCode;
    in >> pos.x >> pos.y
	>> speed.x >> speed.y
	>> collBoxPos.x >> collBoxPos.y
	>> collBoxSize.x >> collBoxSize.y
	>> pixmapIndex
	>> pixmapArrayCode;
    if (!in)
	return NULL;

    const PixmapArray *pa = decodePixmapArrayCode(pixmapArrayCode);
    if (pa == NULL)
	return NULL;

    Sprite *s;
    if (enemy)
	s = new EnemySprite(*pa, pos, speed, collBoxPos, collBoxSize);
    else
	s = new Sprite(*pa, pos, speed, Couple(), collBoxPos, collBoxSize);

    s->currentPixmapIndex = static_cast<size_t>(pixmapIndex);
    return s;
}


IngredientSprite *
BurgerSpaceEngine::deserializeIngredientSprite(istream &in,
						IngredientGroup *ig) const
{
    int pixmapArrayCode, x, y, dx, dy, isLowered;
    in >> pixmapArrayCode >> x >> y >> dx >> dy >> isLowered;
    if (!in)
	return NULL;

    const PixmapArray *pa = decodePixmapArrayCode(pixmapArrayCode);
    if (pa == NULL)
	return NULL;

    IngredientSprite *is = new IngredientSprite(*pa, Couple(x, y), ig);
    is->setSpeed(Couple(dx, dy));
    if (isLowered)
	is->setLowered();
    return is;
}


bool
BurgerSpaceEngine::deserializeSpriteList(istream &in,
					SpriteList &list,
					bool enemies) const
{
    size_t numSprites;
    in >> numSprites;
    if (!in || numSprites > 9999)
	return false;

    list.clear();
    for (size_t i = 0; i < numSprites; i++)
    {
	Sprite *s = deserializeSprite(in, enemies);
	if (s == NULL)
	{
	    deleteSprites(list);
	    return false;
	}
	list.push_back(s);
    }
    return true;
}


int
BurgerSpaceEngine::loadGame(istream &in)
{
    int version;
    in >> version;
    if (!in)
	return -1;
    if (version != savedGameFormatVersion)
	return -2;

    in >> initLevelNo >> cumulLevelNo >> tickCount;
    if (!in)
	return -3;

    initNextLevel(cumulLevelNo);

    if (!deserialize(in, initPlayerPos))
	return -4;

    delete playerSprite;
    playerSprite = deserializeSprite(in);
    if (playerSprite == NULL)
	return -5;

    in >> lastPlayerDirection >> oldMotionMode;
    if (!in)
	return -6;

    if (!deserializeSpriteList(in, pepperSprites))
	return -7;

    in >> timeForNewEnemy;
    if (!in)
	return -8;

    if (!deserializeSpriteList(in, enemySprites, true))
	return -9;

    size_t numIngredientGroups;
    in >> numIngredientGroups;
    if (!in)
	return -10;

    ingredientSprites.clear();
    ingredientGroups.clear();

    for (size_t i = 0; i < numIngredientGroups; i++)
    {
	int verticalTarget, bounceTime, stallTime, state;
	size_t numFloorsToGo;
	bool isTopBun;
	in >> verticalTarget >> bounceTime >> stallTime
			>> state >> numFloorsToGo >> isTopBun;
	if (!in)
	    return -11;

	size_t numCarriedEnemies;
	in >> numCarriedEnemies;
	if (!in)
	    return -12;

	vector<size_t> enemyIndexes;
	for (size_t j = 0; j < numCarriedEnemies; j++)
	{
	    size_t index;
	    in >> index;
	    if (!in || index >= enemySprites.size())
		return -13;
	    enemyIndexes.push_back(index);
	}

	IngredientGroup *group = new IngredientGroup(verticalTarget, isTopBun);
	group->restore(bounceTime, stallTime,
			IngredientGroup::State(state), numFloorsToGo);

	// Read and insert the IngredientSprite members of this group:
	for (size_t k = 0; k < 4; k++)
	{
	    IngredientSprite *is = deserializeIngredientSprite(in, group);
	    if (is == NULL)
	    {
		delete group;
		return -14;
	    }
	    group->setMember(k, is);
	    is->currentPixmapIndex = k;
	    ingredientSprites.push_back(is);
	}

	// Tell each carried enemy by which group it is carried:
	for (vector<size_t>::const_iterator jt = enemyIndexes.begin();
					    jt != enemyIndexes.end(); jt++)
	{
	    size_t enemyIndex = *jt;
	    Sprite *s = enemySprites.at(enemyIndex);
	    EnemySprite *es = dynamic_cast<EnemySprite *>(s);
	    if (s == NULL || es == NULL)
	    {
		delete group;
		return -15;
	    }
	    group->addCarriedEnemy(es);
	}


	ingredientGroups.push_back(group);
    }

    if (!deserializeSpriteList(in, treatSprites))
	return -16;
    in >> timeForTreat;
    if (!in)
	return -17;

    if (!deserializeSpriteList(in, scoreSprites))
	return -18;

    in
	>> numHamburgersToDo
	>> thePeakScore
	>> theScore
	>> celebrationMode
	>> numLives
	>> numAvailablePeppers
	;
    if (!in)
	return -19;

    return 0;
}


/*static*/
string
BurgerSpaceEngine::getSavedGamesDir()
/*  Returns the full path of a directory in which files can be created
    for saved games.  The path does not finish with a '/'.
    If no such path could be formed, an empty string is returned.
*/
{
    const char *home = getenv("HOME");  // TODO: what about non-Unix systems?
    if (home == NULL)
	return "";
    
    struct stat statbuf;
    if (stat(home, &statbuf) != 0)
    {
	int e = errno;
	fprintf(stderr, "%s: stat(%s): %s\n", PACKAGE, home, strerror(e));
	return "";
    }
    if (!S_ISDIR(statbuf.st_mode))
    {
	fprintf(stderr, "%s: $HOME (%s) is not a directory\n", PACKAGE, home);
	return "";
    }

    string subdir = home + string("/") + "." + PACKAGE;
    if (stat(subdir.c_str(), &statbuf) != 0)
    {
	if (mkdir(subdir.c_str(), 0700) != 0)
	{
	    int e = errno;
	    fprintf(stderr, "%s: mkdir(%s): %s\n",
					PACKAGE, subdir.c_str(), strerror(e));
	    return "";
	}
    }
    else if (!S_ISDIR(statbuf.st_mode))
    {
	fprintf(stderr, "%s: %s is not a directory\n", PACKAGE, subdir.c_str());
	return "";
    }

    if (access(subdir.c_str(), W_OK) != 0)
    {
	fprintf(stderr, "%s: %s is not a writable directory\n",
						PACKAGE, subdir.c_str());
	return "";
    }

    return subdir;
}


/*static*/
string
BurgerSpaceEngine::formSavedGameFilename(int slotNum)
{
    char temp[512];
    snprintf(temp, sizeof(temp), "saved-game-%d.dat", slotNum);
    return temp;
}


void
BurgerSpaceEngine::showSlotDirectory()
{
    vector<string> slotTable;
    slotTable.push_back("");
    slotTable.push_back("No  Save Time");
    slotTable.push_back("--  -------------------");

    string dirname = getSavedGamesDir();

    // Scan the saved games directory and build a table of available games:
    for (size_t slotNum = 1; slotNum <= 9; slotNum++)
    {
	string slotDesc = string(" ") + char('0' + slotNum) + "  ";
	string filename = dirname + "/" + formSavedGameFilename(slotNum);
	ifstream file(filename.c_str());
	if (!file)
	    slotDesc += "(empty)";
	else
	{
	    struct stat statbuf;
	    if (stat(filename.c_str(), &statbuf) != 0)
		slotDesc += "(empty)";
	    else
	    {
		struct tm t = *localtime(&statbuf.st_mtime);
		char fileTime[512];
		strftime(fileTime, sizeof(fileTime), "%Y-%m-%d %H:%M:%S", &t);
		slotDesc += fileTime;
	    }
	}
	slotTable.push_back(slotDesc);
    }

    slotTable.push_back("--  -------------------");
    slotTable.push_back("");

    // Find length of longest line:
    size_t maxlen = 0;
    for (vector<string>::const_iterator it = slotTable.begin();
					    it != slotTable.end(); it++)
	maxlen = max(maxlen, it->length());

    // Write the table:
    static const Couple fontdim = getFontDimensions();
    static const char *spaces =
       "                                                                      ";
    Couple pos((theScreenSizeInPixels.x - (maxlen + 4) * fontdim.x) / 2,
		12 * fontdim.y);

    size_t row = 0;
    for (vector<string>::const_iterator it = slotTable.begin();
					    it != slotTable.end(); it++, row++)
    {
	string s = "  " + *it + string(spaces, maxlen + 2 - it->length());
	writeString(s, pos + Couple(0, row * fontdim.y));
    }
}
