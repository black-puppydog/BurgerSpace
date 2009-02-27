/*  $Id: BurgerSpaceEngine.h,v 1.31 2009/02/27 02:50:42 sarrazip Exp $
    BurgerSpaceEngine.h - Main engine

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

#ifndef _H_BurgerSpaceEngine
#define _H_BurgerSpaceEngine

#include "IngredientGroup.h"
#include "IngredientSprite.h"
#include "ChefController.h"

#include <flatzebra/GameEngine.h>
#include <flatzebra/Sprite.h>
#include <flatzebra/SoundMixer.h>
#include <flatzebra/KeyState.h>

#include <string>
#include <iostream>


class BurgerSpaceEngine : public flatzebra::GameEngine
/*  A hamburger-smashing video game.
*/
{
public:

    typedef const char **XPM;

    BurgerSpaceEngine(const std::string &windowManagerCaption,
			int initLevelNumber,
			bool useSound,
			bool fullScreen,
			bool useOldMotionMode,
			SDLKey pepperKey)
				throw(int, std::string);
    /*  See base class.

	'windowManagerCaption' must contain the title to give to the window.

	The game will start at level 'initLevelNumber'.
	This must be a positive integer.

	'useOldMotionMode' determines if the pre v1.7.1 player motion
	should be used, or not.
	In the new motion system, when the requested direction is impossible,
	the last moving direction is attempted.
	This can be useful to turn corners automatically.
	In the old system, when the requested direction is impossible,
	the player does not move.

	Throws a std::string or an integer code if an error occurs.
    */

    virtual ~BurgerSpaceEngine();
    /*  Nothing interesting.
    */

    virtual void processKey(SDLKey keysym, bool pressed);
    /*  Inherited.
    */

    virtual bool tick();
    /*  Inherited.
    */

private:

    ///////////////////////////////////////////////////////////////////////////
    //
    //  LOCAL TYPES, CLASSES AND CONSTANTS
    //
    //


    struct IntPair
    {
	int first, second;
    };

    
    struct IntQuad
    {
	int first, second, third, fourth;
    };


    class Level
    {
    public:
	flatzebra::Couple sizeInTiles;
	flatzebra::Couple sizeInPixels;
	flatzebra::Couple positionInPixels;

	Level();
	~Level();

	void init(int no, int nCols, int nRows, flatzebra::Couple posInPixels);
	void setLevelNo(int no);
	int  getLevelNo() const;
	void setTileMatrixEntry(int colNo, int rowNo,
				XPM xpm, SDL_Surface *pixmap);
	SDL_Surface **getTileMatrixRow(int rowNo);
	XPM getXPMAtPixel(flatzebra::Couple pos) const;

    private:
	int levelNo;
	XPM *xpmMatrix;   // array of XPM pointers
	SDL_Surface **tileMatrix;  // array of X11 pixmaps
    };


    class IngInit
    {
    public:
	enum IngType
	{
	    BOTTOM_BUN, MEAT, LETTUCE, RED_STUFF, YELLOW_STUFF, TOP_BUN

	    /*	The red stuff is a slice of tomato, and the yellow stuff
		is cheese.  This was not known at the time when this
		enumeration was defined...
	    */
	};

	int xInitTile, yInitTile, yTargetTile, rank;
	IngType type;
    };


    ///////////////////////////////////////////////////////////////////////////
    //
    //  DATA MEMBERS
    //
    //

    static const char
	*levelDescriptor1[],
	*levelDescriptor2[],
	*levelDescriptor3[],
	*levelDescriptor4[],
	*levelDescriptor5[],
	*levelDescriptor6[];

    static const char **levelDescriptorTable[];

    static const IngInit
	tableIngredientsLevel1[],
	tableIngredientsLevel2[],
	tableIngredientsLevel3[],
	tableIngredientsLevel4[],
	tableIngredientsLevel5[],
	tableIngredientsLevel6[];
    
    static const IngInit *tableOfTablesOfIngredientsLevel[];

    static const IntQuad enemyStartingHeights[];
    static const IntPair playerStartingPos[];


    int initLevelNo;
    int cumulLevelNo;

    bool paused;
    unsigned long tickCount;

    SDL_Surface *tilePixmaps[5];


    /*  PLAYER:
    */
    flatzebra::Couple initPlayerPos;  // initial player position in a level
    flatzebra::PixmapArray playerPA;
    flatzebra::Sprite *playerSprite;
    int lastPlayerDirection;
    bool oldMotionMode;

    flatzebra::PixmapArray pepperPA;
    flatzebra::SpriteList  pepperSprites;


    /*  ENEMIES:
    */
    unsigned long timeForNewEnemy;
    		// tick count at which a new enemy must be created;
		// 0 means none

    flatzebra::PixmapArray eggPA;
    flatzebra::PixmapArray hotdogPA;
    flatzebra::PixmapArray picklePA;
    flatzebra::SpriteList  enemySprites;


    /*  INGREDIENTS:
    */
    flatzebra::PixmapArray topBunPA;
    flatzebra::PixmapArray lettucePA;
    flatzebra::PixmapArray meatPA;
    flatzebra::PixmapArray redStuffPA;
    flatzebra::PixmapArray yellowStuffPA;
    flatzebra::PixmapArray bottomBunPA;
    IngredientSprite::List ingredientSprites;  // owns the contained objects
    IngredientGroup::List  ingredientGroups;
				    // contained objects must come from 'new'


    /*  TREATS (icecream, etc, that the player eats to get a pepper):
    */
    flatzebra::PixmapArray treatPA;
    flatzebra::SpriteList treatSprites;
    int timeForTreat;


    /*  DIGITS:
    */
    flatzebra::PixmapArray digitPA;
    flatzebra::SpriteList scoreSprites;


    /*  KEYBOARD COMMANDS:
    */
    flatzebra::KeyState startKS;
    flatzebra::KeyState quitKS;
    ChefController theChefController;
    flatzebra::KeyState pauseKS;
    flatzebra::KeyState saveGameKS;
    flatzebra::KeyState loadGameKS;
    SDLKey lastKeyPressed;


    int    numHamburgersToDo;
    long   thePeakScore;  // player's best score yet in this game

    long   theScore;      // player's score in points
    flatzebra::Couple scoreAreaPos;
    flatzebra::Couple scoreAreaSize;

    bool   celebrationMode;  // used when player has just won the level

    int    numLives;  // number of player lives left
    flatzebra::Couple numLivesAreaPos;
    flatzebra::Couple numLivesAreaSize;

    int    numAvailablePeppers;  // number of pepper shots available to player
    flatzebra::Couple numAvailablePeppersAreaPos;
    flatzebra::Couple numAvailablePeppersAreaSize;

    flatzebra::Couple levelNoAreaPos;
    flatzebra::Couple levelNoAreaSize;

    Level theCurrentLevel;

    bool inQuitDialog;
    bool inSaveDialog;
    bool inLoadDialog;

    static int savedGameFormatVersion;

    /*  SOUND EFFECTS:
    */
    flatzebra::SoundMixer *theSoundMixer;  // see method playSoundEffect()
    bool useSound;
    flatzebra::SoundMixer::Chunk ingredientBouncesSound;
    flatzebra::SoundMixer::Chunk ingredientInPlateSound;
    flatzebra::SoundMixer::Chunk ingredientFallsSound;
    flatzebra::SoundMixer::Chunk ingredientLoweredSound;
    flatzebra::SoundMixer::Chunk enemyCatchesChefSound;
    flatzebra::SoundMixer::Chunk enemyParalyzedSound;
    flatzebra::SoundMixer::Chunk enemySmashedSound;
    flatzebra::SoundMixer::Chunk chefThrowsPepperSound;
    flatzebra::SoundMixer::Chunk chefGetsTreatSound;
    flatzebra::SoundMixer::Chunk chefShootsBlanksSound;
    flatzebra::SoundMixer::Chunk newGameStartsSound;
    flatzebra::SoundMixer::Chunk levelFinishedSound;
    flatzebra::SoundMixer::Chunk treatAppearsSound;
    flatzebra::SoundMixer::Chunk treatDisappearsSound;


    ///////////////////////////////////////////////////////////////////////////
    //
    //  IMPLEMENTATION FUNCTIONS
    //
    //
    void chooseDirectionAmongMany(bool directions[4]) const;
    int  chooseDirectionTowardTarget(
			    flatzebra::Couple startPos, flatzebra::Couple targetPos, int speedFactor,
			    const bool allowedDirections[4]) const;
    void avoidEnemies(bool desiredDirs[4]) const;
    void getPlayerDesiredDirections(bool desiredDirs[4]) const;

    bool animatePlayer();
    void animateAutomaticCharacters();
    void detectCollisions();
    void restoreBackground();
    void drawSprites();

    void putSprite(const flatzebra::Sprite &s);
    void showInstructions();
    void initGameParameters();
    void initNextLevel(int levelNo = 0) throw(int);
    void startNewLife();
    void animateTemporarySprites(flatzebra::SpriteList &slist) const;
    void givePlayerPepper();
    void makePlayerWin();
    void makePlayerDie();
    void releaseAllCarriedEnemies();
    void detectEnemyCollisions(flatzebra::SpriteList &slist);
    void detectCollisionBetweenIngredientGroupAndEnemyList(
	    const IngredientGroup &aGroup, flatzebra::SpriteList &enemies);
    bool ingredientGroupCollidesWithSprite(
			const flatzebra::Couple groupPos, const flatzebra::Couple groupSize,
			const flatzebra::Sprite &s) const;
    void loadPixmaps() throw(flatzebra::PixmapLoadError);
    void loadLevel(int levelNo) throw(std::string);
    void displayErrorMessage(const std::string &msg) throw();
    void initializeSprites() throw(flatzebra::PixmapLoadError);
    void initializeMisc() throw(std::string);
    void killSpritesInList(flatzebra::SpriteList &sl);
    IngredientGroup *findIngredientGroupRightBelow(
					const IngredientGroup &upperGroup);
    bool isIngredientSpriteOnFloor(const IngredientSprite &s) const;
    bool spriteTouchesIngredientGroup(
			    const flatzebra::Sprite &s, const IngredientGroup &g) const;
    size_t carryEnemies(IngredientGroup &g);
    size_t carryEnemiesInList(IngredientGroup &g, flatzebra::SpriteList &slist);
    size_t releaseCarriedEnemies(IngredientGroup &g);
    void createScoreSprites(long n, flatzebra::Couple center);
    void moveEnemyList(flatzebra::SpriteList &slist, int speedFactor);
    flatzebra::Couple getDistanceToPerfectPos(const flatzebra::Sprite &s) const;
    flatzebra::Couple determineAllowedDirections(const flatzebra::Sprite &s,
				    int speedFactor, int tolerance,
				    bool allowedDirections[4]) const;
    flatzebra::Couple attemptMove(const flatzebra::Sprite &s, bool attemptLeft, bool attemptRight,
			bool attemptUp, bool attemptDown,
			int speedFactor) const;
    bool positionAllowsLeftMovement(flatzebra::Couple pos, flatzebra::Couple size) const;
    bool positionAllowsRightMovement(flatzebra::Couple pos, flatzebra::Couple size) const;
    bool spriteBottomCenterIsOnLadder(const flatzebra::Sprite &s) const;
    bool spriteBottomCenterIsOverLadder(const flatzebra::Sprite &s) const;
    void drawComplexEnemySprites(const flatzebra::SpriteList &slist, int oscBit);
    void addToScore(long n);
    void addToNumLives(int n);
    void initTimeForTreat();
    void displayPauseMessage(bool display);
    void displayStartMessage(bool display);
    void displayMessage(int row, const char *msg);
    void playSoundEffect(flatzebra::SoundMixer::Chunk &wb);

    void drawQuitDialog();
    bool doQuitDialog();

    void drawSaveDialog();
    void drawLoadDialog();
    int  getSlotNumber();
    void doSaveDialog();
    void doLoadDialog();
    void showDialogBox(const std::string &msg);

    std::string serialize(flatzebra::Couple c) const;
    std::string serialize(const flatzebra::Sprite &s) const;
    std::string serialize(const EnemySprite &s) const;
    int encodePixmapArrayPointer(const flatzebra::PixmapArray *ptr) const;
    const flatzebra::PixmapArray *decodePixmapArrayCode(int) const;
    void serializeSpriteList(std::ostream &out,
				    const flatzebra::SpriteList &list) const;
    void serializeEnemySpriteList(std::ostream &out,
				    const flatzebra::SpriteList &list) const;
    std::string serialize(const IngredientSprite &is) const;
    void saveGame(std::ostream &out, int slotNum);
    bool deserialize(std::istream &in, flatzebra::Couple &c) const;
    flatzebra::Sprite *deserializeSprite(std::istream &in,
					bool enemy = false) const;
    bool deserializeSpriteList(std::istream &in,
				flatzebra::SpriteList &list,
				bool enemies = false) const;
    IngredientSprite *deserializeIngredientSprite(std::istream &in,
						IngredientGroup *ig) const;
    int loadGame(std::istream &in, int slotNum);
    static std::string getSavedGamesDir();
    static std::string formSavedGameFilename(int slotNum);
    void showSlotDirectory();


    /*	Forbidden operations:
    */
    BurgerSpaceEngine(const BurgerSpaceEngine &);
    BurgerSpaceEngine &operator = (const BurgerSpaceEngine &);
};


#endif  /* _H_BurgerSpaceEngine */
