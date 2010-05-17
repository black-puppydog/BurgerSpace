/*  $Id: BurgerSpaceClient.cpp,v 1.1.2.72 2010/05/16 02:25:47 sarrazip Exp $
    BurgerSpaceClient.cpp - Game client

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

#include "BurgerSpaceClient.h"

#include "BurgerSpaceServer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "pixmaps.h"

using namespace std;
using namespace flatzebra;


BurgerSpaceClient::BurgerSpaceClient(Role _role,
                                        bool _showRole,
                                        bool _useSound,
                                        SDLKey _pepperKey,
                                        bool _fullScreen,
                                        bool _processActiveEvent)
  : GameEngine(Couple(SCREEN_WIDTH_IN_PIXELS, SCREEN_HEIGHT_IN_PIXELS),
                "BurgerSpace",
                _fullScreen,
                _processActiveEvent),
    serverInterface(NULL),
    role(_role),
    showRole(_showRole),
    roleAreaPos(),
    roleAreaSize(),
    theCurrentLevel(),
    playerPA(18),
    pepperPA(2),
    eggPA(14),
    hotdogPA(14),
    picklePA(14),
    topBunPA(4),
    lettucePA(4),
    meatPA(4),
    redStuffPA(4),
    yellowStuffPA(4),
    bottomBunPA(4),
    treatPA(3),
    digitPA(10),
    spriteTable(),
    theScore(0),
    scoreAreaPos(),
    scoreAreaSize(),
    numLives(0),
    numLivesAreaPos(),
    numLivesAreaSize(),
    numAvailablePeppers(0),
    numAvailablePeppersAreaPos(),
    numAvailablePeppersAreaSize(),
    pepperKey(_pepperKey),
    cumulLevelNo(0),
    levelNoAreaPos(),
    levelNoAreaSize(),
    currentPauseMessage(),
    gameMode(IN_GAME),
    theSoundMixer(NULL),
    useSound(_useSound),
    ingredientBouncesChunk(),
    ingredientInPlateChunk(),
    ingredientFallsChunk(),
    ingredientLoweredChunk(),
    enemyCatchesChefChunk(),
    enemyParalyzedChunk(),
    enemySmashedChunk(),
    chefThrowsPepperChunk(),
    chefGetsTreatChunk(),
    chefShootsBlanksChunk(),
    newGameStartsChunk(),
    levelFinishedChunk(),
    treatAppearsChunk(),
    treatDisappearsChunk()
{
    try
    {
        loadPixmaps();


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


        /*  Sound effects:
        */
        if (useSound)
        {
            try
            {
                theSoundMixer = new SoundMixer(16);  // may throw string
            }
            catch (const SoundMixer::Error &e)
            {
                return;
            }

            string d = getDir(PKGSOUNDDIR, "PKGSOUNDDIR");

            try
            {
                ingredientBouncesChunk.init(d + "ingredient-bounces.wav");
                ingredientInPlateChunk.init(d + "ingredient-in-plate.wav");
                ingredientFallsChunk  .init(d + "ingredient-falls.wav");
                ingredientLoweredChunk.init(d + "ingredient-lowered.wav");
                enemyCatchesChefChunk .init(d + "enemy-catches-chef.wav");
                enemyParalyzedChunk   .init(d + "enemy-paralyzed.wav");
                enemySmashedChunk     .init(d + "enemy-smashed.wav");
                chefThrowsPepperChunk .init(d + "chef-throws-pepper.wav");
                chefGetsTreatChunk    .init(d + "chef-gets-treat.wav");
                chefShootsBlanksChunk .init(d + "chef-shoots-blanks.wav");
                newGameStartsChunk    .init(d + "new-game-starts.wav");
                levelFinishedChunk    .init(d + "level-finished.wav");
                treatAppearsChunk     .init(d + "treat-appears.wav");
                treatDisappearsChunk  .init(d + "treat-disappears.wav");
            }
            catch (const SoundMixer::Error &e)
            {
                throw e.what();
            }
        }
    }
    catch (PixmapLoadError &e)
    {
        string msg = "Could not load pixmap " + e.getFilename();
        throw msg;
    }
}


