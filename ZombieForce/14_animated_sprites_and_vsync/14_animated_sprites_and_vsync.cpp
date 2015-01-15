/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
// const int SCREEN_WIDTH = 640;
// const int SCREEN_HEIGHT = 480;



//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Walking animation
int frame = 0;
const int WALKING_ANIMATION_FRAMES = 6;
SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ];
LTexture gSpriteSheetTexture;



bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
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
		int w = 37;
		int h = 56;
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

void render(){
	SDL_Rect* currentClip = &gSpriteClips[ frame / 4 ];
	gSpriteSheetTexture.render( ( SCREEN_WIDTH - currentClip->w ) / 2, ( SCREEN_HEIGHT - currentClip->h ) / 2, currentClip );

	//Update screen
	SDL_RenderPresent( gRenderer );

	//Go to next frame
	++frame;

	//Cycle animation
	if( frame / 4 >= WALKING_ANIMATION_FRAMES )
	{
		frame = 0;
	}
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

