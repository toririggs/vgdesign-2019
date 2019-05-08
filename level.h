#pragma once

#include "SDL2/SDL.h"
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

using namespace std;


class Level {
public:
	Level(Game *newG) {
		g = newG;
		tile = new Image(g, "grid.bmp", 70, 70);
		wall = new Image(g, "tempwall.bmp", 70, 70);
		hole = new Image(g, "hole.bmp", 70, 70);

		src.x = src.y = 100;
		src.w = dest.w = 100;
		src.h = dest.h = 100;

		dest.x = dest.y = 0;
	}

	void loadLevel(int arr[10][19]) {
		for (int row = 0; row < 10; row++)
		{
			for (int col = 0; col < 19; col++)
			{
				level[row][col] = arr[row][col];
			}
		}
	}
	void drawLevel() {
		int type = 0;
		for (int row = 0; row < 10; row++)
		{
			for (int col = 0; col < 19; col++)
			{
				type = level[row][col];

				dest.x = col * 100;
				dest.y = row * 100;

				switch (type)
				{
				case 0:
					break;
				case 1:
					tile->render(g, dest.x, dest.y);
					break;
				case 2:
					wall->render(g, row, col);
					break;
				case 3:
					hole->render(g, dest.x, dest.y);
					break;
				default:
					break;
				}

			}
		}
	}

private:
	Game* g;
	SDL_Rect src, dest;
	Image* tile;
	Image* wall;
	Image* hole;

	int level[10][19];
};