/*virtual*/
BurgerSpaceClient::~BurgerSpaceClient()
{
    // Free surfaces loaded by loadPixmaps().
    for (size_t i = 0; i < sizeof(tilePixmaps) / sizeof(tilePixmaps[0]); ++i)
        SDL_FreeSurface(tilePixmaps[i]);
}


void
BurgerSpaceClient::setServer(BurgerSpaceServerInterface *server)
{
    serverInterface = server;
}


/*virtual*/
void
BurgerSpaceClient::processActivation(bool appActive)
{
    if (serverInterface != NULL)
    {
        if (!appActive && !serverInterface->isPaused())
        {
            serverInterface->pauseGame();
            restoreBackground();
            (void) serverInterface->update();  // let the server execute the pause request
            if (serverInterface->isPaused())  // remote server does not allow pause
            {
                currentPauseMessage = "PAUSED -- press P to resume";
            }
            draw();
        }
    }
}


void
BurgerSpaceClient::loadPixmaps() throw(PixmapLoadError)
{
    /*  Tiles:
    */
    Couple pmSize;
    loadPixmap(empty_xpm,            tilePixmaps[0], pmSize);
    loadPixmap(floor_xpm,            tilePixmaps[1], pmSize);
    loadPixmap(floor_and_ladder_xpm, tilePixmaps[2], pmSize);
    loadPixmap(ladder_xpm,           tilePixmaps[3], pmSize);
    loadPixmap(plate_xpm,            tilePixmaps[4], pmSize);

    // The loaded SDL_Surface objects must be freed by ~BurgerSpaceClient().


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
BurgerSpaceClient::runClientMode(Uint32 millisecondsPerFrame)
{
    assert(serverInterface != NULL);

    Controller controller(pepperKey);

    for (;;)
    {
        Uint32 frameEnd = SDL_GetTicks() + millisecondsPerFrame;

        SDLKey lastKeyPressed = SDLK_UNKNOWN;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    controller.check(event.key.keysym.sym, true);
                    lastKeyPressed = event.key.keysym.sym;
                    break;

                case SDL_KEYUP:
                    controller.check(event.key.keysym.sym, false);
                    break;

                case SDL_ACTIVEEVENT:
                    // If app is now inactive (iconified or lost focus),
                    // go wait for reactivation, i.e., stop calling tick().
                    // This avoids using the CPU while the app is not active.
                    // waitForReactivation() is inherited from flatzebra::GameEngine.
                    //
                    if (event.active.gain == 0 && !waitForReactivation())
                        return;  // wait ended with quit or with wait error
                    break;

                case SDL_QUIT:
                    return;
            }
        }


        if (controller.isFullScreenToggleRequested())
            setVideoMode(Couple(SCREEN_WIDTH_IN_PIXELS, SCREEN_HEIGHT_IN_PIXELS), !inFullScreenMode());  // ignore failure


        switch (gameMode)  // process dialog if one is open
        {
            case QUIT_DIALOG:
                if (!doQuitDialog(lastKeyPressed))  // if dialog asks to quit program
                    return;
                break;
            case SAVE_DIALOG:
                doSaveDialog(lastKeyPressed);
                break;
            case LOAD_DIALOG:
                doLoadDialog(lastKeyPressed);
                break;
            case IN_GAME:
                if (controller.isQuitRequested())
                {
                    gameMode = QUIT_DIALOG;
                    currentPauseMessage = "PAUSED";
                    serverInterface->pauseGame();
                }
                else if (!showRole && controller.isSaveRequested())  // only in stand-alone mode
                {
                    gameMode = SAVE_DIALOG;
                    currentPauseMessage = "PAUSED";
                    serverInterface->pauseGame();
                }
                else if (!showRole && controller.isLoadRequested())  // only in stand-alone mode
                {
                    gameMode = LOAD_DIALOG;
                    currentPauseMessage = "PAUSED";
                    serverInterface->pauseGame();
                }
                break;
        }


        if (gameMode == IN_GAME)  // if not in a dialog
        {
            if (numLives == 0 && controller.isStartRequested())
            {
                if (serverInterface->isPaused())  // if starting while paused
                    serverInterface->resumeGame();

                serverInterface->startNewGame();  // overrides resume request if any
            }
            else if (!serverInterface->isPaused() && controller.isPauseRequested())
            {
                currentPauseMessage = "PAUSED -- press P to resume";
                serverInterface->pauseGame();
            }
            else if (serverInterface->isPaused() && controller.isResumeRequested())
                serverInterface->resumeGame();
        }


        if (numLives > 0 && !serverInterface->isPaused())
        {
            // Get player direction and pepper shot.
            // Chef controller is reused for enemy role.

            bool desiredDirs[4];
            controller.getDesiredDirections(desiredDirs);
            if (0) cout << "CONTROL: "
                    << role << ": "
                    << desiredDirs[0] << " "
                    << desiredDirs[1] << " "
                    << desiredDirs[2] << " "
                    << desiredDirs[3] << ", "
                    << controller.isShotRequested() << endl;
            switch (role)
            {
                case ROLE_CHEF:
                    serverInterface->setChefRequest(desiredDirs, controller.isShotRequested());
                    break;
                case ROLE_ENEMY:
                    serverInterface->setEnemyRequest(desiredDirs);
                    break;
                case ROLE_SPECTATOR:
                    break;
            }
        }


        controller.update();

        // Empty the screen and redraw the setting tiles:
        restoreBackground();

        // Ask the server to send updates to the game state:
        if (!serverInterface->update())
            break;

        // Draw the current game state:
        draw();

        // If a dialog is open, draw it (on top of everything else):
        switch (gameMode)
        {
            case QUIT_DIALOG: drawQuitDialog(); break;
            case SAVE_DIALOG: drawSaveDialog(); break;
            case LOAD_DIALOG: drawLoadDialog(); break;
            case IN_GAME: break;
        }

        // Double buffering: switch pages:
        SDL_Flip(theSDLScreen);

        // Wait until next animation frame:
        Uint32 now = SDL_GetTicks();
        if (frameEnd > now)
            SDL_Delay(frameEnd - now);
    }
}


