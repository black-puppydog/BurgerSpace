/*  $Id: BurgerSpaceServer.cpp,v 1.3.2.53 2010/05/16 02:25:48 sarrazip Exp $
    BurgerSpaceServer.cpp - Main engine

    burgerspace - A hamburger-smashing video game.
    Copyright (C) 2001-2008 Pierre Sarrazin <http://sarrazip.com/>

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

#include "BurgerSpaceServer.h"

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

#include "pixmaps.h"

#include "LevelSet.h"
#include "Helpers.h"
#include "ParsedLevelSet.h"

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
    FLOOR_POS = TILE_SIDE - 6,  // position of the floor's surface, in a tile
    INGREDIENT_FALL_SPEED = 8,
    PLAYER_SPEED_FACTOR = 4,
    ENEMY_SPEED_FACTOR = 3,
    SCORE_TTL = 20,
    NUM_INIT_LIVES = 3,
    CORNER_TURN_TOLERANCE = 8;


///////////////////////////////////////////////////////////////////////////////
//
// UTILITY FUNCTIONS
//


inline
ostream &
operator << (ostream &out, const Couple &c)
{
    return out << '(' << c.x << ", " << c.y << ')';
}


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
deleteIngredientGroup(IngredientGroup *p)
{
    delete p;
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

///////////////////////////////////////////////////////////////////////////////


BurgerSpaceServer::BurgerSpaceServer(int initLevelNumber,
                                    bool _oldMotionMode) throw(int, string)
  : theScreenSizeInPixels(SCREEN_WIDTH_IN_PIXELS, SCREEN_HEIGHT_IN_PIXELS),

    initLevelNo(1),
    cumulLevelNo(1),
    currentRequest(NO_REQUEST),
    paused(false),
    tickCount(0),

    initPlayerPos(),
    playerPA(18),
    playerSprite(NULL),
    lastPlayerDirection(-1),
    oldMotionMode(_oldMotionMode),
    chefWantsToShootPepper(false),

    pepperPA(2),
    pepperSprites(),

    timeForNewEnemy(0),

    eggPA(14),
    hotdogPA(14),
    picklePA(14),
    enemySprites(),
    enemyTypeCounter(0),
    requestedEnemyDirection(-1),

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

    numHamburgersToDo(0),
    thePeakScore(0),

    theScore(0),

    celebrationMode(false),

    numLives(0),
    numAvailablePeppers(0),
    theCurrentLevel()
{
    assert(initLevelNumber >= 1);
    initLevelNo = initLevelNumber;
    cumulLevelNo = initLevelNo;

    initLevelSet();
//    initNextLevel(cumulLevelNo);

    desiredDirs[RIGHT] =
    desiredDirs[UP] =
    desiredDirs[LEFT] =
    desiredDirs[DOWN] = false;
    chefWantsToShootPepper = false;
}

void BurgerSpaceServer::initLevelSet()
{
//     try{
        levelSet=new ParsedLevelSet();
// 	cout << "loading of level descriptions successful" << endl;
//     } catch(...)
//     {
// 	cout << "loading of level file failed, falling back to standard levelset." << endl;
// 	levelSet = new SimpleLevelSetImplementation();
//     }
}


void
BurgerSpaceServer::finishInit()
{
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
BurgerSpaceServer::displayErrorMessage(const string &msg) throw()
{
    cerr << msg << endl;
}


BurgerSpaceServer::~BurgerSpaceServer()
{
    /*  Here, we call 'delete' directly to destroy the player sprite,
        because deleteSprite() would try to call notifySpriteDeletion(),
        which is a virtual function. This would be an attempt at calling
        a virtual function from a destructor (even if indirectly), which
        is forbidden.  It would cause a crash under GNU/Linux with this
        error message:
            pure virtual method called
            terminate called without an active exception
        Anyway, since the server is being shutdown, it presumably has
        no clients anymore, and so it would be useless to trigger the
        sprite deletion notification code.
    */
    delete playerSprite;
}


