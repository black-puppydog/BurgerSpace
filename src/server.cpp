/*  $Id: server.cpp,v 1.1.2.65 2010/05/16 02:25:48 sarrazip Exp $
    server.cpp - main() function for BurgerSpace server

    burgerspace - A hamburger-smashing video game.
    Copyright (C) 2008 Pierre Sarrazin <http://sarrazip.com/>

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

#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <map>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif

#ifndef DEFAULT_UDP_SERVER_PORT
#error DEFAULT_UDP_SERVER_PORT must be defined to valid UDP port number
#endif

using namespace std;


#ifdef HAVE_GETOPT_LONG
static struct option knownOptions[] =
{
    { "help",          no_argument,       NULL, 'h' },
    { "version",       no_argument,       NULL, 'v' },
    { "initial-level", required_argument, NULL, 'i' },
    { "port",          required_argument, NULL, 'p' },
    { "levelset",      required_argument, NULL, 'l' },

    { NULL, 0, NULL, 0 }  // marks the end
};


static
void
displayVersionNo()
{
    cout << PROGRAM << " (" << PACKAGE << " " << VERSION << ")" << endl;
}


static
void
displayHelp()
{
    cout << "\n";

    displayVersionNo();

    cout <<
"\n"
"Copyright (C) 2001-2008 Pierre Sarrazin <http://sarrazip.com/>\n"
"This program is free software; you may redistribute it under the terms of\n"
"the GNU General Public License.  This program has absolutely no warranty.\n"
    ;

    cout <<
"\n"
"Known options:\n"
"--help             Display this help page and exit\n"
"--version          Display this program's version number and exit\n"
"--initial-level=N  Start game at level N (N >= 1) [default=1]\n"
"--port=PORT        Listen on specified UDP port [default=" << DEFAULT_UDP_SERVER_PORT << "]\n"
"\n"
    ;
}
#endif  /* HAVE_GETOPT_LONG */


template <class Integer>
inline void
appendUint16(string &s, Integer n)
{
    uint16_t word = static_cast<uint16_t>(n);
    s.append((char *) &word, sizeof(word));
}


template <class Integer>
inline void
appendUint32(string &s, Integer n)
{
    uint32_t dword = static_cast<uint32_t>(n);
    s.append((char *) &dword, sizeof(dword));
}


class GameClient
{
public:

    GameClient()
      : udpPort(0),
        to(),
        role(ROLE_SPECTATOR)
    {
        memset(ipAddr, '\0', 4);
        memset(&to, '\0', sizeof(to));
    }

    GameClient(const unsigned char _ipAddr[4], unsigned short _udpPort, Role _role)
      : udpPort(0),
        to(),
        role(_role)
    {
        init(_ipAddr, _udpPort);
    }

    GameClient(const GameClient &gc)
      : udpPort(0),
        to(),
        role(gc.role)
    {
        init(gc.ipAddr, gc.udpPort);
    }

    GameClient &operator = (const GameClient &gc)
    {
        if (this != &gc)
        {
            role = gc.role;
            init(gc.ipAddr, gc.udpPort);
        }
        return *this;
    }

    void init(const unsigned char _ipAddr[4], unsigned short _udpPort)
    {
        memcpy(ipAddr, _ipAddr, 4);
        udpPort = _udpPort;

        memset(&to, '\0', sizeof(to));
        to.sin_family = AF_INET;
        memcpy(&to.sin_addr, _ipAddr, 4);
        to.sin_port = htons(_udpPort);
    }

    bool isValid() const
    {
        return ipAddr[0] != 0 || ipAddr[1] != 0 || ipAddr[2] != 0 || ipAddr[3] != 0;
    }

    Role getRole() const
    {
        return role;
    }

    void setRole(Role newRole)
    {
        role = newRole;
    }

    bool hasAddress(const unsigned char _ipAddr[4], unsigned short _udpPort) const
    {
        return udpPort == _udpPort && memcmp(ipAddr, _ipAddr, 4) == 0;
    }

    bool operator == (const GameClient &gc) const
    {
        return ipAddr[0] == gc.ipAddr[0]
               && ipAddr[1] == gc.ipAddr[1]
               && ipAddr[2] == gc.ipAddr[2]
               && ipAddr[3] == gc.ipAddr[3]
               && udpPort == gc.udpPort
               && role == gc.role;
    }

