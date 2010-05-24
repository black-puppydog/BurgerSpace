#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <flatzebra/GameEngine.h>
#include <string>

class Level
    {
    public:
        Level();
        ~Level();

        void init(int no, int nCols, int nRows, flatzebra::Couple posInPixels);
        void setLevelNo(int no);
        int  getLevelNo() const;
        void setTileMatrixEntry(int colNo, int rowNo,
                                const char **xpm, SDL_Surface *pixmap);
        SDL_Surface **getTileMatrixRow(int rowNo);
        const char **getXPMAtPixel(flatzebra::Couple pos) const;

        void setTextDescription(const std::string &desc);
        std::string getTextDescription() const;

    public:
        flatzebra::Couple sizeInTiles;
        flatzebra::Couple sizeInPixels;
        flatzebra::Couple positionInPixels;

    private:
        int levelNo;
        const char ***xpmMatrix;   // array of 'const char **' pointers
        SDL_Surface **tileMatrix;  // array of X11 pixmaps
        std::string desc;  // see BurgerSpaceServer::loadLevel()

        // Forbidden operations:
        Level(const Level &);
        Level &operator = (const Level &);
    };


#endif
