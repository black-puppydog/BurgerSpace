#ifndef _H_Burgerspace_LevelSet_
#define _H_Burgerspace_LevelSet_

//#include "BurgerSpaceServer.h"
#include "Helpers.h"
#include "Level.h"

#include <vector>
#include <string>

using namespace std;

struct LevelDescription
{
  public:
        vector<string> LineStrings;
	vector<IngInit> tableOfIngredients;
	IntQuad enemyStartingHeights;
	IntPair playerStartingPosition;
};

class LevelSet
{
  public:
    int getNumLevels();
    LevelDescription getLevelDescription(int levelNumber);

  protected:
    LevelSet();
    ~LevelSet();
    void setLevelDescriptions(vector<LevelDescription> * l);

  private:
    vector<LevelDescription> * levelDescriptions;
};
	
	
class SimpleLevelSetImplementation : public LevelSet
{
  public:

	SimpleLevelSetImplementation();
	~SimpleLevelSetImplementation();

//	LevelDescription * getLevelDescription(int levelNumber);
//	int getNumLevels();

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

    protected:
        vector<LevelDescription> * initializeLevelDescriptions();

};



#endif