void
BurgerSpaceClient::restoreBackground()
{
    //cout << "restoreBackground: " << theCurrentLevel.sizeInTiles.x << " x " << theCurrentLevel.sizeInTiles.y << " tiles" << endl;
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


// Display levels are used by draw() to make sure that the chef
// appears over the enemies, which themselves must appear over
// the ingredients.
int
BurgerSpaceClient::getSpriteDisplayLevel(const Sprite &s) const
{
    const PixmapArray *pa = s.getPixmapArray();
    if (pa == NULL)
    {
        assert(false);
        return 0;
    }

    if (pa == &playerPA
        || pa == &pepperPA
        || pa == &treatPA
        || pa == &digitPA)
        return 2;

    if (pa == &eggPA
        || pa == &hotdogPA
        || pa == &picklePA)
        return 1;

    return 0;
}


void
BurgerSpaceClient::draw()
{
    const Couple fontdim = getFontDimensions();

    for (int level = 0; level <= 2; ++level)
        for (map<uint32_t, Sprite *>::const_iterator it = spriteTable.begin();
                                                    it != spriteTable.end(); ++it)
        {
            const Sprite *s = it->second;

            // Display sprite 's' if it is at the right display level
            // and it is not prohibited.
            //
            if (getSpriteDisplayLevel(*s) == level && s->currentPixmapIndex != size_t(-1))
                copySpritePixmap(*s, s->currentPixmapIndex, s->getPos());
        }

    char s[64];
    snprintf(s, sizeof(s), "%s:%10ld", "Score", theScore);
    writeString(s, scoreAreaPos);
    snprintf(s, sizeof(s), "%s:%3d", "Lives", numLives);
    writeString(s, numLivesAreaPos);
    snprintf(s, sizeof(s), "%s:%3d", "Peppers", numAvailablePeppers);
    writeString(s, numAvailablePeppersAreaPos);
    snprintf(s, sizeof(s), "%s:%3d", "Level", cumulLevelNo);
    writeString(s, levelNoAreaPos);
    if (showRole)
    {
        const char *as;
        switch (role)
        {
            case ROLE_SPECTATOR: as = "a spectator"; break;
            case ROLE_CHEF: as = "the chef"; break;
            case ROLE_ENEMY: as = "the egg"; break;
            default: assert(false); as = "X"; break;
        }
        snprintf(s, sizeof(s), "Client-server mode: you are %s", as);
        writeStringXCentered(s, levelNoAreaPos.y);
    }

    if (serverInterface->isPaused())
        displayMessage(5, currentPauseMessage.c_str());

    if (numLives == 0)  // if demo mode
    {
        displayMessage(0, "BurgerSpace " VERSION " - by Pierre Sarrazin");
        displayMessage(2, "Move with arrow keys - shoot pepper with Ctrl key");
        displayMessage(3, "SPACE to start, Escape to quit");
    }

    if (!showRole)  // if stand-alone mode
        displayMessage(8, "[P] pause   [S] save game   [L] load game  [F11] toggle full screen");
}


void
BurgerSpaceClient::displayMessage(int row, const char *msg)
{
    writeStringXCentered(msg, theScreenSizeInPixels.y + (row - 10) * getFontDimensions().y);
}


void
BurgerSpaceClient::showDialogBox(const string &msg)
{
    string blanks(
            "                                                                                ",
            msg.length() + 2 * 2);
    displayMessage(0, blanks.c_str());
    displayMessage(1, ("  " + msg + "  ").c_str());
    displayMessage(2, blanks.c_str());
    displayMessage(3, blanks.c_str());
}


void
BurgerSpaceClient::drawQuitDialog()
{
    showDialogBox("          QUIT: are you sure? (Y=yes, N=no)          ");
}


bool
BurgerSpaceClient::doQuitDialog(SDLKey lastKeyPressed)
{
    if (lastKeyPressed == SDLK_y)
        return false;  // quit program
    if (lastKeyPressed == SDLK_n)
    {
        currentPauseMessage = "Game not quit -- press P to resume";
        gameMode = IN_GAME;
    }
    return true;  // stay in program
}


void
BurgerSpaceClient::drawSaveDialog()
{
    showDialogBox("     SAVE GAME: which slot? (1-9, Escape=cancel)     ");
    showSlotDirectory();
}


int
BurgerSpaceClient::getSlotNumber(SDLKey lastKeyPressed)
/*  Returns -1 if no valid key pressed.
    Returns -2 if quit chosen.
    Returns 1..9 if slot chosen.
*/
{
    if (lastKeyPressed >= SDLK_1 && lastKeyPressed <= SDLK_9)
    {
        int slotNum = int(lastKeyPressed - SDLK_1) + 1;
        lastKeyPressed = SDLK_UNKNOWN;
        return slotNum;
    }

    if (lastKeyPressed == SDLK_ESCAPE)
    {
        lastKeyPressed = SDLK_UNKNOWN;
        return -2;
    }

    return -1;
}


/*static*/
string
BurgerSpaceClient::getSavedGamesDir()
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
        cerr << PROGRAM << ": stat(" << home << "): " << strerror(e) << endl;
        return "";
    }
    if (!S_ISDIR(statbuf.st_mode))
    {
        cerr << PROGRAM << ": $HOME (" << home << ") is not a directory\n";
        return "";
    }

    string subdir = home + string("/") + "." + PACKAGE;
    if (stat(subdir.c_str(), &statbuf) != 0)
    {
        if (mkdir(subdir.c_str(), 0700) != 0)
        {
            int e = errno;
            cerr << PROGRAM << ": mkdir(" << subdir << "): " << strerror(e) << endl;
            return "";
        }
    }
    else if (!S_ISDIR(statbuf.st_mode))
    {
        cerr << PROGRAM << ": " << subdir << " is not a directory\n";
        return "";
    }

    if (access(subdir.c_str(), W_OK) != 0)
    {
        cerr << PROGRAM << ": " << subdir << " is not a writable directory\n";
        return "";
    }

    return subdir;
}


