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
#include <fstream>

#include "game.h"
#include "image.h"
#include "animation.h"
#include "sprite.h"

using namespace std;

class MyGame:public Game {
	Image *background, *grid, *gev, *geh, *b1, *b2, *b3, *b4, *b5, *b6, *sphere, *goal, *endScreen, *nextScreen, *replay, *menu, *nextArrow;
	Image *wallImages[10];
	pair<int, int> walls[10];
	int count = 0;
	int levelnum = 0;
	int passX, passY;
	int goalX, goalY;
	int maxX, maxY;
	int lBallX, lBallY, lGoalX, lGoalY, numWalls;
	bool nextlevel = false;
	bool outoflevels = false;
	vector<Sprite *> sprites;
	public:
	MyGame():Game("Karls Game",1900,1000){}
	void readLevel() {
		ifstream fin;
		stringstream sstream;
		int tempx, tempy;
		
		for (int i = 0; i < 10; i++) walls[i] = make_pair (-100, -100);
		
		nextlevel = false;
		levelnum++;
		sstream << levelnum;
		if (levelnum == 6) outoflevels = true;
		//string level = "samplelevel.txt";
		string level = "level" + sstream.str() + ".txt";
		
		fin.open(level.c_str());
		
		fin >> maxX >> maxY >> lBallX >> lBallY >> lGoalX >> lGoalY >> numWalls;
		
		for (int i = 0; i < (numWalls); i++)
		{
			fin >> tempx >> tempy;
			walls[i] = make_pair (tempx, tempy);
		}
		
		fin.close();
	}
	void init() {
		readLevel();
		passX=lBallX;
		passY=lBallY;
		goalX=lGoalX;
		goalY=lGoalY;
		background=new Image(this,"space.bmp", 0, 0);
		grid=new Image (this, "grid.bmp", 0, 0);
		gev=new Image (this, "gridedgev.bmp", 0, 0);
		geh=new Image (this, "gridedgeh.bmp", 0, 0);
		b1 = new Image (this, "pbs1.bmp", 0, 0);
		b2 = new Image (this, "pbs2.bmp", 0, 0);
		b3 = new Image (this, "pbs3.bmp", 0, 0);
		b4 = new Image (this, "pbs4.bmp", 0, 0);
		b5 = new Image (this, "pbs5.bmp", 0, 0);
		b6 = new Image (this, "pbs6.bmp", 0, 0);
		sphere = new Image (this, "pbs1.bmp", 61, 61);
		for (int i = 0; i < numWalls; i++) wallImages[i] = new Image (this, "tempwall.bmp", 70, 70);
		goal = new Image (this, "hole.bmp", 0, 0);
		endScreen = new Image (this, "temp.bmp", 0, 0);
		nextScreen = new Image (this, "nextlevelscreen.bmp", 0, 0);
		replay = new Image (this, "replayArrow.bmp", 0, 0);
		menu = new Image (this, "menuicon.bmp", 0, 0);
		nextArrow = new Image (this, "nextarrow.bmp", 0, 0);
		for (int i=0;i<2;i++) {
			Sprite *ball=new Sprite(this,"",7,10);
			sprites.push_back(ball);
			ball->random();
		}
		cout << "Finished init" << endl;
	}
	void interact(SDL_Event event) {
			  if (event.key.keysym.sym==SDLK_RIGHT){spheregoright();}
			  if (event.key.keysym.sym==SDLK_LEFT) {spheregoleft();}
			  if (event.key.keysym.sym==SDLK_UP) {spheregoup();}
			  if (event.key.keysym.sym==SDLK_DOWN) {spheregodown();}
	}
	void update(float dt) {
		for (unsigned int i=0;i<sprites.size();i++)
			sprites[i]->update(dt);
	}
	void render() {
		background->render(this);
				for (int i = 400; i < maxX; i +=74){
					for (int j = 295; j < maxY; j+=74) {
						grid->render(this,i,j);
						if(j+74<maxY) geh->render(this,i,j+71);
						if(i+74<maxX) gev->render(this,i+71,j); 
						}
				}
				for (int i = 0; i < numWalls; i++) wallImages[i]->render(this, walls[i].first, walls[i].second);
				goal->render(this,goalX,goalY);
				sphere->render(this,passX,passY);
				if (count < 25) {b1->render(this,passX,passY);}
				else if (count < 50) {b6->render(this,passX,passY);}
				else if (count < 75) {b5->render(this,passX,passY);}
				else if (count < 100) {b4->render(this,passX,passY);}
				else if (count < 125) {b3->render(this,passX,passY);}
				else if (count < 150) {b2->render(this,passX,passY);}
				else {count = 0;}
				count++;
				if (outoflevels) {
					SDL_Renderer *renderer=getRenderer();
					endScreen->render(this);
					SDL_RenderPresent(renderer);
				}
				SDL_RenderPresent(renderer);
	}
	bool collision (int objX, int objY, int objW, int objH, int wallX, int wallY, int wallW, int wallH, string direction) 
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
		
