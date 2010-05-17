/*  $Id: util.h,v 1.1.2.20 2010/04/24 22:57:06 sarrazip Exp $
    util.h - Miscellaneous simply definitions.

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

#ifndef _H_util
#define _H_util

#include <string>
#include <stdlib.h>


/** Possible directions of movement, in trigonometric order.
    This enumeration is garanteed to start at zero.
    The order of the constants is guaranteed, so they are 0, 1, 2, 3.
*/
enum { RIGHT, UP, LEFT, DOWN };


// Graphical dimensions.
enum
{
    TILE_SIDE = 32,
    SCREEN_WIDTH_IN_PIXELS = 672,
    SCREEN_HEIGHT_IN_PIXELS = 520
};


// Network-related constants.
enum
{
    MAX_LEN_PACKET = 1024,
    UPDATE_SPRITE_PACKET_LEN = 12
};


enum ServerPacketType
{
    UPDATE_LEVEL_PACKET,
    UPDATE_SPRITE_PACKET,
    UPDATE_SCORE_PACKET,
    PLAY_SOUND_EFFECT_PACKET,
    ROLE_ASSIGNMENT_PACKET
};


enum ClientPacketType
{
    CLIENT_DESC_PACKET,
    START_NEW_GAME_PACKET,
    LEVEL_DESC_REQUEST_PACKET,
    PLAYER_COMMAND_PACKET,
    DISCONNECT_PACKET
};


enum Role
{
    ROLE_SPECTATOR,
    ROLE_CHEF,
    ROLE_ENEMY
};


enum EnemyType
{
    ENEMY_EGG,
    ENEMY_HOT_DOG,
    ENEMY_PICKLE
};


inline
std::string
getDir(const char *defaultValue, const char *envVarName)
/*
    Makes sure that the returned directory name ends with a slash.
*/
{
    std::string dir;
    const char *s = getenv(envVarName);
    if (s != NULL)
        dir = s;
    else
        dir = defaultValue;
    
    if (!dir.empty() && dir[dir.length() - 1] != '/')
        dir += '/';

    return dir;
}


#endif  /* _H_util */
