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

using namespace std;

#ifdef DEBUG
#define debug(str) cout << str << endl;
#else
#define debug(str) 
#endif

typedef pair<Image*, int /*time */> frameWithTime;

class Animation {
	vector < frameWithTime > frames;
	int time, totalTime, frame; /* in ms */
public:
	Animation(Game* game, string path, int count, int fps = 10) {
		totalTime = 0;
		time = 0;
		for (int i = 1; i < count; i++) {
			stringstream filename;
			filename << "pbs" << i << ".bmp";
			Image* img = new Image(game, filename.str(), 0, 0);
			frames.push_back(frameWithTime(img, 1000 / fps));
			totalTime += 1000 / fps;
		}
		frame = 0;
	}
	void random() {
		time = rand() % totalTime;
	}
	void update(float dt) {
		debug("Update "); debug(dt);
		time += (int)dt;
		time = time % totalTime;
		int currentTime = time;
		frame = 0;
		for (int i = 0; i < frames.size(); i++) {
			if (currentTime < frames[i].second) {
				frame = i;
				break;
			}
			currentTime -= frames[i].second;
		}
	}
	SDL_Rect getSize() {
		SDL_Rect neverHappen;
		if (frames.size() > 0) return frames[frame].first->getSize();
		//return neverHappen;
	}
	void render(Game * game, int x = 0, int y = 0) {
		frames[frame].first->render(game, x, y);
	}
};
