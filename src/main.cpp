#include <game.h>

int main() {
    Game game;

    game.set(1, 4, 1);
    game.display();

    game.set(1, 3, 1);
    game.display();

    return 0;
}
