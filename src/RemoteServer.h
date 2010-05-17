/*  $Id: RemoteServer.h,v 1.1.2.2 2010/05/16 02:25:48 sarrazip Exp $
    RemoteServer.h - UDP server

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

#ifndef _H_RemoteServer
#define _H_RemoteServer

#include "BurgerSpaceServer.h"
#include "util.h"


class BurgerSpaceClient;


class RemoteServer : public BurgerSpaceServerInterface
{
public:

    RemoteServer(BurgerSpaceClient &_client,
                unsigned char _serverIPAddr[4],
                unsigned short _udpPort,
                Role _role);

    virtual ~RemoteServer();

    /** Describe this client to the server.
    */
    void describeClient();

    void requestLevelDescription();

    virtual void startNewGame();

    virtual bool isPaused() const;

    virtual void pauseGame();

    virtual void resumeGame();

    virtual void setChefRequest(const bool desiredDirections[4], bool shootPepper);

    virtual void setEnemyRequest(const bool desiredDirections[4]);

    virtual bool update();

    virtual void disconnect();

    virtual bool saveGame(std::ostream &out);

    virtual int loadGame(std::istream &in);

private:

    void processReceivedBytes(const char *s, ssize_t len);

    ssize_t sendToServer(const void *s, size_t len);


    BurgerSpaceClient &client;
    Role role;
    int sock;
    unsigned char serverIPAddr[4];
    unsigned short serverUDPPort;
    char *buffer;  // array that receives data from socket
    std::string acc;  // accumulates packet bytes until a complete command has been received
    std::string lastPlayerCommandSent;

    // Forbidden operations:
    RemoteServer(const RemoteServer &);
    RemoteServer &operator = (const RemoteServer &);

};


#endif  /* _H_RemoteServer */
