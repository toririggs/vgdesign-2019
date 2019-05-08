#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <time.h>

#include "game.h"
#include "image.h"
#include "animation.h"
#include "sprite.h"
#include "myGame.h"

using namespace std;

int main(int argc, char* argv[]) {
	Game* g;
	g = new MyGame();
	g->run();
	delete g;
	return 0;
}
