#include <game.h>
#include <iostream>

int main() {
    Game game;

    int s;
    do {
        game.display();
        s = game.step();
    } while(s == 0);

    switch(s) {
    case 16:
        std::cout << "you lost\n";
        break;
    default:
        std::cout << "you won in " << s << " turns\n";
        break;
    }

    return 0;
}
