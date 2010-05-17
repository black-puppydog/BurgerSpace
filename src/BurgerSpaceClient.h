/*  $Id: BurgerSpaceClient.h,v 1.1.2.43 2010/05/15 02:03:58 sarrazip Exp $
    BurgerSpaceClient.h - Game client

    burgerspace - A hamburger-smashing video game.
    Copyright (C) 2008-2010 Pierre Sarrazin <http://sarrazip.com/>

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

#ifndef _H_BurgerSpaceClient
#define _H_BurgerSpaceClient

#include <flatzebra/GameEngine.h>
#include <flatzebra/SoundMixer.h>
#include <flatzebra/Sprite.h>

#include <map>

#include "BurgerSpaceServer.h"

class RemoteServer;


class BurgerSpaceClient : public flatzebra::GameEngine
{
public:

    BurgerSpaceClient(Role _role,
                        bool _showRole,
                        bool _useSound,
                        SDLKey _pepperKey,
                        bool _fullScreen,
                        bool _processActiveEvent);

    virtual ~BurgerSpaceClient();

    /*  Creates an object that establishes a UDP connection to a UDP server.
        This method blocks while it resolves the given server hostname
        to an IP address.
        'port' must be the UDP port on which the server is listening on
        the machine designated by the hostname.
        'role' must be the role that the user of this client wants to play;
        the server may not grand ROLE_CHEF or ROLE_ENEMY it the role is
        already assigned to an already connected user.
        Stores -1 in 'errorCode' if the host name could not be resolved to
        an IP address.
        Stores a positive <errno.h> code in 'errorCode' if it failed to
        create and connect a UDP socket to the designated host and port.
        Stores 0 in 'errorCode' otherwise.
        If 'errorCode' is non-zero, then the method also returns NULL.
        If 'errorCode' is zero, then the method also returns a non-null
        pointer to an object that represents the server.  This pointer
        should then be passed to setServer().
        Upon a successful connection, the method sends a description of
        this client to the server and requests a level update from the
        server.
    */
    RemoteServer *createRemoteServer(const std::string &serverHostname,
                                        unsigned short port,
                                        Role role,
                                        int &errorCode);

    /*  Tells this client object to use 'server' as its server interface.
        'server' should be obtained from createRemoteServer().
    */
    void setServer(BurgerSpaceServerInterface *server);


    // Inherited from flatzebra::GameEngine -- not used.
    virtual void processKey(SDLKey /*keysym*/, bool /*pressed*/) {}

    // Inherited from flatzebra::GameEngine.
    virtual void processActivation(bool appActive);

    // Inherited from flatzebra::GameEngine -- not used.
    virtual bool tick() { return false; }

    void runClientMode(Uint32 millisecondsPerFrame);

    void handleRoleAssignment(Role assignedRole);

    void handleLevelUpdate(int levelNo, size_t numColumns, size_t numRows, flatzebra::Couple levelPos, const std::string &desc);

    void handleSpriteUpdate(uint32_t id, BurgerSpaceServer::SpriteType type, flatzebra::Couple pos, size_t pixmapIndex);

    void handleSpriteDeletion(uint32_t id);

    void handleSoundEffect(BurgerSpaceServer::SoundEffect se);

    void handleScoreUpdate(long theScore, int numLives, int numAvailablePeppers, int cumulLevelNo);

private:

    int getSpriteDisplayLevel(const flatzebra::Sprite &s) const;

private:

    BurgerSpaceServerInterface *serverInterface;
    Role role;
    bool showRole;
    flatzebra::Couple roleAreaPos;
    flatzebra::Couple roleAreaSize;
    BurgerSpaceServer::Level theCurrentLevel;
    SDL_Surface *tilePixmaps[5];
    flatzebra::PixmapArray playerPA;
    flatzebra::PixmapArray pepperPA;
    flatzebra::PixmapArray eggPA;
    flatzebra::PixmapArray hotdogPA;
    flatzebra::PixmapArray picklePA;
    flatzebra::PixmapArray topBunPA;
    flatzebra::PixmapArray lettucePA;
    flatzebra::PixmapArray meatPA;
    flatzebra::PixmapArray redStuffPA;
    flatzebra::PixmapArray yellowStuffPA;
    flatzebra::PixmapArray bottomBunPA;
    flatzebra::PixmapArray treatPA;
    flatzebra::PixmapArray digitPA;

    std::map<uint32_t, flatzebra::Sprite *> spriteTable;

    long theScore;      // player's score in points
    flatzebra::Couple scoreAreaPos;
    flatzebra::Couple scoreAreaSize;

    int numLives;  // number of player lives left
    flatzebra::Couple numLivesAreaPos;
    flatzebra::Couple numLivesAreaSize;

    int numAvailablePeppers;  // number of pepper shots available to player
    flatzebra::Couple numAvailablePeppersAreaPos;
    flatzebra::Couple numAvailablePeppersAreaSize;
    SDLKey pepperKey;

    int cumulLevelNo;
    flatzebra::Couple levelNoAreaPos;
    flatzebra::Couple levelNoAreaSize;

    std::string currentPauseMessage;

    enum GameMode { QUIT_DIALOG, SAVE_DIALOG, LOAD_DIALOG, IN_GAME };

    GameMode gameMode;

    /*  SOUND EFFECTS:
    */
    flatzebra::SoundMixer *theSoundMixer;  // see method playSoundEffect()
    bool useSound;
    flatzebra::SoundMixer::Chunk ingredientBouncesChunk;
    flatzebra::SoundMixer::Chunk ingredientInPlateChunk;
    flatzebra::SoundMixer::Chunk ingredientFallsChunk;
    flatzebra::SoundMixer::Chunk ingredientLoweredChunk;
    flatzebra::SoundMixer::Chunk enemyCatchesChefChunk;
    flatzebra::SoundMixer::Chunk enemyParalyzedChunk;
    flatzebra::SoundMixer::Chunk enemySmashedChunk;
    flatzebra::SoundMixer::Chunk chefThrowsPepperChunk;
    flatzebra::SoundMixer::Chunk chefGetsTreatChunk;
    flatzebra::SoundMixer::Chunk chefShootsBlanksChunk;
    flatzebra::SoundMixer::Chunk newGameStartsChunk;
    flatzebra::SoundMixer::Chunk levelFinishedChunk;
    flatzebra::SoundMixer::Chunk treatAppearsChunk;
    flatzebra::SoundMixer::Chunk treatDisappearsChunk;

    ///////////////////////////////////////////////////////////////////////////

    void loadPixmaps() throw(flatzebra::PixmapLoadError);
    void restoreBackground();
    void draw();
    void displayMessage(int row, const char *msg);
    void showDialogBox(const std::string &msg);

    void drawQuitDialog();
    bool doQuitDialog(SDLKey lastKeyPressed);

    void drawSaveDialog();
    int getSlotNumber(SDLKey lastKeyPressed);
    static std::string getSavedGamesDir();
    static std::string formSavedGameFilename(int slotNum);
    void showSlotDirectory();
    void doSaveDialog(SDLKey lastKeyPressed);

    void drawLoadDialog();
    void doLoadDialog(SDLKey lastKeyPressed);

    // Forbidden operations:
    BurgerSpaceClient(const BurgerSpaceClient &x);
    BurgerSpaceClient &operator = (const BurgerSpaceClient &x);
};


#endif  /* _H_BurgerSpaceClient */
