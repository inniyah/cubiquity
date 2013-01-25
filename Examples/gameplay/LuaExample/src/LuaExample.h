#ifndef LUAEXAMPLE_H_
#define LUAEXAMPLE_H_

#include "gameplay.h"

using namespace gameplay;

/**
 * Main game class.
 *
 * This game has all the implementation code done in lua.
 */
class LuaExample: public Game
{
public:

    /**
     * Constructor.
     */
    LuaExample();

protected:

    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);
};


#endif //LUAEXAMPLE_H_
