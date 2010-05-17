/*  $Id: LocalServer.cpp,v 1.1.2.2 2010/05/10 02:50:11 sarrazip Exp $
    LocalServer.cpp - Direct function call server

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

#include "LocalServer.h"

#include "EnemySprite.h"

#include <algorithm>

using namespace std;
using namespace flatzebra;

bool
LocalServer::saveGame(ostream &out)
{
    out << savedGameFormatVersion << "\n";
    if (out.fail())
        return false;
    out << "\n";

    out
        << initLevelNo << "\n"
        << cumulLevelNo << "\n"
        << tickCount << "\n"
        << serialize(initPlayerPos) << "\n"
        << serialize(*playerSprite) << "\n"
        << lastPlayerDirection << "\n"
        << oldMotionMode << "\n"
        ;

    serializeSpriteList(out, pepperSprites);

    out << timeForNewEnemy << "\n";

    serializeEnemySpriteList(out, enemySprites);

    out << "\n";
    out << ingredientGroups.size() << "\n";
    for (IngredientGroup::List::const_iterator it = ingredientGroups.begin();
                                            it != ingredientGroups.end(); it++)
    {
        const IngredientGroup &group = **it;
        out << "\n";

        out << group.getVerticalTarget() << " "
            << group.getBounceTime() << " "
            << group.getStallTime() << " "
            << (int) group.getState() << " "
            << group.getNumFloorsToGo() << " "
            << group.isTopBun() << "\n";

        // Write indexes in enemySprites for each carried enemy:
        const SpriteList &enemies = group.getCarriedEnemies();
        out << enemies.size();
        for (SpriteList::const_iterator jt = enemies.begin();
                                                jt != enemies.end(); jt++)
        {
            SpriteList::iterator kt = find(
                                enemySprites.begin(), enemySprites.end(), *jt);
            assert(kt != enemySprites.end());
            size_t index = size_t(distance(enemySprites.begin(), kt));
            out << " " << index;
        }
        out << "\n";

        for (size_t i = 0; i < 4; i++)
            out << serialize(*group.getMember(i)) << "\n";
    }
    out << "\n";

    serializeSpriteList(out, treatSprites);
    out << timeForTreat << "\n";

    serializeSpriteList(out, scoreSprites);

    out
        << numHamburgersToDo << "\n"
        << thePeakScore << "\n"
        << theScore << "\n"
        << celebrationMode << "\n"
        << numLives << "\n"
        << numAvailablePeppers << "\n"
        ;

    return out.good();
}


//virtual
int
LocalServer::loadGame(std::istream &in)
{
    int version;
    in >> version;
    if (!in)
        return -1;
    if (version != savedGameFormatVersion)
        return -2;

    in >> initLevelNo >> cumulLevelNo >> tickCount;
    if (!in)
        return -3;

    initNextLevel(cumulLevelNo);

    if (!deserialize(in, initPlayerPos))
        return -4;

    delete playerSprite;
    playerSprite = deserializeSprite(in);
    if (playerSprite == NULL)
        return -5;

    in >> lastPlayerDirection >> oldMotionMode;
    if (!in)
        return -6;

    if (!deserializeSpriteList(in, pepperSprites))
        return -7;

    in >> timeForNewEnemy;
    if (!in)
        return -8;

    if (!deserializeSpriteList(in, enemySprites, true))
        return -9;

    size_t numIngredientGroups;
    in >> numIngredientGroups;
    if (!in)
        return -10;

    ingredientSprites.clear();
    ingredientGroups.clear();

    for (size_t i = 0; i < numIngredientGroups; i++)
    {
        int verticalTarget, bounceTime, stallTime, state;
        size_t numFloorsToGo;
        bool isTopBun;
        in >> verticalTarget >> bounceTime >> stallTime
                        >> state >> numFloorsToGo >> isTopBun;
        if (!in)
            return -11;

        size_t numCarriedEnemies;
        in >> numCarriedEnemies;
        if (!in)
            return -12;

        vector<size_t> enemyIndexes;
        for (size_t j = 0; j < numCarriedEnemies; j++)
        {
            size_t index;
            in >> index;
            if (!in || index >= enemySprites.size())
                return -13;
            enemyIndexes.push_back(index);
        }

        IngredientGroup *group = new IngredientGroup(verticalTarget, isTopBun);
        group->restore(bounceTime, stallTime,
                        IngredientGroup::State(state), numFloorsToGo);

        // Read and insert the IngredientSprite members of this group:
        for (size_t k = 0; k < 4; k++)
        {
            IngredientSprite *is = deserializeIngredientSprite(in, group);
            if (is == NULL)
            {
                delete group;
                return -14;
            }
            group->setMember(k, is);
            is->currentPixmapIndex = k;
            ingredientSprites.push_back(is);
        }

        // Tell each carried enemy by which group it is carried:
        for (vector<size_t>::const_iterator jt = enemyIndexes.begin();
                                            jt != enemyIndexes.end(); jt++)
        {
            size_t enemyIndex = *jt;
            Sprite *s = enemySprites.at(enemyIndex);
            EnemySprite *es = dynamic_cast<EnemySprite *>(s);
            if (s == NULL || es == NULL)
            {
                delete group;
                return -15;
            }
            group->addCarriedEnemy(es);
        }


        ingredientGroups.push_back(group);
    }

    if (!deserializeSpriteList(in, treatSprites))
        return -16;
    in >> timeForTreat;
    if (!in)
        return -17;

    if (!deserializeSpriteList(in, scoreSprites))
        return -18;

    in
        >> numHamburgersToDo
        >> thePeakScore
        >> theScore
        >> celebrationMode
        >> numLives
        >> numAvailablePeppers
        ;
    if (!in)
        return -19;

    return 0;
}
