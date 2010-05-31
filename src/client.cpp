/*  $Id: client.cpp,v 1.1.2.29 2010/05/10 02:40:38 sarrazip Exp $
    sdlmain.cpp - main() function for BurgerSpace

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

#include "LocalServer.h"
#include "RemoteServer.h"
#include "util.h"

#include <errno.h>
#include <time.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

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
    { "ms-per-frame",  required_argument, NULL, 'm' },
    { "no-sound",      no_argument,       NULL, 'n' },
    { "full-screen",   no_argument,       NULL, 'f' },
    { "old-motion",    no_argument,       NULL, 'o' },
    { "z-for-pepper",  no_argument,       NULL, 'z' },
    { "server",        required_argument, NULL, 's' },
    { "port",          required_argument, NULL, 'p' },
    { "chef",          no_argument,       NULL, 'c' },
    { "enemy",         no_argument,       NULL, 'e' },
    { "levelset",      required_argument, NULL, 'l' },

    { NULL, 0, NULL, 0 }  // marks the end
};


static
void
displayVersionNo()
{
    cout << PROGRAM << " " << VERSION << endl;
}


static
void
displayHelp()
{
    cout << "\n";

    displayVersionNo();

    cout <<
"\n"
"Copyright (C) 2001-2010 Pierre Sarrazin <http://sarrazip.com/>\n"
"This program is free software; you may redistribute it under the terms of\n"
"the GNU General Public License.  This program has absolutely no warranty.\n"
    ;

    cout <<
"\n"
"Known options:\n"
"--help             Display this help page and exit\n"
"--version          Display this program's version number and exit\n"
"--initial-level=N  Start game at level N (N >= 1) [default=1]\n"
"--ms-per-frame=N   N milliseconds per animation frame [default=55]\n"
"                   Min. 1, max. 1000.  50 ms means 20 frames per second\n"
"--no-sound         Disable sound effects [default is to enable them]\n"
"--full-screen      Attempt to use full screen mode [default is window mode]\n"
"--old-motion       Use the old player motion system [default is new system]\n"
"--z-for-pepper     Use Z key to shoot pepper [default is Ctrl key]\n"
"--server=HOSTNAME  Run as a network client and use specified server\n"
"--port=PORT        Specify UDP port of server (only relevant with --server)\n"
"--chef             In network game, play the role of the chef\n"
"--enemy            In network game, play the role of one of the enemies\n"
"\n"
    ;
}
#endif  /* HAVE_GETOPT_LONG */


static RemoteServer *
createRemoteServer(BurgerSpaceClient &client,
                   const string &serverHostname,
                   unsigned short port,
                   Role role,
                   int &errorCode)
{
    // Try to resolve the server hostname:
    hostent *ent = gethostbyname(serverHostname.c_str());
    if (ent == NULL)
    {
        errorCode = -1;
        return NULL;
    }

    try
    {
        // Create and initialize an object representing the remote server:
        RemoteServer *rs = new RemoteServer(client,
                                            (unsigned char *) ent->h_addr_list[0],
                                            port,
                                            role);
        rs->describeClient();
        rs->requestLevelDescription();
        errorCode = 0;
        return rs;
    }
    catch (int errNo)
    {
        errorCode = errNo;
        return NULL;
    }
}


int
main(int argc, char *argv[])
{
    /*  Default game parameters:
    */
    int initLevelNo = 1;
    int millisecondsPerFrame = 55;
    bool useSound = true;
    bool fullScreen = false;
    bool useOldMotionMode = false;
    SDLKey pepperKey = SDLK_LCTRL;
    string serverHostname;  // UDP server hostname if not empty; local server otherwise
    unsigned short port = DEFAULT_UDP_SERVER_PORT;
    Role role = ROLE_SPECTATOR;
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
                        cout << "Invalid initial level number.\n";
                        displayHelp();
                        return EXIT_FAILURE;
                    }

                    initLevelNo = int(n);
                }
                break;

            case 'm':
                {
                    errno = 0;
                    long n = strtol(optarg, NULL, 10);
                    if (errno == ERANGE || n < 1 || n > 1000)
                    {
                        cout << "Invalid number of ms per frame.\n";
                        displayHelp();
                        return EXIT_FAILURE;
                    }

                    millisecondsPerFrame = int(n);
                }
                break;

            case 'l':
		levelFile = optarg;
                break;

            case 'n':
                useSound = false;
                break;

            case 'f':
                fullScreen = true;
                break;

            case 'o':
                useOldMotionMode = true;
                break;

            case 'z':
                pepperKey = SDLK_z;
                break;

            case 's':
                serverHostname = optarg;
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

            case 'c':
                role = ROLE_CHEF;
                break;

            case 'e':
                role = ROLE_ENEMY;
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


    try
    {
        bool standAlone = serverHostname.empty();

        BurgerSpaceClient theBurgerSpaceClient(
                                standAlone ? ROLE_CHEF : role,
                                !standAlone /* show role */,
                                useSound, pepperKey, fullScreen,
                                standAlone);

        BurgerSpaceServerInterface *serverInterface = NULL;

        if (standAlone)
        {
            // Create a "local server":
            LocalServer *ls = new LocalServer(theBurgerSpaceClient, initLevelNo, useOldMotionMode, levelFile);
            ls->finishInit();
            serverInterface = ls;
        }
        else  // remote server version:
        {
            // Upon a successful connection, createRemoteServer() sends
            // a description of this client to the server and requests
            // a level update from the server.
            //
            int errorCode = 0;
            serverInterface = createRemoteServer(theBurgerSpaceClient, serverHostname, port, role, errorCode);
            if (serverInterface == NULL)
            {
                switch (errorCode)
                {
                    case -1:
                        cerr << PROGRAM << ": failed to resolve hostname "
                                        << serverHostname << endl;
                        break;
                    default:
                        assert(errorCode > 0);
                        cerr << PROGRAM << ": failed to initialize UDP socket for server "
                                        << serverHostname << ":" << port << ": "
                                        << strerror(errorCode) << endl;
                }
                return EXIT_FAILURE;
            }

            cout << PROGRAM << ": using server " << serverHostname << " on port " << port << endl;
        }

        // Associate the created server object with the client,
        // and run the game:
        theBurgerSpaceClient.setServer(serverInterface);
        theBurgerSpaceClient.runClientMode(millisecondsPerFrame);
        serverInterface->disconnect();  // in remote mode, this notifies server of client's departure

        delete serverInterface;
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