/*static*/
string
BurgerSpaceClient::formSavedGameFilename(int slotNum)
{
    char temp[512];
    snprintf(temp, sizeof(temp), "saved-game-%d.dat", slotNum);
    return temp;
}


void
BurgerSpaceClient::showSlotDirectory()
{
    int fontHeight = getFontDimensions().y;
    int y = 11 * fontHeight;
    writeStringXCentered("                           ", y += fontHeight);
    writeStringXCentered("  No  Save Time            ", y += fontHeight);
    writeStringXCentered("  --  -------------------  ", y += fontHeight);

    string dirname = getSavedGamesDir();

    // Scan the saved games directory and build a table of available games:
    for (size_t slotNum = 1; slotNum <= 9; slotNum++)
    {
        char slotDesc[32], fileTime[32];
        string filename = dirname + "/" + formSavedGameFilename(slotNum);
        ifstream file(filename.c_str());
        if (!file)
            snprintf(fileTime, sizeof(fileTime), "(empty)");
        else
        {
            struct stat statbuf;
            if (stat(filename.c_str(), &statbuf) != 0)
                snprintf(fileTime, sizeof(fileTime), "(empty)");
            else
            {
                const struct tm t = *localtime(&statbuf.st_mtime);
                strftime(fileTime, sizeof(fileTime), "%Y-%m-%d %H:%M:%S", &t);
            }
        }
        snprintf(slotDesc, sizeof(slotDesc), "   %u  %-19s  ", slotNum, fileTime);
        writeStringXCentered(string(slotDesc), y += fontHeight);
    }

    writeStringXCentered("  --  -------------------  ", y += fontHeight);
    writeStringXCentered("                           ", y += fontHeight);
}