void
BurgerSpaceServer::initializeSprites() throw(PixmapLoadError)
/*  Initializes the sprites that appear at the beginning of a level,
    like the player and the ingredients.

    Assumes that 'playerSprite' is a valid pointer (may be null).

    Assumes that all pixmap arrays have been loaded.
*/
{
    /*        Player:
    */
    deleteSprite(playerSprite);
    Couple playerSize = playerPA.getImageSize();
    assert(playerSize.isNonZero());
    int sx = theCurrentLevelDescription.playerStartingPosition.first;
    assert(sx != 0);
    int sy = theCurrentLevelDescription.playerStartingPosition.second;
    assert(sy != 0);
    initPlayerPos = theCurrentLevel.positionInPixels +
                Couple(sx * TILE_SIDE + 1, sy * TILE_SIDE - playerSize.y);
    playerSprite = new Sprite(playerPA, initPlayerPos,
                                    Couple(0, 0), Couple(0, 0),
                                    Couple(4, 4), playerSize - Couple(8, 8));
    playerSprite->currentPixmapIndex = size_t(-1);  // invisible at first


    /*        Ingredients:
    */
    deleteSprites(ingredientSprites);
    for_each(ingredientGroups.begin(), ingredientGroups.end(),
                                            deleteIngredientGroup);
    ingredientGroups.clear();

    const Couple size = bottomBunPA.getImageSize();
    vector<IngInit> tableIngredients =
                theCurrentLevelDescription.tableOfIngredients;
//     assert(tableIngredients != NULL);

    numHamburgersToDo = 0;

    for (size_t j = 0; j<tableIngredients.size(); j++)
    {
        const IngInit &ii = tableIngredients[j];
        int yTarget = theCurrentLevel.positionInPixels.y +
                                ii.yTargetTile * TILE_SIDE - size.y * ii.rank;
        PixmapArray *pm = NULL;
// 	cout << "adding ingredient: " << ii.type << endl;
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
BurgerSpaceServer::initializeMisc() throw(string)
/*  Initializes things that need to be initialized once, but not at the
    beginning of each level.

    Throws an error message in a 'string' if an error occurs.
*/
{
    initTimeForTreat();
}


void
BurgerSpaceServer::showInstructions()
{
    displayStartMessage(true);
}


void
BurgerSpaceServer::initGameParameters()
{
    loadLevel(initLevelNo);

    celebrationMode = false;
    theScore = 0;
    numAvailablePeppers = 5;
    numLives = 0;

    initTimeForTreat();
}


///////////////////////////////////////////////////////////////////////////////


//virtual
void
BurgerSpaceServer::startNewGame()
{
    if (numLives == 0)  // only revelant in demo mode
        currentRequest = START_GAME_REQUEST;
}


bool
BurgerSpaceServer::isPaused() const
{
    return paused;
}


//virtual
void
BurgerSpaceServer::pauseGame()
{
    currentRequest = PAUSE_REQUEST;
}


//virtual
void
BurgerSpaceServer::resumeGame()
{
    currentRequest = RESUME_REQUEST;
}


//virtual
void
BurgerSpaceServer::setChefRequest(const bool desiredDirections[4], bool shootPepper)
{
    desiredDirs[RIGHT] = desiredDirections[RIGHT];
    desiredDirs[UP] = desiredDirections[UP];
    desiredDirs[LEFT] = desiredDirections[LEFT];
    desiredDirs[DOWN] = desiredDirections[DOWN];
    chefWantsToShootPepper = shootPepper;

    if (0) cout << "CHEF: "
            << desiredDirs[RIGHT] << " "
            << desiredDirs[UP] << " "
            << desiredDirs[LEFT] << " "
            << desiredDirs[DOWN] << ", "
            << chefWantsToShootPepper << endl;
}


/*virtual*/
void
BurgerSpaceServer::setEnemyRequest(const bool desiredDirections[4])
{
    // CONVENTION: when one of the enemies is controlled by the player,
    // that enemy is an egg, and only one egg at a time can exist.

    if (0) cout << "ENEMY: "
            << desiredDirections[RIGHT] << " "
            << desiredDirections[UP] << " "
            << desiredDirections[LEFT] << " "
            << desiredDirections[DOWN] << endl;

    // Search for the egg:
    SpriteList::const_iterator it;
    for (it = enemySprites.begin(); it != enemySprites.end(); it++)
        if ((*it)->getPixmapArray() == &eggPA)
        {
            // Get one of the desired directions and
            // save it for moveEnemyList().
            for (int k = 0; k < 4; k++)
                if (desiredDirections[k])
                {
                    requestedEnemyDirection = k;
                    return;
                }
        }

    requestedEnemyDirection = -1;
}


/*virtual*/
bool
BurgerSpaceServer::update()
{
    if (numLives == 0)  // if in demo mode
    {
        if (currentRequest == START_GAME_REQUEST)
        {
            playSoundEffect(newGameStartsSound);

            initGameParameters();
            startNewLife();
            initNextLevel(initLevelNo);

            theScore = 0;
            thePeakScore = 0;
            numLives = NUM_INIT_LIVES;

            currentRequest = (paused ? RESUME_REQUEST : NO_REQUEST);
        }
    }

    if (paused)
    {
        if (currentRequest == RESUME_REQUEST)
        {
            changePauseState(false);  // changes 'paused'
            currentRequest = NO_REQUEST;
        }
        else if (currentRequest == PAUSE_REQUEST)  // tolerate redundant request
            currentRequest = NO_REQUEST;

        drawSprites();
    }

    if (!paused)  // paused may have changed just now per a RESUME_REQUEST
    {
        if (currentRequest == RESUME_REQUEST)  // tolerate redundant request
            currentRequest = NO_REQUEST;

        ++tickCount;

        if (currentRequest == PAUSE_REQUEST)
        {
            changePauseState(true);
            currentRequest = NO_REQUEST;
        }
        else
        {
            if (!animatePlayer())
                return false;

            animateAutomaticCharacters();
        }

        drawSprites();

        if (!paused)
            detectCollisions();
    }

    assert(currentRequest == NO_REQUEST);  // any request must have been processed by this method
    return true;
}


Couple
BurgerSpaceServer::getDistanceToPerfectPos(const Sprite &s) const
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
BurgerSpaceServer::putSprite(const Sprite &s)
{
    const PixmapArray *pa = s.getPixmapArray();
    SpriteType st = NO_SPRITE;

    if (pa == &playerPA) st = CHEF;
    if (pa == &pepperPA) st = PEPPER;
    if (pa == &eggPA) st = EGG;
    if (pa == &hotdogPA) st = HOTDOG;
    if (pa == &picklePA) st = PICKLE;
    if (pa == &topBunPA) st = TOP_BUN;
    if (pa == &lettucePA) st = LETTUCE;
    if (pa == &meatPA) st = MEAT;
    if (pa == &redStuffPA) st = RED_STUFF;
    if (pa == &yellowStuffPA) st = YELLOW_STUFF;
    if (pa == &bottomBunPA) st = BOTTOM_BUN;
    if (pa == &treatPA) st = TREAT;
    if (pa == &digitPA) st = DIGIT;
    assert(st != NO_SPRITE);

    //cout << "putSprite(" << st << ", " << s.getPos() << ", " << s.currentPixmapIndex << ")" << endl;

    updateSprite(s.getId(), st, s.getPos(), s.currentPixmapIndex);  // virtual
}


void
BurgerSpaceServer::initNextLevel(int levelNo /*= 0*/) throw(int)
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
BurgerSpaceServer::startNewLife()
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
BurgerSpaceServer::chooseDirectionAmongMany(bool directions[4]) const
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
BurgerSpaceServer::chooseDirectionTowardTarget(
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
BurgerSpaceServer::avoidEnemies(bool desiredDirs[4]) const
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


// All pause/resume state changes must be done with a call to this method.
void
BurgerSpaceServer::changePauseState(bool newPauseState)
{
    paused = newPauseState;
}


bool
BurgerSpaceServer::animatePlayer()
/*  Returns true if the game must continue, or false to have it stop.
*/
{
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
                if (numLives == 0)
                    playerSprite->currentPixmapIndex = size_t(-1);  // stop displaying chef
            }
        }

        return true;
    }


    Couple &playerPos = playerSprite->getPos();
    Couple &playerSpeed = playerSprite->getSpeed();


    /*  Shoot if requested:
    */
    static const bool infinitePepper = (getenv("INFINITEPEPPER") != NULL);
    if (chefWantsToShootPepper)
    {
        chefWantsToShootPepper = false;

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


    /*        Apply the speed to the position:
    */
    assert(playerSpeed.isNonZero());
    playerPos += playerSpeed;
    lastPlayerDirection = dir;

    return true;
}


Couple
BurgerSpaceServer::determineAllowedDirections(const Sprite &s,
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
BurgerSpaceServer::attemptMove(const Sprite &s,
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
BurgerSpaceServer::positionAllowsLeftMovement(Couple pos, Couple size) const
/*  Determines if the proposed position 'pos' for a sprite that would
    have the given 'size' would allow movement to the left.
    Looks at the tiles at the given position, which must be floor tiles
    or floor-and-ladder tiles.
*/
{
    Couple posBotLeft = pos + Couple(0, size.y);
    if (posBotLeft.y % TILE_SIDE != 0)
        return false;

    const char **botLeftXPM = theCurrentLevel.getXPMAtPixel(posBotLeft);
    return (botLeftXPM == floor_xpm || botLeftXPM == floor_and_ladder_xpm);
}


bool
BurgerSpaceServer::positionAllowsRightMovement(Couple pos, Couple size) const
{
    Couple posBotRight = pos + size + Couple(-1, 0);
    if (posBotRight.y % TILE_SIDE != 0)
        return false;

    const char **botRightXPM = theCurrentLevel.getXPMAtPixel(posBotRight);
    return (botRightXPM == floor_xpm || botRightXPM == floor_and_ladder_xpm);
}


bool
BurgerSpaceServer::spriteBottomCenterIsOnLadder(const Sprite &s) const
/*  Determines if the sprite's bottom center touches a ladder.
    This method does NOT judge whether a sprite's horizontal position
    is close enough to a ladder.
    This method can be used to determine if a sprite can climb up a ladder.
*/
{
    const Couple &centerPos = s.getCenterPos();
    const Couple &lowerRightPos = s.getLowerRightPos();
    const char **botXPM = theCurrentLevel.getXPMAtPixel(
                                     Couple(centerPos.x, lowerRightPos.y - 1));
    return (botXPM == ladder_xpm || botXPM == floor_and_ladder_xpm);
}


bool
BurgerSpaceServer::spriteBottomCenterIsOverLadder(const Sprite &s) const
/*  Determines if the pixel UNDER the sprite's bottom center touches a ladder.
    This method does NOT judge whether a sprite's horizontal position
    is close enough to a ladder.
    This method can be used to determine if a sprite can climb down a ladder.
*/
{
    const Couple &centerPos = s.getCenterPos();
    const Couple &lowerRightPos = s.getLowerRightPos();
    const char **botXPM = theCurrentLevel.getXPMAtPixel(
                                     Couple(centerPos.x, lowerRightPos.y));
    return (botXPM == ladder_xpm || botXPM == floor_and_ladder_xpm);
}


void
BurgerSpaceServer::deleteSprite(Sprite *s) const
{
    if (s != NULL)
    {
        const_cast<BurgerSpaceServer *>(this)->notifySpriteDeletion(s->getId());
        delete s;
    }
}


void
BurgerSpaceServer::deleteSprites(SpriteList &sl) const
{
    for (SpriteList::iterator its = sl.begin(); its != sl.end(); its++)
    {
        Sprite *s = *its;
        assert(s != NULL);
        deleteSprite(s);
    }

    sl.clear();
    assert(sl.size() == 0);
}


void
BurgerSpaceServer::deleteSprites(IngredientSprite::List &sl) const
{
    for (IngredientSprite::List::iterator its = sl.begin(); its != sl.end(); its++)
    {
        Sprite *s = *its;
        assert(s != NULL);
        deleteSprite(s);
    }

    sl.clear();
    assert(sl.size() == 0);
}


void
BurgerSpaceServer::animateAutomaticCharacters()
{
    /*  Animate score sprites:
    */
    animateTemporarySprites(scoreSprites);


    /*  Everything freezes if the player is agonizing:
    */
    if (playerSprite->getTimeToLive() != 0)
        return;


    /*  If there is not the minimum number of enemies in the play,
        schedule the creation of another one some time in the near future.
    */
    size_t totalNumEnemies = enemySprites.size();
    size_t requiredNumEnemies = 6 + 3 * ((cumulLevelNo - 1) / levelSet->getNumLevels());
    if (totalNumEnemies < requiredNumEnemies && timeForNewEnemy == 0)
        timeForNewEnemy = tickCount + 40;

    static bool noEnemiesFlag = (getenv("NOENEMIES") != NULL);
    if (noEnemiesFlag)
        timeForNewEnemy = 0;

    if (tickCount >= timeForNewEnemy && timeForNewEnemy != 0)
    {
        timeForNewEnemy = 0;

        assert(theCurrentLevel.getLevelNo() >= 1);
        IntQuad sh = theCurrentLevelDescription.enemyStartingHeights;
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

        EnemyType type = chooseEnemyType();

        switch (type)
        {
            case ENEMY_EGG:
            {
                Sprite *s = new EnemySprite(eggPA, pos, speed,
                                            Couple(3, 3), size - Couple(6, 6));
                enemySprites.push_back(s);
            }
            break;

            case ENEMY_HOT_DOG:
            {
                Sprite *s = new EnemySprite(hotdogPA, pos, speed,
                                            Couple(3, 3), size - Couple(6, 6));
                enemySprites.push_back(s);
            }
            break;

            case ENEMY_PICKLE:
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
            deleteSprite(*it);
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
            deleteSprite(*it);
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


//virtual
EnemyType
BurgerSpaceServer::chooseEnemyType()
{
    return EnemyType(enemyTypeCounter++ % 3);
}


//virtual
bool
BurgerSpaceServer::isUserControlledEnemy(const Sprite &/*s*/) const
{
    return false;
}


IngredientGroup *
BurgerSpaceServer::findIngredientGroupRightBelow(
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
BurgerSpaceServer::isIngredientSpriteOnFloor(const IngredientSprite &s) const
{
    Couple size = s.getSize();
    Couple pos = s.getPos();
    Couple p(pos.x, pos.y + size.y - 6);
            /*  The -6 represents the fact that an ingredient is supposed
                to be a bit sunken in the floor.
            */
    if (p.y % TILE_SIDE != 0)
        return false;
    const char **xpm = theCurrentLevel.getXPMAtPixel(p);
    bool onFloor = (xpm == floor_xpm || xpm == floor_and_ladder_xpm);
    return onFloor;
}


size_t
BurgerSpaceServer::carryEnemies(IngredientGroup &g)
/*  Carries enemies that touch 'g'.  See carryEnemiesInList().
    Returns the number of sprites that are carried.
*/
{
    return carryEnemiesInList(g, enemySprites);
}


size_t
BurgerSpaceServer::carryEnemiesInList(
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
        const char **xpm = theCurrentLevel.getXPMAtPixel(lrp);
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
BurgerSpaceServer::releaseCarriedEnemies(IngredientGroup &g)
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
        enemy->setClimbingFromPlate(true);
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


inline
size_t
countNumDirections(const bool dirs[4])
{
    size_t count = 0;
    for (int k = 0; k < 4; ++k)
        if (dirs[k])
            ++count;
    return count;
}


void
BurgerSpaceServer::moveEnemyList(SpriteList &slist, int speedFactor)
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

        const PixmapArray *pa = s->getPixmapArray();

        // If enemy is dying:
        if (s->getTimeToDie() != 0)
        {
            if (s->decTimeToDie() != 0)
                continue;

            long score = 0;
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
            deleteSprite(s);
            *it = NULL;
            continue;
        }

        if (s->decTimeToLive() != 0)  // if enemy is paralyzed
            continue;


        /*  Determine the directions that are allowed.
        */
        bool allowedDirections[4];
        Couple delta = determineAllowedDirections(
                            *s,
                            speedFactor,
                            /*playerControlled ? CORNER_TURN_TOLERANCE :*/ speedFactor - 1,
                            allowedDirections);

        /*  Enemy sprites are created at a position to the left or right
            of the floor and ladder structure, so we have to make an exception
            to allow the left or right directions so that they can join
            the structure.
        */
        Couple pos = s->getPos() - theCurrentLevel.positionInPixels;
        switch (isPositionAtSideOfStructure(pos))
        {
            case -1:  // if at left
                allowedDirections[RIGHT] = true;
                break;
            case +1:  // if at right
                allowedDirections[LEFT] = true;
                break;
        }


        /*  Count the number of allowed directions.  If none are allowed,
            we assume that the enemy is below the structure because it has
            just been carried down into the plate.  Thus, we allow it to go up.
        */
        bool belowStructure = false;
        {
            size_t numDirectionsAllowed = 0;
            for (size_t j = 0; j < 4; j++)
                if (allowedDirections[j])
                    numDirectionsAllowed++;
            if (numDirectionsAllowed == 0)
            {
                belowStructure = true;
                allowedDirections[UP]++;
            }
        }


        int dir = -1;

        bool userControlled = isUserControlledEnemy(*s);
        if (!userControlled || s->isClimbingFromPlate() || numLives == 0)
        {
            // Automatic control.

            /*  If we are now at a point where more than one direction
                is allowed, then the enemy has finished climbing from
                a plate.
            */
            if (s->isClimbingFromPlate() && countNumDirections(allowedDirections) >= 2)
                s->setClimbingFromPlate(false);

            /*  If the current direction is allowed, then disallow the
                opposite direction.
            */
            assert(s->getSpeed().isNonZero());
            int currentDir = getDirectionFromCouple(s->getSpeed());

            assert(currentDir >= 0 && currentDir <= 3);
            if (allowedDirections[currentDir])
                allowedDirections[currentDir ^ 2] = false;


            const Couple spos = s->getPos();

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
        }
        else
        {
            int side = isPositionAtSideOfStructure(pos);
            if (side == 0)
                side = isPositionAtSideOfStructure(s->getLowerRightPos() - theCurrentLevel.positionInPixels);
                    // necessary for case where sprite comes from the right

            switch (side)
            {
                case -1:
                    dir = RIGHT;
                    break;
                case +1:
                    dir = LEFT;
                    break;
                default:
                    assert(requestedEnemyDirection >= -1 && requestedEnemyDirection <= 3);
                    if (belowStructure)
                        dir = UP;
                    else if (s->isClimbingFromPlate())
                    {
                        // belowStructure is false, but the enemy sprite is still
                        // climbing from the plate, so this means that belowStructure
                        // has just passed from true to false.
                        // We need to make the sprite go up one more time so that it
                        // will appear on a floor (instead of a step below a floor).
                        // Also, take the sprite out of the "climbing from plate" mode.

                        dir = rand() % 4;
                        while (!allowedDirections[dir])
                            dir = (dir + 1) % 4;

                        s->setClimbingFromPlate(false);
                    }
                    else if (requestedEnemyDirection != -1)
                    {
                        if (allowedDirections[requestedEnemyDirection])
                            dir = requestedEnemyDirection;
                        else
                        {
                            int lastDir = s->getLastDirection();
                            if (lastDir != -1 && allowedDirections[lastDir])
                                dir = lastDir;
                        }
                    }
            }
        }

        // Add a correction to get to a "perfect" position:
        if (dir == RIGHT || dir == LEFT)
            delta.x = 0;
        else if (dir == UP)
            delta.y = 0;
        else if (dir == DOWN)
            delta.y = 0;
        if (dir != -1)
            s->getPos() += delta;

        // Convert the direction (RIGHT/UP/LEFT/DOWN) into a speed couple:
        assert(dir >= -1 && dir <= 3);
        Couple speed = (dir == -1 ? Couple() : getCoupleFromDirection(dir, speedFactor));
        s->setSpeed(speed);
        s->addSpeedToPos();
        if (dir != -1)
            s->setLastDirection(dir);
    }

    removeNullElementsFromSpriteList(slist);
}


int
BurgerSpaceServer::isPositionAtSideOfStructure(Couple pos) const
{
    if (pos.x / TILE_SIDE < 3)
        return -1;  // yes, at left
    if (pos.x / TILE_SIDE >= theCurrentLevel.sizeInTiles.x - 3)
        return +1;  // yes, at right
    return 0;  // no
}


void
BurgerSpaceServer::animateTemporarySprites(SpriteList &slist) const
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
            deleteSprite(s);
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
BurgerSpaceServer::givePlayerPepper()
{
    numAvailablePeppers++;
}


void
BurgerSpaceServer::makePlayerWin()
{
    playSoundEffect(levelFinishedSound);

    playerSprite->setTimeToLive(80);
    releaseAllCarriedEnemies();

    celebrationMode = true;

    givePlayerPepper();
}


void
BurgerSpaceServer::makePlayerDie()
{
    playerSprite->setTimeToLive(60);
        // The player will agonize for this number of ticks.

    releaseAllCarriedEnemies();
}


void
BurgerSpaceServer::releaseAllCarriedEnemies()
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
BurgerSpaceServer::detectCollisions()
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
            deleteSprite(treat);

            playSoundEffect(chefGetsTreatSound);
            givePlayerPepper();
        }
    }

    removeNullElementsFromSpriteList(treatSprites);
}


void
BurgerSpaceServer::detectCollisionBetweenIngredientGroupAndEnemyList(
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
BurgerSpaceServer::detectEnemyCollisions(SpriteList &slist)
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

        static bool invincibleChef = (getenv("INVINCIBLECHEF") != NULL);
        if (!invincibleChef && numLives != 0 && s->collidesWithSprite(*playerSprite))
        {
            playSoundEffect(enemyCatchesChefSound);
            makePlayerDie();
        }
    }

    removeNullElementsFromSpriteList(slist);
}


bool
BurgerSpaceServer::ingredientGroupCollidesWithSprite(
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
BurgerSpaceServer::createScoreSprites(long n, Couple center)
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


static void
loadPixmap(const char **xpmData,
                SDL_Surface *&pixmap,
                Couple &pixmapSize) throw(PixmapLoadError)
{
    pixmapSize.zero();

    if (xpmData == NULL || xpmData[0] == NULL)
        throw PixmapLoadError(PixmapLoadError::INVALID_ARGS, NULL);

    pixmap = IMG_ReadXPMFromArray(const_cast<char **>(xpmData));
    if (pixmap == NULL)
        throw PixmapLoadError(PixmapLoadError::UNKNOWN, NULL);

    pixmapSize.x = pixmap->w;
    pixmapSize.y = pixmap->h;
}


static void
loadPixmap(const char **xpmData, PixmapArray &pa, size_t index)
                                                throw(PixmapLoadError)
{
    // Masks are not be relevant with SDL.

    SDL_Surface *pixmap;
    Couple size;
    loadPixmap(xpmData, pixmap, size);
    pa.setArrayElement(index, pixmap);
    pa.setImageSize(size);
}


void
BurgerSpaceServer::loadPixmaps() throw(PixmapLoadError)
{
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
BurgerSpaceServer::loadLevel(int levelNo) throw(string)
/*  Affects the data member 'theCurrentLevel', which must already be
    initialized to defined values.
    'levelNo' may exceed NUM_LEVELS.

    Throws an error message string if an error occurs.
*/
{

    levelNo = (levelNo - 1) % levelSet->getNumLevels() + 1;
    theCurrentLevelDescription = levelSet->getLevelDescription(levelNo);

    vector<string> levelDesc = theCurrentLevelDescription.LineStrings;
    assert(levelDesc[0].size()!=0);

    // Count the number of rows and columns:
    size_t numRows = 0, numColumns = 0;
    size_t rowNo;
    for (rowNo = 0; rowNo<levelDesc.size(); rowNo++)
    {
        size_t thisRowsLength = levelDesc[rowNo].size();
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

    string desc;
    desc.reserve(numRows * numColumns);

    for (rowNo = 0; rowNo<levelDesc.size(); rowNo++)
    {

        for (size_t colNo = 0; levelDesc[rowNo][colNo] != '\0'; colNo++)
        {
            const char **xpm = NULL;
            char c=levelDesc[rowNo][colNo];
            switch (c)
            {
                case 'e':  xpm = empty_xpm;  break;
                case 'f':  xpm = floor_xpm;  break;
                case 't':  xpm = floor_and_ladder_xpm; break;
                case 'l':  xpm = ladder_xpm; break;
                case 'p':  xpm = plate_xpm;  break;

                default:  throw string(__("Level character '")) +
                                    levelDesc[rowNo][colNo] + __("' unknown");
            }

            assert(xpm != NULL);
            theCurrentLevel.setTileMatrixEntry(colNo, rowNo, xpm, NULL);

            desc += levelDesc[rowNo][colNo];
        }

        assert(desc.length() == (rowNo + 1) * numColumns);
    }
    assert(desc.length() == numRows * numColumns);

    theCurrentLevel.setTextDescription(desc);  // save in case need to resend to client

    updateLevel(levelNo, numColumns, numRows, levelPos, desc);  // virtual
}


void
BurgerSpaceServer::sendLevelUpdate()
{
    updateLevel(theCurrentLevel.getLevelNo(),
                theCurrentLevel.sizeInTiles.x,
                theCurrentLevel.sizeInTiles.y,
                theCurrentLevel.positionInPixels,
                theCurrentLevel.getTextDescription());
}


void
BurgerSpaceServer::drawSprites()
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


    /*  Textual info:
    */
    updateScore(theScore, numLives, numAvailablePeppers, cumulLevelNo, false);
}


void
BurgerSpaceServer::sendScoreUpdate()
{
    updateScore(theScore, numLives, numAvailablePeppers, cumulLevelNo, true);
}


void
BurgerSpaceServer::drawComplexEnemySprites(const SpriteList &slist, int oscBit)
/*  'oscBit' is an "oscillating" bit.  See drawSprites().
*/
{
    for (SpriteList::const_iterator it = slist.begin();
                                            it != slist.end(); it++)
    {
        EnemySprite *enemy = dynamic_cast<EnemySprite *>(*it);
        assert(enemy != NULL);

        if (enemy->getDisappearanceTime() != 0)
        {
            // Set an invalid index in the sprite so that
            // it does not get displayed.
            enemy->currentPixmapIndex = size_t(-1);
        }
        else
        {
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
                static const int spriteNoFromDir[4] = { 6, 2, 4, 0 };
                int dir = (enemy->getSpeed().isZero() ? -1 : getDirectionFromCouple(enemy->getSpeed()));
                if (dir == -1)
                    if (enemy->getLastDirection() == -1)
                        spriteNo = -1;
                    else
                        spriteNo = spriteNoFromDir[enemy->getLastDirection()];
                else
                    spriteNo = spriteNoFromDir[dir] | oscBit;
            }
            else  // peppered:
            {
                spriteNo = 12 | oscBit;
            }

            if (spriteNo != -1)
                enemy->currentPixmapIndex = spriteNo;
        }

        putSprite(*enemy);
    }
}


void
BurgerSpaceServer::addToScore(long n)
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
BurgerSpaceServer::addToNumLives(int n)
{
    numLives += n;
}


void
BurgerSpaceServer::initTimeForTreat()
{
    timeForTreat = Rnd(20 * 20, 45 * 20);
}


void
BurgerSpaceServer::displayStartMessage(bool /*display*/)
/*  Displays the start message if 'display' is true, or erases the
    corresponding region if 'display' is false.
*/
{
}


///////////////////////////////////////////////////////////////////////////////
//
// SAVING AND LOADING GAMES
//


int BurgerSpaceServer::savedGameFormatVersion = 1;


string
BurgerSpaceServer::serialize(Couple c) const
{
    char temp[512];
    snprintf(temp, sizeof(temp), "%d %d", c.x, c.y);
    return temp;
}


int
BurgerSpaceServer::encodePixmapArrayPointer(const PixmapArray *ptr) const
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
BurgerSpaceServer::decodePixmapArrayCode(int code) const
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
BurgerSpaceServer::serialize(const Sprite &s) const
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
BurgerSpaceServer::serialize(const EnemySprite &es) const
{
    string s = serialize((Sprite &) es);
    char temp[512];
    snprintf(temp, sizeof(temp), " %d %d",
                        es.getDisappearanceTime(), es.getTimeToDie());
    return s + temp;
}


void
BurgerSpaceServer::serializeSpriteList(ostream &out,
                                        const SpriteList &list) const
// The sprites are serialized in the order in which they appear in 'list'.
{
    out << list.size() << "\n";
    for (SpriteList::const_iterator it = list.begin(); it != list.end(); it++)
        out << serialize(**it) << "\n";
}


void
BurgerSpaceServer::serializeEnemySpriteList(ostream &out,
                                            const SpriteList &list) const
// The sprites are serialized in the order in which they appear in 'list'.
{
    out << list.size() << "\n";
    for (SpriteList::const_iterator it = list.begin(); it != list.end(); it++)
        out << serialize(**it) << "\n";
}


string
BurgerSpaceServer::serialize(const IngredientSprite &is) const
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


bool
BurgerSpaceServer::deserialize(istream &in, Couple &c) const
{
    in >> c.x >> c.y;
    return in.good();
}


Sprite *
BurgerSpaceServer::deserializeSprite(istream &in, bool enemy) const
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
BurgerSpaceServer::deserializeIngredientSprite(istream &in,
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
BurgerSpaceServer::deserializeSpriteList(istream &in,
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


/*static*/
string
BurgerSpaceServer::getSavedGamesDir()
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
        cerr << PACKAGE << ": stat(" << home << "): " << strerror(e) << endl;
        return "";
    }
    if (!S_ISDIR(statbuf.st_mode))
    {
        cerr << PACKAGE << ": $HOME (" << home << ") is not a directory" << endl;
        return "";
    }

    string subdir = home + string("/") + "." + PACKAGE;
    if (stat(subdir.c_str(), &statbuf) != 0)
    {
        if (mkdir(subdir.c_str(), 0700) != 0)
        {
            int e = errno;
            cerr << PACKAGE << ": mkdir(" << subdir << "): " << strerror(e) << endl;
            return "";
        }
    }
    else if (!S_ISDIR(statbuf.st_mode))
    {
        cerr << PACKAGE << ": " << subdir << " is not a directory" << endl;
        return "";
    }

    if (access(subdir.c_str(), W_OK) != 0)
    {
        cerr << PACKAGE << ": " << subdir << "is not a writable directory" << endl;
        return "";
    }

    return subdir;
}


/*static*/
string
BurgerSpaceServer::formSavedGameFilename(int slotNum)
{
    char temp[512];
    snprintf(temp, sizeof(temp), "saved-game-%d.dat", slotNum);
    return temp;
}
