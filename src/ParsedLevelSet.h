#ifndef PARSEDLEVELSET_H
#define PARSEDLEVELSET_H

#include "LevelSet.h"
#include <vector>
#include <string>
#include "Helpers.h"


class ParsedLevelSet: public LevelSet
{
public:
    ParsedLevelSet(std::string levelfile);
    ~ParsedLevelSet();
private:
    
    std::string levelFile;
    
    // main worker method
    vector<LevelDescription> * parseLevels();

    // methods for parsing on different levels
    LevelDescription * parseSingleLevel(std::vector<std::string> * ls);
    IntPair parsePlayerStartingPos(std::string playerPosLine);
    IntQuad parseEnemyStartingHeights(std::string enemyStartPosLine);
    std::vector<IngInit> * parseIngredients(std::vector<std::string> * ingredientsString);
    IngInit parseSingleIngredient(std::string ingString);
    IngInit::IngType parseIngredientType(std::string ingString);
    

    // helper functions
    void trim(std::string &s);
    bool starts_with(string& haystack, string& needle);
    void rtrim(std::string &s);
    void ltrim(std::string &s);
    int parseInt(std::string s);

    // a lot of strings for comparisons
    std::string lineString;
    std::string pPosString;
    std::string eStartString;
    std::string ingString;
};

#endif // PARSEDLEVELSET_H