void
BurgerSpaceClient::doSaveDialog(SDLKey lastKeyPressed)
{
    drawSaveDialog();

    int slotNum = getSlotNumber(lastKeyPressed);
    if (slotNum == -1)
        return;

    stringstream msg;
    if (slotNum > 0)  // if valid slot number chosen
    {
        string dir = getSavedGamesDir();
        if (dir.empty())
        {
            msg << "No usable directory for saved games";
        }
        else
        {
            string filename = dir + "/" + formSavedGameFilename(slotNum);
            ofstream out(filename.c_str());
            if (!out)
            {
                int e = errno;
                msg << "Failed to create slot " << slotNum << " (error #" << e << ")";
                cerr << PACKAGE << ": failed to write to " << filename << ": " << strerror(e) << endl;
            }
            else
            {
                if (serverInterface->saveGame(out))
                    msg << "Game saved in slot " << slotNum;
                else
                    msg << "Server interface failed to save";
            }
        }
    }
    else if (slotNum == -2)  // if cancel chosen
    {
        lastKeyPressed = SDLK_UNKNOWN;
        msg << "Game NOT saved";
    }

    currentPauseMessage = msg.str() + " -- press P to resume";
    gameMode = IN_GAME;
}


void
BurgerSpaceClient::drawLoadDialog()
{
    showDialogBox("     LOAD GAME: which slot? (1-9, Escape=cancel)     ");
    showSlotDirectory();
}


