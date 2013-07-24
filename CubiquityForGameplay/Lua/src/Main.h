#ifndef MAIN_H_
#define MAIN_H_

#include "gameplay.h"

using namespace gameplay;

/**
 * Main game class.
 *
 * This game has all the implementation code done in lua.
 */
class Main: public Game
{
public:

    /**
     * Constructor.
     */
    Main();

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


#endif //MAIN_H_
