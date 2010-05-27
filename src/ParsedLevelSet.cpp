#include "ParsedLevelSet.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <assert.h>



using namespace std;

ParsedLevelSet::ParsedLevelSet()
{
    lineString="-lines:";
    pPosString="-playerstart:";
    eStartString="-enemystartingheights:";
    ingString="-ingredients:";
    setLevelDescriptions(parseLevels());
}


vector<LevelDescription> * ParsedLevelSet::parseLevels()
        /* parses the level descriptions from a yaml-like file */
{
    vector<vector<string> > * levelStrings = new vector<vector<string> >();

    ifstream inFile("testlevels.yaml");
    string line;

    vector<string> levelString = vector<string>();
    bool firstLevel=false;

    while(getline(inFile, line))
    {
        trim(line);
        if(line.size()==0 || line.at(0)=='#')
            continue;

        // check if this line starts a new level description:
        if(!firstLevel && starts_with(line, lineString))
        {
            // append this levelstring to the others
            levelStrings->push_back(levelString);

            // replace the old levelstring with a new one
            levelString = vector<string>();
        }

        // add the line to the (possibly new) levelstring
        levelString.push_back(line);
    }
    inFile.close();

    vector<LevelDescription> * levelDescs = new vector<LevelDescription>();
    // now we need to make every levelstring into a levelDescription
    for(int i=0; i<levelStrings->size(); i++)
    {
        LevelDescription l = parseSingleLevel(levelStrings->at(i));


        cout << "loaded level no. " << i+1 << endl;
        cout << "Description lines:" << endl;
        for(int j=0;j<l.LineStrings.size();j++)
        {
            cout << l.LineStrings.at(j) << endl;
        }
        cout << endl;
        cout << endl << "Enemy starting heights: " << l.enemyStartingHeights.first << ", " << l.enemyStartingHeights.second << ", "  << l.enemyStartingHeights.third << ", "  << l.enemyStartingHeights.fourth << endl;
        cout << "player starting position: " << l.playerStartingPosition.first << ", " << l.playerStartingPosition.second << endl;
        levelDescs->push_back(l);
    }

    return levelDescs;
}

LevelDescription ParsedLevelSet::parseSingleLevel(vector<string>& levelString)
{
    LevelDescription * ld = new LevelDescription();


    vector<string> * levelLinesString = &(ld->LineStrings);
    vector<string> * ingredientsString = new vector<string>();


    // this points to the vector we are currently filling
    vector<string> * workingOn;


    for(int lineNo=0; lineNo<levelString.size(); lineNo++)
    {
        string line = levelString[lineNo];

        if(starts_with(line, lineString)){
            workingOn=levelLinesString;
            continue; // nothing left to do here
        }

        else if(starts_with(line, pPosString)){
            ld->playerStartingPosition=parsePlayerStartingPos(line);

        }

        else if(starts_with(line, eStartString)){
            ld->enemyStartingHeights=parseEnemyStartingHeights(line);
        }

        else if(starts_with(line, ingString)){
            workingOn=ingredientsString;
            continue; // nothing left to do here
        }

        else {
        workingOn->push_back(line);
        }
    }

    return *ld;
}

IntPair ParsedLevelSet::parsePlayerStartingPos(string playerPosLine)
{
    assert(starts_with(playerPosLine, pPosString));

    string posArray = playerPosLine.substr(pPosString.size());

    assert(posArray.size()>=5); // minimum size is 5: [x,y]
    assert(posArray.at(0)=='[');
    assert(posArray.at(posArray.size()-1)==']');

    posArray=posArray.substr(1, posArray.size()-2); // remove "[]"
    int comma = posArray.find(',', 0);
    IntPair result = IntPair();
    result.first=parseInt(posArray.substr(0, comma));
    result.second=parseInt(posArray.substr(comma+1));
    return result;

}


IntQuad ParsedLevelSet::parseEnemyStartingHeights(string enemyStartPosLine)
{
    assert(starts_with(enemyStartPosLine, eStartString));

    string posArray = enemyStartPosLine.substr(eStartString.size());

    assert(posArray.size()>=9); // minimum size is 9: [w,x,y,z]
    assert(posArray.at(0)=='[');
    assert(posArray.at(posArray.size()-1)==']');

    posArray=posArray.substr(1, posArray.size()-2); // remove "[]"

    int comma;
    IntQuad result = IntQuad();

    // parse first
    comma = posArray.find(',', 0);
    result.first=parseInt(posArray.substr(0, comma));
    posArray=posArray.substr(comma+1);

    // parse second
    comma = posArray.find(',', 0);
    result.second=parseInt(posArray.substr(0, comma));
    posArray=posArray.substr(comma+1);

    // parse third
    comma = posArray.find(',', 0);
    result.third=parseInt(posArray.substr(0, comma));
    posArray=posArray.substr(comma+1);

    // parsing fouth is easy now
    result.fourth=parseInt(posArray);

    return result;
}




int ParsedLevelSet::parseInt(string s)
{
    istringstream strin;
    int var;
    strin.str(s);
    strin >> var;
    return var;
}


// trim from both ends
void ParsedLevelSet::trim(std::string &s) {
    remove_if(s.begin(), s.end(), ::isspace);

}

// trim from start
void ParsedLevelSet::ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end
void ParsedLevelSet::rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// check if one string is prefix of the other
bool ParsedLevelSet::starts_with(string& haystack, string& needle)
{
  return needle.length() <= haystack.length() &&
    std::equal(needle.begin(), needle.end(), haystack.begin());
}
