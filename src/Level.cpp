#ifndef _LEVEL_CPP_
#define _LEVEL_CPP_


#include <assert.h>
#include "Level.h"
#include "EnemySprite.h"
#include "util.h"

using namespace flatzebra;
using namespace std;

Level::Level()
:   sizeInTiles(),
    sizeInPixels(),
    positionInPixels(),
    levelNo(1),
    xpmMatrix(NULL),
    tileMatrix(NULL),
    desc()
{
}


Level::~Level()
{
    // NOTE:  We do not free the pixmaps.  The program is dying anyway.

    delete [] xpmMatrix;
    delete [] tileMatrix;
}


void
Level::init(int no, int nCols, int nRows, Couple posInPixels)
/*  nCols and nRows must be the number of columns and rows
    of _tiles_, not of pixels.
    posInPixels must be the offset in pixels from the origin
    of the drawing pixmap.
*/
{
    assert(no > 0);
    assert(nCols > 0);
    assert(nRows > 0);

    levelNo = no;

    sizeInTiles = Couple(nCols, nRows);
    sizeInPixels = sizeInTiles * TILE_SIDE;

    positionInPixels = posInPixels;

    delete [] xpmMatrix;
    xpmMatrix = new const char **[nRows * nCols];
    memset(xpmMatrix, 0, nRows * nCols * sizeof(const char **));

    delete [] tileMatrix;
    tileMatrix = new SDL_Surface *[nRows * nCols];
    memset(tileMatrix, 0, nRows * nCols * sizeof(SDL_Surface *));
}


void
Level::setLevelNo(int no)
{
    assert(no >= 1);
    levelNo = no;
}


int
Level::getLevelNo() const
{
    assert(levelNo >= 1);
    return levelNo;
}


void
Level::setTileMatrixEntry(int colNo, int rowNo,
                                        const char **xpm, SDL_Surface *pixmap)
{
    assert(colNo < sizeInTiles.x);
    assert(rowNo < sizeInTiles.y);

    xpmMatrix[rowNo * sizeInTiles.x + colNo] = xpm;
    tileMatrix[rowNo * sizeInTiles.x + colNo] = pixmap;
}


SDL_Surface **
Level::getTileMatrixRow(int rowNo)
{
    return &tileMatrix[rowNo * sizeInTiles.x];
}


const char **
Level::getXPMAtPixel(Couple pos) const
/*  Returns a pointer to the XPM variable that corresponds to
    the tile that contains the pixels at 'pos'.
    Returns NULL if 'pos' is in no tile.
*/
{
    pos -= positionInPixels;
    if (pos.x < 0 || pos.x >= sizeInPixels.x ||
            pos.y < 0 || pos.y >= sizeInPixels.y)
        return NULL;

    pos /= TILE_SIDE;
    return xpmMatrix[pos.y * sizeInTiles.x + pos.x];
}


void
Level::setTextDescription(const string &desc)
{
    this->desc = desc;
}


string
Level::getTextDescription() const
{
    return desc;
}


#endif
