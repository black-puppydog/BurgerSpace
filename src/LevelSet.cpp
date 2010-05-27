#ifndef _Burgerspace_LevelSet_
#define _Burgerspace_LevelSet_

#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include "LevelSet.h"
#include "BurgerSpaceServer.h"


const char *
SimpleLevelSetImplementation::levelDescriptor1[] =
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
    "eeetffftffftftfteleleee",
    "eeeleeeleeeleleleleleee",
    "eeefffffffffffffffffeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeeeeeeeeeeeeeeeeeeeee",
    "eeeepppepppepppepppeeee",
    NULL  // marks the end
};


const char *
SimpleLevelSetImplementation::levelDescriptor2[] =
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


const char *
SimpleLevelSetImplementation::levelDescriptor3[] =
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


const char *
SimpleLevelSetImplementation::levelDescriptor4[] =
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


const char *
SimpleLevelSetImplementation::levelDescriptor5[] =
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


const char *
SimpleLevelSetImplementation::levelDescriptor6[] =
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



const char **
SimpleLevelSetImplementation::levelDescriptorTable[] =
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


IngInit SimpleLevelSetImplementation::tableIngredientsLevel1[] =
{
    // 1st column:
    {  4, 11, 15,  1, IngInit::BOTTOM_BUN },
    {  4,  9, 15,  2, IngInit::MEAT       },
    {  4,  6, 15,  3, IngInit::LETTUCE    },
    {  4,  2, 15,  4, IngInit::TOP_BUN    },

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


/*static*/ IngInit
SimpleLevelSetImplementation::tableIngredientsLevel2[] =
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


/*static*/ IngInit
SimpleLevelSetImplementation::tableIngredientsLevel3[] =
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


/*static*/ IngInit
SimpleLevelSetImplementation::tableIngredientsLevel4[] =
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


/*static*/ IngInit
SimpleLevelSetImplementation::tableIngredientsLevel5[] =
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


/*static*/ IngInit
SimpleLevelSetImplementation::tableIngredientsLevel6[] =
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

IngInit *
SimpleLevelSetImplementation::tableOfTablesOfIngredientsLevel[] =
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


IntQuad
SimpleLevelSetImplementation::enemyStartingHeights[] =
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


IntPair
SimpleLevelSetImplementation::playerStartingPos[] =
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

vector<LevelDescription> *
SimpleLevelSetImplementation::initializeLevelDescriptions()
{
    vector<LevelDescription> * result = new vector<LevelDescription>();

    for(int levelNumber=1;levelNumber<=6;levelNumber++)
    {        
//        cout << endl << "loading level description of level no. " << levelNumber << endl;

        LevelDescription * l = new LevelDescription;
        l->enemyStartingHeights = enemyStartingHeights[levelNumber];
        l->playerStartingPosition=playerStartingPos[levelNumber];
        l->tableOfIngredients=tableOfTablesOfIngredientsLevel[levelNumber];

        // construct strings
        l->LineStrings = * new vector<string>();
        int j=0;
        while(levelDescriptorTable[levelNumber][j]!=NULL)
        {
//            cout << "adding level line to levelDescription: " << levelDescriptorTable[levelNumber][j] << endl;
            l->LineStrings.push_back(levelDescriptorTable[levelNumber][j]);
            j++;
        }

        cout << "loaded level no. " << levelNumber << endl;
        cout << "Description lines:" << endl;
        for(int i=0;i<l->LineStrings.size();i++)
        {
            cout << l->LineStrings.at(i) << endl;
        }
        cout << endl << "Enemy starting heights: " << l->enemyStartingHeights.first << ", " << l->enemyStartingHeights.second << ", "  << l->enemyStartingHeights.third << ", "  << l->enemyStartingHeights.fourth << endl;
        cout << "player starting position: " << l->playerStartingPosition.first << ", " << l->playerStartingPosition.second << endl;
        result->push_back(*l);
    }
    cout << "loading of level descriptions successful" << endl;
    return result;
}


SimpleLevelSetImplementation::SimpleLevelSetImplementation()
{
    setLevelDescriptions(initializeLevelDescriptions());
}


SimpleLevelSetImplementation::~SimpleLevelSetImplementation()
{
}

LevelSet::LevelSet()
{
    // nothing to do here
}

LevelSet::~LevelSet()
{

}

LevelDescription LevelSet::getLevelDescription(int levelNumber)
{
    assert(levelNumber>=0);
    assert(levelNumber<levelDescriptions->size());
    LevelDescription result = levelDescriptions->at(levelNumber);
    return result;
}

int
LevelSet::getNumLevels()
{
    return levelDescriptions->size();
}

void LevelSet::setLevelDescriptions(vector<LevelDescription> *l)
{
    levelDescriptions=l;
}

#endif
