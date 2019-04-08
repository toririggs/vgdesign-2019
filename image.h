#pragma once

#include "SDL.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <time.h>

#include "game.h"

using namespace std;

class Image {
	SDL_Texture* texture;
	int w, h;
protected:
	SDL_Rect src;
public:
	Image(Game* game, string filename, int newW, int newH) {
		w = newW;
		h = newH;
		SDL_Surface* surface;
		surface = SDL_LoadBMP(filename.c_str());
		// The window is open: could enter program loop here (see SDL_PollEvent())
		if (surface == NULL) {
			cout << "Something happened" << endl;
			return;
		}
		Uint32 colorKey = SDL_MapRGB(surface->format, 155, 100, 0);
		SDL_SetColorKey(surface, SDL_TRUE, colorKey);
		SDL_Renderer* renderer = game->getRenderer();
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == NULL) {
			fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
		}
		SDL_QueryTexture(texture, NULL, NULL, &(src.w), &(src.h));
		src.x = 0;
		src.y = 0;
		SDL_FreeSurface(surface);
	}
	SDL_Rect getSize() {
		return src;
	}

	void render(Game* game, int x = 0, int y = 0) {
		SDL_Rect dest;
		dest.w = src.w;
		dest.h = src.h;
		dest.x = x;
		dest.y = y;
		SDL_Renderer* renderer = game->getRenderer();
		SDL_RenderCopy(renderer, texture, &src, &dest);
	}
	int getW()
	{
		return w;
	}
	int getH()
	{
		return h;
	}
};