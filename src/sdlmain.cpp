/*  $Id: sdlmain.cpp,v 1.13 2009/02/27 02:50:42 sarrazip Exp $
    sdlmain.cpp - main() function for BurgerSpace

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

#include "BurgerSpaceEngine.h"

#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string>

#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
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

    { NULL, 0, NULL, 0 }  // marks the end
};


static
void
displayVersionNo()
{
    printf("%s %s\n", PACKAGE, VERSION);
}


static
void
displayHelp()
{
    printf("\n");

    displayVersionNo();

    printf(
"\n"
"Copyright (C) 2001-2007 Pierre Sarrazin <http://sarrazip.com/>\n"
"This program is free software; you may redistribute it under the terms of\n"
"the GNU General Public License.  This program has absolutely no warranty.\n"
    );

    printf(
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
"\n"
    );
}
#endif  /* HAVE_GETOPT_LONG */


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
			printf("Invalid initial level number.\n");
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
			printf("Invalid number of ms per frame.\n");
			displayHelp();
			return EXIT_FAILURE;
		    }

		    millisecondsPerFrame = int(n);
		}
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
    fprintf(stderr, "seed = %u\n", seed);
    fprintf(stderr, "init-level-no = %d\n", initLevelNo);
    fprintf(stderr, "ms-per-frame = %d\n", millisecondsPerFrame);
    #endif
    srand(seed);


    try
    {
	BurgerSpaceEngine theBurgerSpaceEngine("BurgerSpace",
		initLevelNo, useSound, fullScreen, useOldMotionMode, pepperKey);
	theBurgerSpaceEngine.run(millisecondsPerFrame);
    }
    catch (const string &s)
    {
	fprintf(stderr, "%s: engine init error: %s\n", PACKAGE, s.c_str());
	return EXIT_FAILURE;
    }
    catch (int e)
    {
	fprintf(stderr, "%s: init error # %d\n", PACKAGE, e);
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
