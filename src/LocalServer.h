/*  $Id: LocalServer.h,v 1.1.2.3 2010/05/15 02:03:58 sarrazip Exp $
    LocalServer.h - Direct function call server

    burgerspace - A hamburger-smashing video game.
    Copyright (C) 2010 Pierre Sarrazin <http://sarrazip.com/>

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

#ifndef _H_LocalServer
#define _H_LocalServer

#include "BurgerSpaceServer.h"
#include "BurgerSpaceClient.h"


class LocalServer : public BurgerSpaceServer
{
public:
    LocalServer(BurgerSpaceClient &_client, int initLevelNo, bool oldMotionMode, std::string levelfile)
    :   BurgerSpaceServer(initLevelNo, oldMotionMode, levelfile),
        client(_client)
    {
        // NOTE: finishInit() must be called on this object, outside of a constructor
    }
    virtual void updateLevel(int levelNo, size_t numColumns, size_t numRows, flatzebra::Couple levelPos, const std::string &desc)
    {
        client.handleLevelUpdate(levelNo, numColumns, numRows, levelPos, desc);
    }
    virtual void updateSprite(uint32_t id, BurgerSpaceServer::SpriteType type, flatzebra::Couple pos, size_t pixmapIndex)
    {
        client.handleSpriteUpdate(id, type, pos, pixmapIndex);
    }
    virtual void notifySpriteDeletion(uint32_t id)
    {
        client.handleSpriteDeletion(id);
    }
    virtual void playSoundEffect(BurgerSpaceServer::SoundEffect se)
    {
        client.handleSoundEffect(se);
    }
    virtual void updateScore(long theScore, int numLives, int numAvailablePeppers, int cumulLevelNo, bool /*forceTransmission*/)
    {
        client.handleScoreUpdate(theScore, numLives, numAvailablePeppers, cumulLevelNo);
    }
    virtual void disconnect()
    {
    }
    virtual bool saveGame(std::ostream &out);
    virtual int loadGame(std::istream &in);
private:
    BurgerSpaceClient &client;
};


#endif  /* _H_LocalServer */