    bool operator < (const GameClient &gc) const
    {
        if (ipAddr[0] != gc.ipAddr[0])
            return ipAddr[0] < gc.ipAddr[0];
        if (ipAddr[1] != gc.ipAddr[1])
            return ipAddr[1] < gc.ipAddr[1];
        if (ipAddr[2] != gc.ipAddr[2])
            return ipAddr[2] < gc.ipAddr[2];
        if (ipAddr[3] != gc.ipAddr[3])
            return ipAddr[3] < gc.ipAddr[3];
        if (udpPort != gc.udpPort)
            return udpPort < gc.udpPort;
        return int(role) < int(gc.role);
    }

    ssize_t sendString(const string &p, int sock)
    {
        return sendto(sock, p.data(), p.length(), 0, (const sockaddr *) &to, sizeof(to));
    }

public:
    unsigned char ipAddr[4];
    unsigned short udpPort;
    sockaddr_in to;  // copy of ipAddr+udpPort for sendPacket()
    Role role;
};


class CircularPacketBuffer
{
public:

    CircularPacketBuffer(size_t capacity)
      : vec(capacity),
        reader(0),
        writer(0)
    {
        assert(vec.size() > 0);
        assert(empty());
        assert(!full());
    }

    bool empty() const
    {
        return writer == reader;
    }

    bool full() const
    {
        return nextWriterIndex() == reader;
    }

    size_t size() const
    {
        if (writer >= reader)
            return writer - reader;
        return vec.size() - (reader - writer);
    }

    void add(const string &packet)
    {
        assert(!(empty() && full()));
        if (full())
            removeOldest(1);

        vec[writer] = packet;
        writer = nextWriterIndex();
        assert(!(empty() && full()));
    }

    const string &oldest() const
    {
        assert(!empty());
        return vec[reader];
    }

    void removeOldest(size_t numToRemove)
    {
        assert(numToRemove <= size());
        reader = (reader + numToRemove) % vec.size();
    }

private:

    size_t nextWriterIndex() const
    {
        return (writer + 1) % vec.size();
    }

    vector<string> vec;
    size_t reader;  // index into vec
    size_t writer;  // index into vec
};


class BurgerSpaceCommandLineServer : public BurgerSpaceServer
{
public:

    BurgerSpaceCommandLineServer(int _initLevelNumber, int _sock, Uint32 _minMSBetweenWrites, string levelfile) throw(int, std::string)
      : BurgerSpaceServer(_initLevelNumber, false, levelfile),
        sock(_sock),
        minMSBetweenWrites(_minMSBetweenWrites),
        pendingLevelUpdatePacket(),
        spriteUpdatePacketTable(),
        pendingSpriteUpdatePackets(400),
        pendingSoundEffectPacket(),
        timeOfLastSend(0),
        pendingScoreUpdatePacket(),
        lastScoreUpdatePacket(),
        timeOfLastScoreUpdate(0),
        numPacketsSent(0),
        totalPacketBytesSent(0),
        gameClientSet()
    {
    }

    /*  If the server socket has any available packets from the client(s),
        this method processes them immediately, then returns.
        If no packets are available, this method returns immediately.
        Writes error messages to cerr.
    */
    void processAvailableIncomingPackets();

    GameClient *insertGameClient(const unsigned char _ipAddr[4], unsigned short _udpPort, Role _requestedRole)
    {
        if (haveClientWithRole(_requestedRole))
            _requestedRole = ROLE_SPECTATOR;
        gameClientSet.push_back(GameClient(_ipAddr, _udpPort, _requestedRole));
        return &gameClientSet.back();
    }

    void eraseGameClient(const GameClient &gc)
    {
        std::vector<GameClient>::iterator newEnd =
                        remove(gameClientSet.begin(), gameClientSet.end(), gc);
        gameClientSet.erase(newEnd, gameClientSet.end());
    }

