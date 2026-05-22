#include <game.h>

int main() {
    Game game;

    while (true) {
        game.display();
        game.step();
    }

    return 0;
}
