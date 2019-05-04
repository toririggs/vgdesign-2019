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
#include "image.h"
#include "animation.h"

using namespace std;

class Sprite :public Animation {
	float x, y, dx, dy, ax, ay;
	int gw, gh;
	bool dead;
public:
	bool within(float otherx, float othery) {
		SDL_Rect us = getSize();
		return x <= otherx && otherx <= x + us.w && y <= othery && othery <= y + us.h;
	}

	Sprite(Game* g, string filename, int count, int fps, float newX = 0.0, float newY = 0.0, float newDx = 0.0, float newDy = 0.0, float newAx = 0.0, float newAy = 0.0) :
		Animation(g, filename, count, fps) {
		dead = false;
		x = newX;
		y = newY;
		dx = newDx;
		dy = newDy;
		ax = newAx;
		ay = newAy;
		SDL_Rect gsize = g->getSize();
		gw = gsize.w;
		gh = gsize.h;
	}
	void random() {
		Animation::random();
		x = (float)(rand() % gw);
		y = (float)(rand() % gh);
		dx = (float)((rand() % (40 * 100)) - 2000) / 100.0;
		dy = (float)((rand() % (40 * 100)) - 2000) / 100.0;
	}
	void update(float dt /* in ms */) {
		if (dead) return;
		Animation::update(dt);
		dt /= 1000.0;
		dx = dx + ax * dt;
		dy = dy + ay * dt;
		x = x + dx * dt;
		y = y + dy * dt;
		SDL_Rect src = getSize();
		if (y<0 || y>gh - src.h) dy = -dy;
		if (x<0 || x>gw - src.w) dx = -dx;
	}
	float getPositionX() { return x; }
	float getPositionY() { return y; }
	float getVelocityX() { return dx; }
	float getVelocityY() { return dy; }
	float getAccelerationX() { return ax; }
	float getAccelerationY() { return ay; }
	void setAcceleration(float newAx, float newAy) {
		ax = newAx;
		ay = newAy;
	}
	void kill() {
		dead = true;
	}
	void setVelocity(float newDx, float newDy) {
		dx = newDx;
		dy = newDy;
	}
	void setPosition(float newx, float newy) {
		x = newx;
		y = newy;
	}
	void render(Game * g, int setX = -1, int setY = -1) {
		if (dead) return;
		if (setX != -1 && setY != -1) Animation::render(g, setX, setY);
		else Animation::render(g, (int)x, (int)y);
	}
};