    virtual void updateLevel(int levelNo, size_t numRows, size_t numColumns, flatzebra::Couple levelPos, const std::string &desc)
    {
        //cout << "updateLevel(" << levelNo << ", " << numRows << ", " << numColumns << ", (" << levelPos.x << ", " << levelPos.y << "), '" << desc << "')" << endl;
        assert(desc.length() == numRows * numColumns);

        pendingLevelUpdatePacket.clear();
        appendUint16(pendingLevelUpdatePacket, UPDATE_LEVEL_PACKET);
        appendUint16(pendingLevelUpdatePacket, levelNo);
        appendUint16(pendingLevelUpdatePacket, numRows);
        appendUint16(pendingLevelUpdatePacket, numColumns);
        appendUint16(pendingLevelUpdatePacket, levelPos.x);
        appendUint16(pendingLevelUpdatePacket, levelPos.y);
        appendUint16(pendingLevelUpdatePacket, desc.length());
        pendingLevelUpdatePacket += desc;
        //cout << "updateLevel: pendingLevelUpdatePacket now " << pendingLevelUpdatePacket.length() << " chars" << endl;

        // Force update of all sprites upon level change.
        spriteUpdatePacketTable.clear();
    }

    virtual void updateSprite(uint32_t id, SpriteType type, flatzebra::Couple pos, size_t pixmapIndex)
    {
        //cout << "updateSprite(" << type << ", (" << pos.x << ", " << pos.y << "), " << pixmapIndex << ")" << endl;
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - timeOfLastSend;
        if (elapsed < minMSBetweenWrites)
            return;
        //cout << "updateSprite: " << elapsed << " ms; " << pendingSpriteUpdatePackets.size() << " SUs" << endl;

        string packet;
        appendUint32(packet, id);
        appendUint16(packet, type);
        appendUint16(packet, pos.x);
        appendUint16(packet, pos.y);
        appendUint16(packet, pixmapIndex == size_t(-1) ? USHRT_MAX : pixmapIndex);
        assert(packet.length() == UPDATE_SPRITE_PACKET_LEN);

        // Remember packet sent for each sprite ID.
        // If this packet has not changed since last send,
        // then don't send it again.
        if (registerSpriteUpdatePacket(id, packet))
            pendingSpriteUpdatePackets.add(packet);
    }

    virtual void notifySpriteDeletion(uint32_t id)
    {
        string packet;
        appendUint32(packet, id);
        appendUint16(packet, NO_SPRITE);  // indicates deletion
        appendUint16(packet, 0);
        appendUint16(packet, 0);
        appendUint16(packet, 0);
        assert(packet.length() == UPDATE_SPRITE_PACKET_LEN);

        if (registerSpriteUpdatePacket(id, packet))
            pendingSpriteUpdatePackets.add(packet);
    }

    virtual void updateScore(long theScore, int numLives, int numAvailablePeppers, int cumulLevelNo, bool forceTransmission)
    {
        Uint32 now = SDL_GetTicks();
        if (now - timeOfLastScoreUpdate < 55)
            return;

        string newScoreUpdatePacket;
        appendUint16(newScoreUpdatePacket, UPDATE_SCORE_PACKET);
        appendUint32(newScoreUpdatePacket, theScore);
        appendUint16(newScoreUpdatePacket, numLives);
        appendUint16(newScoreUpdatePacket, numAvailablePeppers);
        appendUint16(newScoreUpdatePacket, cumulLevelNo);

        if (!forceTransmission && newScoreUpdatePacket == lastScoreUpdatePacket)
            return;

        //cout << "updateScore: " << theScore << " " << numLives << " " << numAvailablePeppers << " " << cumulLevelNo << endl;
        pendingScoreUpdatePacket = newScoreUpdatePacket;
        timeOfLastScoreUpdate = now;
    }

    virtual void playSoundEffect(SoundEffect se)
    {
        //cout << "playSoundEffect(" << se << ")" << endl;
        pendingSoundEffectPacket.clear();
        appendUint16(pendingSoundEffectPacket, PLAY_SOUND_EFFECT_PACKET);
        appendUint16(pendingSoundEffectPacket, se);
    }

    virtual EnemyType chooseEnemyType()
    {
        flatzebra::SpriteList::const_iterator it;
        for (it = enemySprites.begin(); it != enemySprites.end(); it++)
            if ((*it)->getPixmapArray() == &eggPA)
            {
                // There is already an egg among the existing enemies.
                // Since the egg is controlled by the user, none of the
                // computer-controlled enemies are eggs, to avoid confusion.
                return enemyTypeCounter++ % 2 == 0 ? ENEMY_HOT_DOG : ENEMY_PICKLE;
            }
        return ENEMY_EGG;  // no egg, so create one
    }

