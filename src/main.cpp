#include <game.h>
#include <iostream>

int main() {
    Game game;

    int s;
    do {
        game.display();
        s = game.step();
    } while(s == 0);

    if (s <= 10) {
        std::cout << "-----------------------\n";
        std::cout << "        VICTORY!       \n";
        std::cout << "-----------------------\n";
        std::cout << "You took the hill with a score of " << s << "\n";
    } else if (s <= 15) {
        std::cout << "-----------------------\n";
        std::cout << "          DRAW         \n";
        std::cout << "-----------------------\n";
        std::cout << "You took the hill, but the score was " << s << "\n";
    } else {
        std::cout << "-----------------------\n";
        std::cout << "         DEFEAT        \n";
        std::cout << "-----------------------\n";
        std::cout << "The battle was lost with a score of " << s << "\n";
    }

    return 0;
}
