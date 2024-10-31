#include <Game.h>
#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    Game g("../config.txt");
    g.run();

    return 0;
}