    virtual bool isUserControlledEnemy(const flatzebra::Sprite &s) const
    {
        return s.getPixmapArray() == &eggPA;
    }

    virtual void disconnect()
    {
    }

    virtual bool saveGame(std::ostream &out)
    {
        return false;  // save not supported in client-server mode
    }

    virtual int loadGame(std::istream &in)
    {
        return -1;  // load not supported in client-server mode
    }

    void sendPendingPacketsToClientSet()
    {
        //cerr << pendingSpriteUpdatePackets.size() << endl;

        // Prepare a "batch" packet that contains several
        // sprite update sub-packets to be sent to each client.
        // This removes the oldest elements of pendingSpriteUpdatePackets.
        // Note that this prevents pendingSpriteUpdatePackets from growing
        // indefinitely while waiting for clients: the oldest elements are
        // removed even when no client is connected.
        //
        string spriteUpdateBatchPacket;
        buildSpriteUpdateBatchPacket(spriteUpdateBatchPacket);

        // For each connected client, send any pending packets.
        for (vector<GameClient>::iterator it = gameClientSet.begin();
                                                it != gameClientSet.end(); ++it)
            sendPendingPackets(*it, spriteUpdateBatchPacket);

        // Clear pending packets.
        pendingLevelUpdatePacket.clear();
        pendingSoundEffectPacket.clear();
        pendingScoreUpdatePacket.clear();
    }

    GameClient *getGameClientByAddress(const unsigned char _ipAddr[4], unsigned short _udpPort)
    {
        for (vector<GameClient>::iterator it = gameClientSet.begin();
                                            it != gameClientSet.end(); ++it)
            if (it->hasAddress(_ipAddr, _udpPort))
                return &*it;
        return NULL;
    }

    bool haveClientWithRole(Role role) const
    {
        for (vector<GameClient>::const_iterator it = gameClientSet.begin();
                                            it != gameClientSet.end(); ++it)
            if (it->getRole() == role)
                return true;
        return false;
    }

    size_t getNumGameClients() const
    {
        return gameClientSet.size();
    }

    bool haveTwoPlayers() const
    {
        return haveClientWithRole(ROLE_CHEF) && haveClientWithRole(ROLE_ENEMY);
    }

    void getStats(size_t &_numPacketsSent, ssize_t &_totalPacketBytesSent)
    {
        _numPacketsSent = numPacketsSent;
        _totalPacketBytesSent = totalPacketBytesSent;
    }

private:

    void sendPendingPackets(GameClient &client, const string &spriteUpdateBatchPacket)
    {
        //cout << "sendPendingPackets: " << pendingLevelUpdatePacket.length() << " " << pendingSpriteUpdatePackets.size() << endl;

        if (pendingLevelUpdatePacket.length() > 0)
        {
            sendPacket(pendingLevelUpdatePacket, client);
        }
        if (spriteUpdateBatchPacket.size() > 0)
        {
            sendPacket(spriteUpdateBatchPacket, client);
        }
        if (pendingSoundEffectPacket.length() > 0)
        {
            sendPacket(pendingSoundEffectPacket, client);
        }
        if (pendingScoreUpdatePacket.length() > 0)
        {
            sendPacket(pendingScoreUpdatePacket, client);
            lastScoreUpdatePacket = pendingScoreUpdatePacket;
        }
    }