		if ( ((objUpY == wallUpY && objDownY == wallDownY) && (objRightX == wallLeftX-4)) && direction == "right") return true;
		else if ( ((objUpY == wallUpY && objDownY == wallDownY) && (objLeftX == wallRightX+4)) && direction == "left") return true;
		else if ( ((objRightX == wallRightX && objLeftX == wallLeftX) && (objUpY == wallDownY+4)) && direction == "up") return true;
		else if ( ((objRightX == wallRightX && objLeftX == wallLeftX) && (objDownY == wallUpY-4)) && direction == "down") return true;
		else return false;
	}
	bool goalHit (int objX, int objY, int goalX, int goalY)
	{
		if (outoflevels && objX == goalX && objY == goalY) 
		{
			SDL_Renderer *renderer=getRenderer();
			endScreen->render(this);
			SDL_RenderPresent(renderer);
			return true;
		}
		else if (objX == goalX && objY == goalY)
		{
			nextlevel = true;
			bool next = false;
			SDL_Renderer *renderer=getRenderer();
			nextScreen->render(this);
			replay->render(this, 275, 400);
			menu->render(this, 575, 400);
			nextArrow->render(this, 875, 400);
			SDL_RenderPresent(renderer);
			SDL_Event event;
			while (!next) {
				if (SDL_PollEvent(&event)) {
					if (event.key.keysym.sym==SDLK_n) next = true;
					if (event.key.keysym.sym==SDLK_r) {levelnum--; next = true;}
					// if (even.key.keysym.sym==SDLK_m) {add functionality to return to main menu;}
				}
			}
			return true;
		}
		else return false;
	}
	void spheregoright() 
	{
		string direction = "right";
		bool Break = false;
		count = 0;
		if (passX != maxX && !nextlevel) {
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passX;passX<maxX+1;passX++) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				for (int i = 0; i < numWalls; i++) if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),walls[i].first,walls[i].second,wallImages[i]->getW(),wallImages[i]->getH(), direction)) Break = true;
				//now this is just render
				if (Break) break;
				render();
			}
			if (nextlevel) {
				init();	
				render();
			}
		}
	}
	void spheregoleft()
	{
		string direction = "left";
		bool Break = false;
		count = 0;
		if (passX != 405 && !nextlevel) {
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passX;passX>405;passX--) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				for (int i = 0; i < numWalls; i++) if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),walls[i].first,walls[i].second,wallImages[i]->getW(),wallImages[i]->getH(), direction)) Break = true;
				if (Break) break;
				render();
			}	
			if (nextlevel) {
				init();
				render();
			}
		}
	}
	void spheregoup()
	{
		string direction = "up";
		bool Break = false;
		count = 0;
		if (passY != 300 && !nextlevel) {
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passY;passY>300;passY--) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				for (int i = 0; i < numWalls; i++) if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),walls[i].first,walls[i].second,wallImages[i]->getW(),wallImages[i]->getH(), direction)) Break = true;
				if (Break) break;
				render();
			}	
			if (nextlevel) {
				init();
				render();
			}
		}
	}
	void spheregodown()
	{
		string direction = "down";
		bool Break = false;
		count = 0;
		if (passY != maxY && !nextlevel) {
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passY;passY<maxY;passY++) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				for (int i = 0; i < numWalls; i++) if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),walls[i].first,walls[i].second,wallImages[i]->getW(),wallImages[i]->getH(), direction)) Break = true;
				if (Break) break;
				render();
			}	
			if (nextlevel) {
				init();
				render();
			}
		}
	}
};
