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

using namespace std;

class Game {
	SDL_Window* window;
public:SDL_Renderer* renderer; //this had to be made public
	   SDL_Rect size;
	   bool done = false;
public:
	Game(string name, int w = 640, int h = 480) {
		srand(time(NULL));
		size.w = w;
		size.h = h;
		window = NULL;
		renderer = NULL;
		SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
		// Create an application window with the following settings:
		window = SDL_CreateWindow(
			name.c_str(),                  // window title
			SDL_WINDOWPOS_UNDEFINED,           // initial x position
			SDL_WINDOWPOS_UNDEFINED,           // initial y position
			w,                               // width, in pixels
			h,                               // height, in pixels
			SDL_WINDOW_OPENGL                  // flags - see below
		);
		// Check that the window was successfully created
		if (window == NULL) {
			// In the case that the window could not be made...
			printf("Could not create window: %s\n", SDL_GetError());
			return;
		}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer == NULL) {
			// In the case that the window could not be made...
			printf("Could not create renderer: %s\n", SDL_GetError());
			return;
		}
	}
	SDL_Rect getSize() { return size; }
	SDL_Renderer* getRenderer() { return renderer; }
	SDL_Window* getWindow() { return window; }
	~Game() {
		// Close and destroy the window
		SDL_DestroyWindow(window);
		// Clean up
		SDL_Quit();
	}
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void render() = 0;
	virtual void interact(SDL_Event event) = 0;
	void run() {
		SDL_Event event;
		unsigned start = SDL_GetTicks();

		init();
		render();
		while (!done) {
			unsigned end = SDL_GetTicks();
			float dt = (end - start);
			start = end;
			if (SDL_PollEvent(&event)) {
				if (event.key.keysym.sym == SDLK_ESCAPE) { done = true; }
				else { interact(event); }
			}
		}
	}
};
