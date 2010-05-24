#ifndef _H_Burgerspace_LevelSet_
#define _H_Burgerspace_LevelSet_

//#include "BurgerSpaceServer.h"
#include "Helpers.h"
#include "Level.h"

using namespace std;

struct LevelDescription
{
  public:
	const char ** LineStrings;

	IngInit * tableOfIngredients;

	IntQuad enemyStartingHeights;

	IntPair playerStartingPosition;
};

class LevelSet
{
  public:
    virtual int getNumLevels()=0;

    virtual LevelDescription * getLevelDescription(int levelNumber)=0;
};
	
	
class SimpleLevelSetImplementation : LevelSet
{
  public:

	SimpleLevelSetImplementation();
	~SimpleLevelSetImplementation();

	LevelDescription * getLevelDescription(int levelNumber);
	int getNumLevels();

  private:

	static const char **levelDescriptorTable[];
    static IngInit *tableOfTablesOfIngredientsLevel[];
    static IntQuad enemyStartingHeights[];
    static IntPair playerStartingPos[];
    

    static const char
        *levelDescriptor1[],
        *levelDescriptor2[],
        *levelDescriptor3[],
        *levelDescriptor4[],
        *levelDescriptor5[],
        *levelDescriptor6[];

    static IngInit
        tableIngredientsLevel1[],
        tableIngredientsLevel2[],
        tableIngredientsLevel3[],
        tableIngredientsLevel4[],
        tableIngredientsLevel5[],
        tableIngredientsLevel6[];
};



#endif