    void buildSpriteUpdateBatchPacket(string &batchPacket)
    {
        batchPacket.clear();

        if (pendingSpriteUpdatePackets.size() == 0)
            return;

        // Compute number of updates that fit in a packet
        // of at most MAX_LEN_PACKET bytes.
        size_t maxUpdates = (MAX_LEN_PACKET - 2 - 2) / UPDATE_SPRITE_PACKET_LEN;
        size_t numToSend = min(pendingSpriteUpdatePackets.size(), maxUpdates);

        assert(numToSend <= pendingSpriteUpdatePackets.size());

        batchPacket.reserve(MAX_LEN_PACKET);
        appendUint16(batchPacket, UPDATE_SPRITE_PACKET);
        appendUint16(batchPacket, numToSend);

        for (size_t j = 0; j < numToSend; ++j)
        {
            const string &packet = pendingSpriteUpdatePackets.oldest();
            assert(packet.length() == UPDATE_SPRITE_PACKET_LEN);
            batchPacket += packet;
            pendingSpriteUpdatePackets.removeOldest(1);
        }

        //cout << "UPDATE_SPRITE_PACKET: max=" << maxUpdates << ", numToSend=" << numToSend << ", vector=" << pendingSpriteUpdatePackets.size() << pendingSpriteUpdatePackets.size() << ", singlePacket=" << singlePacket.length() << endl;

        assert(batchPacket.length() == 2 + 2 + numToSend * UPDATE_SPRITE_PACKET_LEN);
        assert(batchPacket.length() <= MAX_LEN_PACKET);
    }

private:

    void sendPacket(const string &p, GameClient &client)
    {
        ssize_t numBytesSent = client.sendString(p, sock);
        //cerr << "Sent " << numBytesSent << " bytes of packet of " << p.length() << " bytes" << endl;
        if (numBytesSent != ssize_t(p.length()))
        {
            int e = errno;
            cout << PROGRAM << ": sendPacket: tried to send " << p.length() << " bytes but only " << numBytesSent << " sent: " << strerror(e) << endl;
        }
        else
        {
            ++numPacketsSent;
            totalPacketBytesSent += numBytesSent;
            //cout << "sendPacket: sent " << numBytesSent << " bytes to " << hex
                    //<< to.sin_addr.s_addr << dec << ":" << to.sin_port << endl;
            timeOfLastSend = SDL_GetTicks();
        }
    }

    // Registers the given packet string with the given sprite ID.
    // If spriteUpdatePacketTable does not already have any entry for spriteId,
    // then a new entry is added and this method returns true.
    // If spriteUpdatePacketTable already has any entry for spriteId,
    // then the packet string is stored in this entry and this method
    // returns true.
    // Otherwise, this method returns false, to indicate that 'packet' was
    // exactly the sprite update packet that was last sent to 'spriteId'
    // (thus, it need not be sent again).
    //
    bool registerSpriteUpdatePacket(uint32_t spriteId, const string &packet)
    {
        map<uint32_t, string>::iterator it = spriteUpdatePacketTable.find(spriteId);
        if (it == spriteUpdatePacketTable.end())
        {
            spriteUpdatePacketTable[spriteId] = packet;
            return true;
        }
        if (it->second != packet)
        {
            it->second = packet;
            return true;
        }
        return false;
    }


    int sock;
    Uint32 minMSBetweenWrites;  // minimum period in milliseconds between writes to client(s)
    string pendingLevelUpdatePacket;
    map<uint32_t, string> spriteUpdatePacketTable;  // key: sprite ID; value: last sent sprite update packet for this ID
    CircularPacketBuffer pendingSpriteUpdatePackets;
    string pendingSoundEffectPacket;
    Uint32 timeOfLastSend;  // maintained by sendPacket()
    string pendingScoreUpdatePacket;
    string lastScoreUpdatePacket;
    Uint32 timeOfLastScoreUpdate;  // used to space out score updates
    size_t numPacketsSent;
    ssize_t totalPacketBytesSent;
    std::vector<GameClient> gameClientSet;

};


