// continuing from feb 25
// -static in build commands says you want libraries in executable

#include "SDL.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <time.h>

#ifdef DEBUG
  #define debug(str) cout << str << endl;
#else
  #define debug(str) 
#endif

using namespace std;

class Game {
	SDL_Window *window;
    public:SDL_Renderer *renderer; //this had to be made public
    SDL_Rect size;
    bool done = false;
	public:
	  Game(string name,int w=640,int h=480) {
		srand (time(NULL));
		size.w=w;
		size.h=h;
		window=NULL;
		renderer=NULL;
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
	  SDL_Renderer *getRenderer() { return renderer; }
	  SDL_Window *getWindow() { return window; }
	  ~Game() {
		// Close and destroy the window
        SDL_DestroyWindow(window);
        // Clean up
        SDL_Quit();
	  }
      virtual void init()=0;
      virtual void update(float dt)=0;
      virtual void render()=0;
      virtual void interact(SDL_Event event)=0; 
	  void run() {
		  SDL_Event event;
		  unsigned start=SDL_GetTicks();
		  
		  init();
		  render();
		  while(!done) {
			  unsigned end=SDL_GetTicks();
			  float dt=(end-start);
			  start=end;
			  if (SDL_PollEvent(&event)) {
				if (event.key.keysym.sym==SDLK_ESCAPE) {done=true;}
				else {interact(event);}
			}
		}
	}
};

class Image {
	SDL_Texture *texture;
	int w,h;
	protected:
	SDL_Rect src;
	public:
	Image(Game *game,string filename, int newW, int newH) {
		w = newW;
		h = newH;
		SDL_Surface *surface;
		surface=SDL_LoadBMP(filename.c_str());
		// The window is open: could enter program loop here (see SDL_PollEvent())
		if (surface==NULL) {
			cout << "Something happened" << endl;
			return;
		}
		Uint32 colorKey=SDL_MapRGB(surface->format,155,100,0);
		SDL_SetColorKey(surface,SDL_TRUE,colorKey);
	    SDL_Renderer *renderer=game->getRenderer();
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == NULL) {
			fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
		}
		SDL_QueryTexture(texture,NULL,NULL,&(src.w),&(src.h));
		src.x=0;
		src.y=0;
		SDL_FreeSurface(surface);
	}
	SDL_Rect getSize() {
		return src;
	}

