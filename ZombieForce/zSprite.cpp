/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <random>

//Screen dimension constants
// const int SCREEN_WIDTH = 640;
// const int SCREEN_HEIGHT = 480;

const int WALKING_ANIMATION_FRAMES = 6;
const int EXPL_ANIMATION_FRAMES = 4;
const int TOTAL_ANIMATION_FRAMES = WALKING_ANIMATION_FRAMES + EXPL_ANIMATION_FRAMES;


class ZSprite{

	public:
	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;
	int origW = 36;
	int origH = 56;
	int rectW = 36;
	int rectH = 56;
	float curFactor = 1.0;
	int rectX = 0;
	int rectY = 0;
	int dotXprev = 0;
	int dotYprev = 0;
	int dotW = 20;
	int dotH = 20;
	std::string lastPose = "";
	bool grabbed = false;
	bool destroyed = false;
	int explodeFrames = -1;
	int explodeFrameMax = 3;
	double endSize = 0.4;
	double preEndSize = endSize - 0.05;
	double scaleDecr = 1.3;

	//The window renderer
	SDL_Renderer* gRenderer = NULL;

	//Walking animation
	int frame = 0;
	int frameSlowdown = 8;
	SDL_Rect gSpriteClips[ TOTAL_ANIMATION_FRAMES ];
	LTexture gSpriteSheetTexture;

	//Initializes the variables
	void initZSprite(int initDotW, int initDotH){
		rectX = rand() % (SCREEN_WIDTH - rectW);
		rectY = rand() % (SCREEN_HEIGHT - rectH);
		dotW = initDotW;
		dotH = initDotH;
	};

	void scale(float factorInc){
		curFactor += factorInc;
		printf("%f ", curFactor);
		if (rectH > SCREEN_HEIGHT) {
			// TODO lose health?
			return;
		}
		rectW = (int) origW*curFactor;
		rectH = (int) origH*curFactor;
		gSpriteSheetTexture.mWidth = rectW;
		gSpriteSheetTexture.mHeight = rectH;
	}

	// updates zombie sprite to next frame when walking
	int render(){
		if (rectH > SCREEN_HEIGHT*endSize) {
			return 2;
		}

		scale(0.02);

		int curFrame = 0;
		if (explodeFrames > -1) {
			curFrame = WALKING_ANIMATION_FRAMES + explodeFrameMax - explodeFrames;
			explodeFrames--;
			if (explodeFrames == -1) {
				frame = 0;
				rectX = rand() % (SCREEN_WIDTH - rectW);
				rectY = rand() % (SCREEN_HEIGHT - rectH);
				scale(-scaleDecr);
			}
		}
		else {
			curFrame = frame / frameSlowdown;
		}

		SDL_Rect* currentClip = &gSpriteClips[ curFrame ];
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

		if (rectH > SCREEN_HEIGHT*preEndSize) return 1;
		else return 0;
	}

	bool checkGrab(std::string curPose, int dotX, int dotY, myo::Myo* myo){
		int dotR = dotX + dotW;
		int dotB = dotY + dotH;
		int rectR = rectX + rectW;
		int rectB = rectY + rectH;
		bool gotZombie = false;
		// they collide
		if(rectR >= dotX && rectX <= dotR && rectB >= dotY && rectY <= dotB) {
			if (lastPose != curPose) {
				// if (lastPose == "fingersSpread") {
				// 	grabbed = false;
				// }
				// if (curPose == "fingersSpread") {
				// 	grabbed = true;
				// }
				if (curPose == "fist" || curPose == "waveIn" || 
						curPose == "waveIn" || curPose == "doubleTap") {
					Mix_PlayChannel( -1, gSplode, 0 );
					explodeFrames = explodeFrameMax;
					gotZombie = true;
					myo->vibrate(myo::Myo::vibrationShort);
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
		return gotZombie;
	};

	bool checkThrow(bool bigMoveHappened){
		if (grabbed){
			if (bigMoveHappened){
				scale(-scaleDecr);
			}
			return false;
		}
		else {
			return bigMoveHappened;			
		}
	}

	bool loadMedia(){
		//Loading success flag
		bool success = true;
		//Load sprite sheet texture
		if( !gSpriteSheetTexture.loadFromFile( "zombieSprites.png", 0xFF, 0x00, 0xFF ) )
		{
			printf( "Failed to load walking animation texture!\n" );
			success = false;
		}
		else
		{
			// key out background green color

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

			// explosion

			int exX = 28;
			int exY = 284;
			int exW = 43;
			int exH = 54;

			gSpriteClips[ 6 ].x = 170;
			gSpriteClips[ 6 ].y = 210;
			gSpriteClips[ 6 ].w = exW;
			gSpriteClips[ 6 ].h = exH;

			gSpriteClips[ 7 ].x = exX;
			gSpriteClips[ 7 ].y = exY;
			gSpriteClips[ 7 ].w = exW;
			gSpriteClips[ 7 ].h = exH;

			gSpriteClips[ 8 ].x = exX + exW;
			gSpriteClips[ 8 ].y = exY;
			gSpriteClips[ 8 ].w = exW;
			gSpriteClips[ 8 ].h = exH;

			gSpriteClips[ 9 ].x = exX + 2*exW;
			gSpriteClips[ 9 ].y = exY;
			gSpriteClips[ 9 ].w = exW;
			gSpriteClips[ 9 ].h = exH;
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