void
BurgerSpaceClient::doLoadDialog(SDLKey lastKeyPressed)
{
    drawLoadDialog();

    int slotNum = getSlotNumber(lastKeyPressed);
    if (slotNum == -1)
        return;

    stringstream msg;
    if (slotNum > 0)  // if valid slot number chosen
    {
        string dir = getSavedGamesDir();
        if (dir.empty())
        {
            msg << "No usable directory for saved games";
        }
        else
        {
            string filename = dir + "/" + formSavedGameFilename(slotNum);
            ifstream in(filename.c_str());
            if (!in)
            {
                msg << "No game saved in slot " << slotNum;
            }
            else
            {
                int errorCode = serverInterface->loadGame(in);
                if (errorCode != 0)
                    msg << "Failed to load from slot" << slotNum;
                else
                {
                    // Show the new game situation:
                    restoreBackground();
                    (void) serverInterface->update();  // let the server send a score update (with new number of lives)
                    draw();

                    msg << "Game loaded from slot " << slotNum;
                }
            }
        }
    }
    else if (slotNum == -2)  // if cancel chosen
    {
        lastKeyPressed = SDLK_UNKNOWN;
        msg << "No game loaded";
    }

    currentPauseMessage = msg.str() + " -- press P to resume";
    gameMode = IN_GAME;
}


void
BurgerSpaceClient::handleLevelUpdate(int levelNo, size_t numColumns, size_t numRows, flatzebra::Couple levelPos, const std::string &desc)
{
    //cout << "updateLevel(" << levelNo << ", " << numRows << ", " << numColumns << ", (" << levelPos.x << ", " << levelPos.y << "), '" << desc << "')" << endl;

    theCurrentLevel.init(levelNo, numColumns, numRows, levelPos);

    for (size_t rowNo = 0; rowNo < numRows; rowNo++)
    {
        for (size_t colNo = 0; colNo < numColumns; colNo++)
        {
            const char **xpm = NULL;
            SDL_Surface *pixmap = NULL;
            assert(rowNo * numColumns + colNo < desc.length());
            char levelChar = desc[rowNo * numColumns + colNo];
            switch (levelChar)
            {
                case 'e':  xpm = empty_xpm;  pixmap = tilePixmaps[0]; break;
                case 'f':  xpm = floor_xpm;  pixmap = tilePixmaps[1]; break;
                case 't':  xpm = floor_and_ladder_xpm;
                                             pixmap = tilePixmaps[2]; break;
                case 'l':  xpm = ladder_xpm; pixmap = tilePixmaps[3]; break;
                case 'p':  xpm = plate_xpm;  pixmap = tilePixmaps[4]; break;

                default:  throw string("Level character '") + levelChar + "' unknown";
            }

            assert(xpm != NULL);
            assert(pixmap);
            theCurrentLevel.setTileMatrixEntry(colNo, rowNo, xpm, pixmap);
        }
    }

    // Clear the sprite table to force redrawing of all sprites
    // upon a level change.
    spriteTable.clear();
}


void
BurgerSpaceClient::handleSpriteUpdate(uint32_t id, BurgerSpaceServer::SpriteType type, flatzebra::Couple pos, size_t pixmapIndex)
{
    //cerr << "BurgerSpaceClient::handleSpriteUpdate(" << id << ", " << type << ", (" << pos.x << ", " << pos.y << "), " << pixmapIndex << ")" << endl;

    PixmapArray *pa = NULL;

    switch (type)
    {
        case BurgerSpaceServer::CHEF: pa = &playerPA; break;
        case BurgerSpaceServer::PEPPER: pa = &pepperPA; break;
        case BurgerSpaceServer::EGG: pa = &eggPA; break;
        case BurgerSpaceServer::HOTDOG: pa = &hotdogPA; break;
        case BurgerSpaceServer::PICKLE: pa = &picklePA; break;
        case BurgerSpaceServer::TOP_BUN: pa = &topBunPA; break;
        case BurgerSpaceServer::LETTUCE: pa = &lettucePA; break;
        case BurgerSpaceServer::MEAT: pa = &meatPA; break;
        case BurgerSpaceServer::RED_STUFF: pa = &redStuffPA; break;
        case BurgerSpaceServer::YELLOW_STUFF: pa = &yellowStuffPA; break;
        case BurgerSpaceServer::BOTTOM_BUN: pa = &bottomBunPA; break;
        case BurgerSpaceServer::TREAT: pa = &treatPA; break;
        case BurgerSpaceServer::DIGIT: pa = &digitPA; break;
        default:
            cout << PROGRAM ": BurgerSpaceClient::handleSpriteUpdate() got invalid sprite type " << type << endl;
            assert(!"Invalid SpriteType value");
            return;
    }
    assert(pa != NULL);

    map<uint32_t, Sprite *>::iterator it = spriteTable.find(id);
    Sprite *s;
    if (it == spriteTable.end())
    {
        s = new Sprite(*pa, pos, Couple(), Couple(), Couple(), Couple());
        spriteTable[id] = s;
    }
    else
    {
        s = it->second;
        assert(s != NULL);
        assert(s->getPixmapArray() == pa);

        s->setPos(pos);
    }
    s->currentPixmapIndex = pixmapIndex;

    assert(s->currentPixmapIndex < pa->getNumImages() || s->currentPixmapIndex == size_t(-1));
}