void
BurgerSpaceCommandLineServer::processAvailableIncomingPackets()
{
    char buffer[MAX_LEN_PACKET];
    sockaddr_in from;

    for (;;)
    {
        memset(&from, '\0', sizeof(from));
        socklen_t length = sizeof(from);
        ssize_t numBytesReceived = recvfrom(sock, buffer, sizeof(buffer), MSG_DONTWAIT, (sockaddr *) &from, &length);
        int e = errno;
        if (numBytesReceived < 0)  // if error or no packet available
        {
            if (e != EAGAIN)  // if error
                cerr << PROGRAM << ": receive error from socket: " << strerror(e) << endl;
            break;
        }
        if (numBytesReceived == 0)
            continue;

        const unsigned char *ipAddr = (unsigned char *) &from.sin_addr.s_addr;
        unsigned short port = (unsigned short) ntohs(from.sin_port);

        GameClient *gameClient = getGameClientByAddress(ipAddr, port);

        #if 0
        cout << "Got " << numBytesReceived << " bytes (" << hex << int(buffer[0]) << ") from 0x"
                << (unsigned long) from.sin_addr.s_addr << ":0x" << from.sin_port << dec
                << ", role=" << (gameClient == NULL ? -1 : gameClient->getRole()) << endl;
        #endif

        switch (static_cast<ClientPacketType>(buffer[0]))
        {
            case CLIENT_DESC_PACKET:  // must be 1st packet from newly connected client
                assert(numBytesReceived == 2);
                if (gameClient != NULL)
                {
                    cerr << PROGRAM << ": PLAYER_COMMAND_PACKET from already connected client" << endl;
                    break;
                }
                {
                    int role = static_cast<int>(buffer[1]);
                    if (role != ROLE_SPECTATOR
                            && role != ROLE_CHEF
                            && role != ROLE_ENEMY)
                    {
                        cerr << PROGRAM << ": PLAYER_COMMAND_PACKET contains invalid role number" << endl;
                        role = ROLE_SPECTATOR;
                    }
                    gameClient = insertGameClient(ipAddr, port, static_cast<Role>(role));
                    assert(gameClient != NULL);

                    // Send the client the role actually assigned by the server:
                    string roleAssignmentPacket;
                    appendUint16(roleAssignmentPacket, ROLE_ASSIGNMENT_PACKET);
                    roleAssignmentPacket += char(gameClient->getRole());
                    gameClient->sendString(roleAssignmentPacket, sock);
                }
                break;

            case PLAYER_COMMAND_PACKET:
            {
                if (gameClient == NULL)
                {
                    cerr << PROGRAM << ": PLAYER_COMMAND_PACKET from unknown client" << endl;
                    break;
                }

                assert(numBytesReceived == 6);
                bool directions[4] =
                    {
                        buffer[1] != '\0',
                        buffer[2] != '\0',
                        buffer[3] != '\0',
                        buffer[4] != '\0'
                    };

                switch (gameClient->getRole())
                {
                    case ROLE_CHEF:
                        setChefRequest(directions, buffer[5] != '\0');
                        break;
                    case ROLE_ENEMY:
                        setEnemyRequest(directions);
                        break;
                    case ROLE_SPECTATOR:
                        break;
                }
                break;
            }

            case START_NEW_GAME_PACKET:  // if request for start of new game
                if (gameClient == NULL)
                    cerr << PROGRAM << ": START_NEW_GAME_PACKET from unknown client" << endl;
                else if (gameClient->getRole() == ROLE_CHEF /*&& haveTwoPlayers()*/)
                {
                    assert(numBytesReceived == 1);
                    startNewGame();
                }
                break;

            case LEVEL_DESC_REQUEST_PACKET:  // if request for level description
                if (gameClient != NULL)
                {
                    sendLevelUpdate();
                    sendScoreUpdate();
                }
                else
                    cerr << PROGRAM << ": START_NEW_GAME_PACKET from unknown client" << endl;
                break;

            case DISCONNECT_PACKET:  // client is disconnecting
                if (gameClient != NULL)
                    eraseGameClient(*gameClient);
                else
                    cerr << PROGRAM << ": DISCONNECT_PACKET from unknown client" << endl;
                break;

            default:
                cerr << PROGRAM << ": unexpected packet of type " << int(buffer[0])
                        << " received from client" << endl;
        }
    }
}