	void render(Game *game,int x=0,int y=0) {
		SDL_Rect dest;
		dest.w=src.w;
		dest.h=src.h;
		dest.x=x;
		dest.y=y;
	    SDL_Renderer *renderer=game->getRenderer();
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

typedef pair<Image *,int /*time */> frameWithTime;

class Animation {
	vector < frameWithTime > frames;
	int time,totalTime,frame; /* in ms */
	public:
	Animation(Game *game, string path,int count,int fps=10) {
		totalTime=0;
		time=0;
		for (int i=1; i<count; i++) {
  		  stringstream filename;
		  filename << "pbs" << i << ".bmp";
		  Image *img=new Image(game,filename.str(), 0, 0);
		  frames.push_back(frameWithTime(img,1000/fps));
		  totalTime+=1000/fps;
	    }
	    frame=0;
	}
	void random() {
		time=rand() % totalTime;
	}
	void update(float dt) {
		debug("Update "); debug(dt);
		time+=(int)dt;
		time=time % totalTime;
		int currentTime=time;
		frame=0;
		for (int i=0;i<frames.size();i++) {
		  if (currentTime<frames[i].second) {
			  frame=i;
			  break;
		  }
		  currentTime-=frames[i].second;
	    }
	}
	SDL_Rect getSize() {
		SDL_Rect neverHappen;
		if (frames.size()>0) return frames[frame].first->getSize();
		return neverHappen;
	}
	void render(Game *game,int x=0,int y=0) {
	   frames[frame].first->render(game,x,y);
	}
};

class Sprite:public Animation {
	float x,y,dx,dy,ax,ay;
	int gw,gh;
	bool dead;
	public:
	bool within(float otherx, float othery) {
		SDL_Rect us=getSize();
		return x<=otherx && otherx<=x+us.w && y<= othery && othery<=y+us.h;
	}

	Sprite(Game *g,string filename,int count,int fps,float newX=0.0,float newY=0.0,float newDx=0.0,float newDy=0.0,float newAx=0.0,float newAy=0.0):
	  Animation(g,filename,count,fps) {
		  dead=false;
		  x=newX;
		  y=newY;
		  dx=newDx;
		  dy=newDy;
		  ax=newAx;
		  ay=newAy;
		  SDL_Rect gsize=g->getSize();
		  gw=gsize.w;
		  gh=gsize.h;
	}
	void random() {
		Animation::random();
		x=(float)(rand() % gw);
		y=(float)(rand() % gh);
		dx=(float)((rand()% (40*100))-2000)/100.0;
		dy=(float)((rand()% (40*100))-2000)/100.0;
	}
	void update(float dt /* in ms */) {
		if (dead) return;
	    Animation::update(dt);
		dt/=1000.0;
		dx=dx+ax*dt;
		dy=dy+ay*dt;
		x=x+dx*dt;
		y=y+dy*dt;
		SDL_Rect src=getSize();
		if (y<0 || y>gh-src.h) dy=-dy;
		if (x<0 || x>gw-src.w) dx=-dx;
	}
	float getPositionX() { return x; }
	float getPositionY() { return y; }
	float getVelocityX() { return dx; }
	float getVelocityY() { return dy; }
	float getAccelerationX() { return ax; }
	float getAccelerationY() { return ay; }
	void setAcceleration(float newAx,float newAy) {
		ax=newAx;
		ay=newAy;
	}
	void kill() {
		dead = true;
	}
	void setVelocity(float newDx,float newDy) {
		dx=newDx;
		dy=newDy;
	}
	void setPosition(float newx, float newy) {
		x=newx;
		y=newy;
	}
	void render(Game *g,int setX=-1,int setY=-1) {
	  if (dead) return;
	  if (setX!=-1 && setY!=-1) Animation::render(g,setX,setY);
	  else Animation::render(g,(int)x,(int)y);
	}
};

class MyGame:public Game {
	Image *background, *grid, *gev, *geh, *b1, *b2, *b3, *b4, *b5, *b6, *sphere, *wall, *wall2, *goal, *endScreen;
	int count = 0;
	int passX, passY;
	int wallX, wallY, wall2X, wall2Y;
	int goalX, goalY;
	vector<Sprite *> sprites;
	public:
	MyGame():Game("Karls Game",1900,1000){}
	void init() {
		passX=405;
		passY=300;
		wallX=400;
		wallY=369;
		wall2X=992;
		wall2Y=443;
		goalX=923;
		goalY=372;
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
		wall = new Image (this, "tempwall.bmp", 70, 70);
		wall2 = new Image (this, "tempwall.bmp", 70, 70);
		goal = new Image (this, "hole.bmp", 0, 0);
		endScreen = new Image (this, "temp.bmp", 0, 0);
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
				for (int i = 400; i < 1500; i +=74){
					for (int j = 295; j < 800; j+=74) {
						grid->render(this,i,j);
						if(j+74<800) geh->render(this,i,j+71);
						if(i+74<1500) gev->render(this,i+71,j); 
						}
				}
				wall->render(this,wallX,wallY);
				wall2->render(this,wall2X,wall2Y);
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
		if (objX == goalX && objY == goalY)
		{
			SDL_Renderer *renderer=getRenderer();
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
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passX;passX<1442;passX++) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wallX,wallY,wall->getW(),wall->getH(), direction) || 
				    collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wall2X,wall2Y,wall2->getW(),wall2->getH(), direction)) break;
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
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passX;passX>405;passX--) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wallX,wallY,wall->getW(),wall->getH(),direction) || 
				    collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wall2X,wall2Y,wall2->getW(),wall2->getH(),direction)) break;
				render();
			}	
		}
	}
	void spheregoup()
	{
		string direction = "up";
		count = 0;
		if (passY != 300) {
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passY;passY>300;passY--) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wallX,wallY,wall->getW(),wall->getH(),direction) || 
				    collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wall2X,wall2Y,wall2->getW(),wall2->getH(),direction)) break;
				render();
			}	
		}
	}
	void spheregodown()
	{
		string direction = "down";
		count = 0;
		if (passY != 739) {
			SDL_Renderer *renderer=getRenderer();
			SDL_RenderPresent(renderer);
			for (passY;passY<744;passY++) 
			{
				if (goalHit(passX,passY,goalX,goalY)) break;
				if (collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wallX,wallY,wall->getW(),wall->getH(),direction) || 
				    collision(passX-5,passY-5,sphere->getW(),sphere->getH(),wall2X,wall2Y,wall2->getW(),wall2->getH(),direction)) break;
				render();
			}	
		}
	}
};

int main(int argc, char* argv[]) {
    Game *g;
    g=new MyGame();
    g->run();
    delete g;
    return 0;
 }   
