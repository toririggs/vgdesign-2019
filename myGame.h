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
#include "sprite.h"

using namespace std;

class MyGame :public Game {
	Image* background, * grid, * gev, * geh, * b1, * b2, * b3, * b4, * b5, * b6, * sphere, * wall, * wall2, * goal, * endScreen;
	int count = 0;
	int passX, passY;
	int wallX, wallY, wall2X, wall2Y;
	int goalX, goalY;
	vector<Sprite*> sprites;
public:
	MyGame() :Game("Karls Game", 1900, 1000) {}
	void init() {
		passX = 405;
		passY = 300;
		wallX = 400;
		wallY = 369;
		wall2X = 992;
		wall2Y = 443;
		goalX = 923;
		goalY = 372;
		background = new Image(this, "space.bmp", 0, 0);
		grid = new Image(this, "grid.bmp", 0, 0);
		gev = new Image(this, "gridedgev.bmp", 0, 0);
		geh = new Image(this, "gridedgeh.bmp", 0, 0);
		b1 = new Image(this, "pbs1.bmp", 0, 0);
		b2 = new Image(this, "pbs2.bmp", 0, 0);
		b3 = new Image(this, "pbs3.bmp", 0, 0);
		b4 = new Image(this, "pbs4.bmp", 0, 0);
		b5 = new Image(this, "pbs5.bmp", 0, 0);
		b6 = new Image(this, "pbs6.bmp", 0, 0);
		sphere = new Image(this, "pbs1.bmp", 61, 61);
		wall = new Image(this, "tempwall.bmp", 70, 70);
		wall2 = new Image(this, "tempwall.bmp", 70, 70);
		goal = new Image(this, "hole.bmp", 0, 0);
		endScreen = new Image(this, "temp.bmp", 0, 0);
		for (int i = 0; i < 2; i++) {
			Sprite* ball = new Sprite(this, "", 7, 10);
			sprites.push_back(ball);
			ball->random();
		}
		cout << "Finished init" << endl;
	}
	void interact(SDL_Event event) {
		if (event.key.keysym.sym == SDLK_RIGHT) { spheregoright(); }
		if (event.key.keysym.sym == SDLK_LEFT) { spheregoleft(); }
		if (event.key.keysym.sym == SDLK_UP) { spheregoup(); }
		if (event.key.keysym.sym == SDLK_DOWN) { spheregodown(); }
	}
	void update(float dt) {
		for (unsigned int i = 0; i < sprites.size(); i++)
			sprites[i]->update(dt);
	}
	void render() {
		background->render(this);
		for (int i = 400; i < 1500; i += 74) {
			for (int j = 295; j < 800; j += 74) {
				grid->render(this, i, j);
				if (j + 74 < 800) geh->render(this, i, j + 71);
				if (i + 74 < 1500) gev->render(this, i + 71, j);
			}
		}
		wall->render(this, wallX, wallY);
		wall2->render(this, wall2X, wall2Y);
		goal->render(this, goalX, goalY);
		sphere->render(this, passX, passY);
		if (count < 25) { b1->render(this, passX, passY); }
		else if (count < 50) { b6->render(this, passX, passY); }
		else if (count < 75) { b5->render(this, passX, passY); }
		else if (count < 100) { b4->render(this, passX, passY); }
		else if (count < 125) { b3->render(this, passX, passY); }
		else if (count < 150) { b2->render(this, passX, passY); }
		else { count = 0; }
		count++;
		SDL_RenderPresent(renderer);
	}
	bool collision(int objX, int objY, int objW, int objH, int wallX, int wallY, int wallW, int wallH, string direction)
	{
		int objRightX, objLeftX, objUpY, objDownY, wallRightX, wallLeftX, wallUpY, wallDownY;
		objRightX = objX + wallW;
		objLeftX = objX;
		objUpY = objY;
		objDownY = objY + wallH;
		wallRightX = wallX + wallW;
		wallLeftX = wallX;
		wallUpY = wallY;
		wallDownY = wallY + wallH;

		if (((objUpY == wallUpY && objDownY == wallDownY) && (objRightX == wallLeftX - 4)) && direction == "right") return true;
		else if (((objUpY == wallUpY && objDownY == wallDownY) && (objLeftX == wallRightX + 4)) && direction == "left") return true;
		else if (((objRightX == wallRightX && objLeftX == wallLeftX) && (objUpY == wallDownY + 4)) && direction == "up") return true;
		else if (((objRightX == wallRightX && objLeftX == wallLeftX) && (objDownY == wallUpY - 4)) && direction == "down") return true;
		else return false;
	}
	bool goalHit(int objX, int objY, int goalX, int goalY)
	{
		if (objX == goalX && objY == goalY)
		{
			SDL_Renderer* renderer = getRenderer();
			endScreen->render(this);
			SDL_RenderPresent(renderer);
			return true;
		}
		else return false;
	}
	void spheregoright()
	{
		string direction = "right";
		count = 0;
		if (passX != 1441) {
			SDL_Renderer* renderer = getRenderer();
			SDL_RenderPresent(renderer);
			for (passX; passX < 1442; passX++)
			{
				if (goalHit(passX, passY, goalX, goalY)) break;
				if (collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wallX, wallY, wall->getW(), wall->getH(), direction) ||
					collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wall2X, wall2Y, wall2->getW(), wall2->getH(), direction)) break;
				//now this is just render
				render();
			}
		}
	}
	void spheregoleft()
	{
		string direction = "left";
		count = 0;
		if (passX != 405) {
			SDL_Renderer* renderer = getRenderer();
			SDL_RenderPresent(renderer);
			for (passX; passX > 405; passX--)
			{
				if (goalHit(passX, passY, goalX, goalY)) break;
				if (collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wallX, wallY, wall->getW(), wall->getH(), direction) ||
					collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wall2X, wall2Y, wall2->getW(), wall2->getH(), direction)) break;
				render();
			}
		}
	}
	void spheregoup()
	{
		string direction = "up";
		count = 0;
		if (passY != 300) {
			SDL_Renderer* renderer = getRenderer();
			SDL_RenderPresent(renderer);
			for (passY; passY > 300; passY--)
			{
				if (goalHit(passX, passY, goalX, goalY)) break;
				if (collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wallX, wallY, wall->getW(), wall->getH(), direction) ||
					collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wall2X, wall2Y, wall2->getW(), wall2->getH(), direction)) break;
				render();
			}
		}
	}
	void spheregodown()
	{
		string direction = "down";
		count = 0;
		if (passY != 739) {
			SDL_Renderer* renderer = getRenderer();
			SDL_RenderPresent(renderer);
			for (passY; passY < 744; passY++)
			{
				if (goalHit(passX, passY, goalX, goalY)) break;
				if (collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wallX, wallY, wall->getW(), wall->getH(), direction) ||
					collision(passX - 5, passY - 5, sphere->getW(), sphere->getH(), wall2X, wall2Y, wall2->getW(), wall2->getH(), direction)) break;
				render();
			}
		}
	}
};