int
main(int argc, char *argv[])
{
    /*  Default game parameters:
    */
    int initLevelNo = 1;
    unsigned short port = DEFAULT_UDP_SERVER_PORT;
    Uint32 minMSBetweenWrites = 25;
    string levelFile = getDir(PKGLEVELDIR, "PKGLEVELDIR")+"levelset.yaml";



    #ifdef HAVE_GETOPT_LONG

    /*  Interpret the command-line options:
    */
    int c;
    do
    {
        c = getopt_long(argc, argv, "", knownOptions, NULL);

        switch (c)
        {
            case EOF:
                break;  // nothing to do

            case 'i':
                {
                    errno = 0;
                    long n = strtol(optarg, NULL, 10);
                    if (errno == ERANGE || n < 1 || n > 500)
                    {
                        cerr << "Invalid initial level number." << endl;
                        displayHelp();
                        return EXIT_FAILURE;
                    }

                    initLevelNo = int(n);
                }
                break;

            case 'p':
                {
                    char *end = NULL;
                    long n = strtol(optarg, &end, 10);
                    if (n < 0 || n > 65535 || end == optarg || *end != '\0')
                    {
                        cerr << PROGRAM << ": --port: invalid UDP port number " << optarg << endl;
                        return EXIT_FAILURE;
                    }
                    port = static_cast<unsigned short>(n);
                }
                break;

            case 'l':
		levelFile = optarg;
                break;

            case 'v':
                displayVersionNo();
                return EXIT_SUCCESS;

            case 'h':
                displayHelp();
                return EXIT_SUCCESS;

            default:
                displayHelp();
                return EXIT_FAILURE;
        }
    } while (c != EOF && c != '?');

    #endif  /* HAVE_GETOPT_LONG */


    /*  Initialize the random number generator:
    */
    const char *s = getenv("SEED");
    unsigned int seed = (s != NULL ? atol(s) : time(NULL));
    #ifndef NDEBUG
    cerr << "seed = " << seed << endl;
    cerr << "init-level-no = " << initLevelNo << endl;
    #endif
    srand(seed);


    try
    {
        /*  Create a UDP server socket.
        */
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
            int e = errno;
            cout << PROGRAM << ": failed to create server socket: " << strerror(e) << endl;
            return EXIT_FAILURE;
        }
        sockaddr_in server;
        memset(&server, '\0', sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(port);
        if (bind(sock, (sockaddr *) &server, sizeof(server)) < 0)
        {
            int e = errno;
            cout << PROGRAM << ": failed to bind to server port " << port << ": " << strerror(e) << endl;
            return EXIT_FAILURE;
        }
        cout << PROGRAM << ": server bound to port " << port << endl;


        /*  Create the "game logic" object.
        */
        BurgerSpaceCommandLineServer theBurgerSpaceServer(initLevelNo, sock, minMSBetweenWrites, levelFile);
        theBurgerSpaceServer.finishInit();  // calls virtual functions


        /*  Main loop: process incoming network packets if any are available;
            update the game logic; send all pending response packets to the
            client(s); delay until the end of the current frame.
        */
        const Uint32 millisecondsPerFrame = 55;
        #undef STATS
        #ifdef STATS
        const Uint32 gameStartTime = SDL_GetTicks();
        #endif

        for (Uint32 frameCount = 0; ; ++frameCount)
        {
            Uint32 frameEnd = SDL_GetTicks() + millisecondsPerFrame;

            theBurgerSpaceServer.processAvailableIncomingPackets();

            #ifdef STATS
            if (frameCount % 20 == 0)
            {
                Uint32 totalElapsedTime = SDL_GetTicks() - gameStartTime;
                size_t numPacketsSent;
                ssize_t totalPacketBytesSent;
                theBurgerSpaceServer.getStats(numPacketsSent, totalPacketBytesSent);
                cout << "Stats: " << numPacketsSent << " packets, "
                        << totalPacketBytesSent << " B, "
                        << totalElapsedTime / 1000.0f << " s";
                if (numPacketsSent > 0)
                    cout << ", " << totalPacketBytesSent / numPacketsSent << " B/packet";
                if (totalElapsedTime > 0)
                    cout << ", " << totalPacketBytesSent * 1000 / totalElapsedTime << " B/s, "
                            << numPacketsSent * 1000 / totalElapsedTime << " packets/s";
                cout << ", " << theBurgerSpaceServer.getNumGameClients() << " client(s)";
                cout << endl;
            }
            #endif

            if (!theBurgerSpaceServer.update())
                break;

            theBurgerSpaceServer.sendPendingPacketsToClientSet();

            Uint32 now = SDL_GetTicks();
            if (frameEnd > now)
                SDL_Delay(frameEnd - now);
        }
    }
    catch (const string &s)
    {
        cerr << PROGRAM << ": server init error: " << s << endl;
        return EXIT_FAILURE;
    }
    catch (int e)
    {
        cerr << PROGRAM << ": init error # " << e << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
