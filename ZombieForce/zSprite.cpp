/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
// const int SCREEN_WIDTH = 640;
// const int SCREEN_HEIGHT = 480;

const int WALKING_ANIMATION_FRAMES = 6;


class ZSprite{

	public:
	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;
	static const int rectW = 36;
	static const int rectH = 56;
	int rectX = 0;
	int rectY = 0;
	int dotXprev = 0;
	int dotYprev = 0;
	int dotW = 20;
	int dotH = 20;
	std::string lastPose = "";
	bool grabbed = false;

	//The window renderer
	SDL_Renderer* gRenderer = NULL;

	//Walking animation
	int frame = 0;
	int frameSlowdown = 8;
	SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ];
	LTexture gSpriteSheetTexture;

	//Initializes the variables
	ZSprite(int initX, int initY, int initDotW, int initDotH){
		rectX = initX;
		rectY = initY;
		dotW = initDotW;
		dotH = initDotH;
	};

	// updates zombie sprite to next frame when walking
	void render(){
		SDL_Rect* currentClip = &gSpriteClips[ frame / frameSlowdown ];
		gSpriteSheetTexture.render( rectX, rectY, currentClip );

		//Update screen
		SDL_RenderPresent( gRenderer );

		//Go to next frame
		++frame;

		//Cycle animation
		if( frame / frameSlowdown >= WALKING_ANIMATION_FRAMES )
		{
			frame = 0;
		}
	}

	void checkGrab(std::string curPose, int dotX, int dotY, myo::Myo* myo){
		int dotR = dotX + dotW;
		int dotB = dotY + dotH;
		int rectR = rectX + rectW;
		int rectB = rectY + rectH;
		// they collide
		if(rectR >= dotX && rectX <= dotR && rectB >= dotY && rectY <= dotB) {
			printf("\rCOLLIDE");
			if (lastPose != curPose) {
				if (lastPose == "fingersSpread") {
					grabbed = false;
					printf("\rlet go");
				}
				if (curPose == "fingersSpread") {
					grabbed = true;
					printf("\rfist inside");
				}
				lastPose = curPose;
			}
		}
		if (grabbed) {
			int xDiff = dotX - dotXprev;
			int yDiff = dotY - dotYprev;
			rectX += xDiff;
			rectY += yDiff;
		}
		dotXprev = dotX;
		dotYprev = dotY;
	};

	bool loadMedia(){
		//Loading success flag
		bool success = true;
		//Load sprite sheet texture
		if( !gSpriteSheetTexture.loadFromFile( "zombieSprites.png" ) )
		{
			printf( "Failed to load walking animation texture!\n" );
			success = false;
		}
		else
		{
			int x1 = 8;
			int y1 = 82;
			int w = rectW;
			int h = rectH;
			//Set sprite clips
			gSpriteClips[ 0 ].x =  x1;
			gSpriteClips[ 0 ].y =  y1;
			gSpriteClips[ 0 ].w =  w;
			gSpriteClips[ 0 ].h =  h;

			gSpriteClips[ 1 ].x = x1 + w;
			gSpriteClips[ 1 ].y = y1;
			gSpriteClips[ 1 ].w = w;
			gSpriteClips[ 1 ].h = h;
			
			gSpriteClips[ 2 ].x = x1 + 2*w;
			gSpriteClips[ 2 ].y = y1;
			gSpriteClips[ 2 ].w = w;
			gSpriteClips[ 2 ].h = h;

			gSpriteClips[ 3 ].x = x1 + 3*w;
			gSpriteClips[ 3 ].y = y1;
			gSpriteClips[ 3 ].w = w;
			gSpriteClips[ 3 ].h = h;

			gSpriteClips[ 4 ].x = x1 + 4*w;
			gSpriteClips[ 4 ].y = y1;
			gSpriteClips[ 4 ].w = w;
			gSpriteClips[ 4 ].h = h;

			gSpriteClips[ 5 ].x = x1 + 5*w;
			gSpriteClips[ 5 ].y = y1;
			gSpriteClips[ 5 ].w = w;
			gSpriteClips[ 5 ].h = h;
		}

		return success;
	}

	void close()
	{
		//Free loaded images
		gSpriteSheetTexture.free();

		//Destroy window	
		SDL_DestroyRenderer( gRenderer );
		SDL_DestroyWindow( gWindow );
		gWindow = NULL;
		gRenderer = NULL;

		//Quit SDL subsystems
		IMG_Quit();
		SDL_Quit();
	}
};
