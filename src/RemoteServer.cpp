/*  $Id: RemoteServer.cpp,v 1.1.2.2 2010/05/16 02:25:48 sarrazip Exp $
    RemoteServer.cpp - UDP server

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

#include "RemoteServer.h"

#include "BurgerSpaceClient.h"

#include <flatzebra/Couple.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <limits.h>


using namespace std;
using namespace flatzebra;


RemoteServer::RemoteServer(BurgerSpaceClient &_client,
                                                unsigned char _serverIPAddr[4],
                                                unsigned short _serverUDPPort,
                                                Role _role)
  : client(_client),
    role(_role),
    sock(socket(AF_INET, SOCK_DGRAM, 0)),
    serverUDPPort(_serverUDPPort),
    buffer(NULL),
    acc(),
    lastPlayerCommandSent()
{
    if (serverIPAddr == NULL)
        throw EINVAL;
    if (sock < 0)
        throw errno;
    memcpy(serverIPAddr, _serverIPAddr, 4);
    buffer = new char[MAX_LEN_PACKET];
}


//virtual
RemoteServer::~RemoteServer()
{
    if (sock >= 0)
        close(sock);

    delete [] buffer;
}


void
RemoteServer::describeClient()
{
    char s[] =
    {
        char(CLIENT_DESC_PACKET),
        char(role)
    };
    const size_t len = sizeof(s) / sizeof(s[0]);
    ssize_t numBytesSent = sendToServer(s, len);
    if (numBytesSent != ssize_t(len))
        cout << PROGRAM << ": tried to send client descriptor of " << len
                << " bytes but only " << numBytesSent << " bytes sent" << endl;
}


void
RemoteServer::requestLevelDescription()
{
    char packet = char(LEVEL_DESC_REQUEST_PACKET);
    ssize_t numBytesSent = sendToServer(&packet, 1);
    if (numBytesSent != 1)
        cout << PROGRAM << ": tried to send level description request but only " << numBytesSent << " bytes sent" << endl;
}


//virtual
void
RemoteServer::startNewGame()
{
    char s[] =
    {
        char(START_NEW_GAME_PACKET)
    };
    const size_t len = sizeof(s) / sizeof(s[0]);
    ssize_t numBytesSent = sendToServer(s, len);
    if (numBytesSent != ssize_t(len))
        cout << PROGRAM << ": tried to send start request of " << len
                << " bytes but only " << numBytesSent << " bytes sent" << endl;
}


//virtual
bool
RemoteServer::isPaused() const
{
    return false;  // No player can pause the game in client-server mode.
}


//virtual
void
RemoteServer::pauseGame()
{
    // No player can pause the game in client-server mode.
}


//virtual
void
RemoteServer::resumeGame()
{
    // No player can pause the game in client-server mode.
}


//virtual
void
RemoteServer::setChefRequest(const bool desiredDirections[4], bool shootPepper)
{
    char s[] =
    {
        char(PLAYER_COMMAND_PACKET),
        char(desiredDirections[0]),
        char(desiredDirections[1]),
        char(desiredDirections[2]),
        char(desiredDirections[3]),
        char(shootPepper)
    };
    const size_t len = sizeof(s) / sizeof(s[0]);
    if (lastPlayerCommandSent.empty() || lastPlayerCommandSent.compare(0, len, s, len) != 0)
    {
        ssize_t numBytesSent = sendToServer(s, len);
        if (numBytesSent != ssize_t(len))
            cout << PROGRAM << ": tried to send player command but only " << numBytesSent << " bytes sent" << endl;

        lastPlayerCommandSent.assign(s, len);
    }
}


//virtual
void
RemoteServer::setEnemyRequest(const bool desiredDirections[4])
{
    setChefRequest(desiredDirections, false);
}


ssize_t
RemoteServer::sendToServer(const void *s, size_t len)
{
    if (s == NULL || len == 0)
    {
        assert(!"RemoteServer::sendToServer: empty packet");
        return 0;
    }

    assert(serverIPAddr != NULL);

    sockaddr_in to;
    memset(&to, '\0', sizeof(to));
    to.sin_family = AF_INET;
    memcpy(&to.sin_addr, serverIPAddr, 4);
    to.sin_port = htons(serverUDPPort);
    return sendto(sock, s, len, 0, (sockaddr *) &to, sizeof(to));
}


//virtual
bool
RemoteServer::update()
{
    assert(buffer != NULL);

    // Receive and interpret messages from the server, if any.

    if (sock < 0)
    {
        cout << PROGRAM << ": invalid socket" << endl;
        return false;
    }

    // Process all immediately available UDP packets:
    for (;;)
    {
        sockaddr_in from;
        memset(&from, '\0', sizeof(from));
        socklen_t length = sizeof(from);
        ssize_t numBytesReceived = recvfrom(sock, buffer, MAX_LEN_PACKET,
                                        MSG_DONTWAIT, (sockaddr *) &from, &length);
        int e = errno;
        //cerr << "RemoteServer::update: " << numBytesReceived << " " << e << endl;
        if (numBytesReceived < 0)  // if error or no packet available
        {
            if (e == EAGAIN)  // if no packet available
                return true;
            cout << PROGRAM << ": read error on socket: " << strerror(e) << endl;
            return false;
        }
        if (numBytesReceived == 0)
            continue;
        //cerr << "RemoteServer::update: received " << numBytesReceived << " byte(s)" << endl;

        processReceivedBytes(buffer, numBytesReceived);
    }

    return true;
}


//virtual
void
RemoteServer::disconnect()
{
    char s[] =
    {
        char(DISCONNECT_PACKET)
    };
    const size_t len = sizeof(s) / sizeof(s[0]);
    ssize_t numBytesSent = sendToServer(s, len);
    if (numBytesSent != ssize_t(len))
        cout << PROGRAM << ": tried to disconnect packet but only " << numBytesSent << " bytes sent" << endl;
}


//virtual
bool
RemoteServer::saveGame(std::ostream &out)
{
    return false;  // save not supported in client-server mode
}


//virtual
int
RemoteServer::loadGame(std::istream &in)
{
    return -1;  // load not supported in client-server mode
}


void
RemoteServer::processReceivedBytes(const char *s, ssize_t len)
{
    assert(s != NULL);
    assert(len > 0);

    acc.append(s, len);
    if (acc.length() < 2)
        return;  // nothing to do

    size_t origAccLen = acc.length();

    ServerPacketType type = static_cast<ServerPacketType>(* (uint16_t *) acc.data());
    //cerr << "len=" << len << ", type=" << type << ", acc=" << acc.length() << endl;

    if (type == UPDATE_LEVEL_PACKET && acc.length() >= 2 + 12)
    {
        uint16_t levelNo = * (uint16_t *) &acc[2];
        uint16_t numRows = * (uint16_t *) &acc[4];
        uint16_t numColumns = * (uint16_t *) &acc[6];
        uint16_t levelPosX = * (uint16_t *) &acc[8];
        uint16_t levelPosY = * (uint16_t *) &acc[10];
        uint16_t descLen = * (uint16_t *) &acc[12];
        string desc(acc, 14, descLen);

        if (false) cout << "processReceivedBytes/0: "
                << levelNo << " "
                << numRows << " "
                << numColumns << " "
                << int16_t(levelPosX) << " "
                << int16_t(levelPosY) << " "
                << descLen << " '"
                << desc << "'" << endl;
        assert(uint16_t(desc.length()) == numRows * numColumns);

        client.handleLevelUpdate(int(levelNo), size_t(numRows), size_t(numColumns),
                                    Couple(levelPosX, levelPosY), desc);

        acc.erase(0, 14 + descLen);
    }
    else if (type == UPDATE_SPRITE_PACKET && acc.length() >= 4)
    {
        // Expected at offset 2:
        // number of sprite updates, then array of updates.

        size_t numUpdates = * (uint16_t *) &acc[2];
        size_t reqLen = 2 + 2 + numUpdates * UPDATE_SPRITE_PACKET_LEN;
        assert(acc.length() >= reqLen);

        for (size_t i = 0; i < numUpdates; ++i)
        {
            size_t j = 2 + 2 + i * UPDATE_SPRITE_PACKET_LEN;
            uint32_t id = * (uint32_t *) &acc[j];
            uint16_t stype = * (uint16_t *) &acc[j + 4];
            uint16_t posX = * (uint16_t *) &acc[j + 6];
            uint16_t posY = * (uint16_t *) &acc[j + 8];
            uint16_t pixmapIndex = * (uint16_t *) &acc[j + 10];

            BurgerSpaceServer::SpriteType spriteType = BurgerSpaceServer::SpriteType(stype);
            if (spriteType == BurgerSpaceServer::NO_SPRITE)
                client.handleSpriteDeletion(id);
            else
                client.handleSpriteUpdate(id,
                                        spriteType,
                                        Couple(posX, posY),
                                        pixmapIndex == USHRT_MAX ? size_t(-1) : size_t(pixmapIndex));
        }

        acc.erase(0, reqLen);
    }
    else if (type == UPDATE_SCORE_PACKET && acc.length() >= 2 + 10)
    {
        uint32_t score = * (uint32_t *) &acc[2];
        uint16_t lives = * (uint16_t *) &acc[6];
        uint16_t peppers = * (uint16_t *) &acc[8];
        uint16_t level = * (uint16_t *) &acc[10];

        client.handleScoreUpdate(long(score), int(lives), int(peppers), int(level));

        acc.erase(0, 2 + 10);
    }
    else if (type == PLAY_SOUND_EFFECT_PACKET && acc.length() >= 2 + 2)
    {
        uint16_t se = * (uint16_t *) &acc[2];

        client.handleSoundEffect(BurgerSpaceServer::SoundEffect(se));

        acc.erase(0, 2 + 2);
    }
    else if (type == ROLE_ASSIGNMENT_PACKET && acc.length() >= 2 + 1)
    {
        char r = * (char *) &acc[2];
        client.handleRoleAssignment(static_cast<Role>(r));
        acc.erase(0, 2 + 1);
    }
    else
        cout << PROGRAM << ": invalid server packet of type " << type << endl;

    if (acc.length() == origAccLen)
        cout << PROGRAM << ": packet accumulator not consumed" << endl;
}