void
BurgerSpaceClient::handleSpriteDeletion(uint32_t id)
{
    //cerr << "BurgerSpaceClient::handleSpriteDeletion(" << id << ")\n";
     map<uint32_t, Sprite *>::iterator it = spriteTable.find(id);
    if (it == spriteTable.end())
        return;
    delete it->second;
    spriteTable.erase(it);
}


void
BurgerSpaceClient::handleSoundEffect(BurgerSpaceServer::SoundEffect se)
{
    //cerr << "BurgerSpaceClient::handleSoundEffect(" << se << ")" << endl;

    SoundMixer::Chunk *chunk = NULL;
    switch (se)
    {
        case BurgerSpaceServer::ingredientBouncesSound: chunk = &ingredientBouncesChunk; break;
        case BurgerSpaceServer::ingredientInPlateSound: chunk = &ingredientInPlateChunk; break;
        case BurgerSpaceServer::ingredientFallsSound: chunk = &ingredientFallsChunk; break;
        case BurgerSpaceServer::ingredientLoweredSound: chunk = &ingredientLoweredChunk; break;
        case BurgerSpaceServer::enemyCatchesChefSound: chunk = &enemyCatchesChefChunk; break;
        case BurgerSpaceServer::enemyParalyzedSound: chunk = &enemyParalyzedChunk; break;
        case BurgerSpaceServer::enemySmashedSound: chunk = &enemySmashedChunk; break;
        case BurgerSpaceServer::chefThrowsPepperSound: chunk = &chefThrowsPepperChunk; break;
        case BurgerSpaceServer::chefGetsTreatSound: chunk = &chefGetsTreatChunk; break;
        case BurgerSpaceServer::chefShootsBlanksSound: chunk = &chefShootsBlanksChunk; break;
        case BurgerSpaceServer::newGameStartsSound: chunk = &newGameStartsChunk; break;
        case BurgerSpaceServer::levelFinishedSound: chunk = &levelFinishedChunk; break;
        case BurgerSpaceServer::treatAppearsSound: chunk = &treatAppearsChunk; break;
        case BurgerSpaceServer::treatDisappearsSound: chunk = &treatDisappearsChunk; break;
        default: cout << PROGRAM << ": unknown sound effect #" << se << endl;
    }

    if (theSoundMixer != NULL)
    {
        try
        {
            theSoundMixer->playChunk(*chunk);
        }
        catch (const SoundMixer::Error &e)
        {
            cerr << "playSoundEffect: " << e.what() << " (chunk at " << &chunk << ")" << endl;
        }
    }
}


void
BurgerSpaceClient::handleRoleAssignment(Role assignedRole)
{
    role = assignedRole;
}


void
BurgerSpaceClient::handleScoreUpdate(long theScore, int numLives, int numAvailablePeppers, int cumulLevelNo)
{
    //cerr << "BurgerSpaceClient::handleScoreUpdate(" << theScore << ", " << numLives << ", _, _)\n";
    this->theScore = theScore;
    this->numLives = numLives;
    this->numAvailablePeppers = numAvailablePeppers;
    this->cumulLevelNo = cumulLevelNo;
}
