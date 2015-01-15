//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>


//The dot that will move around on the screen
class ZRect
{
    public:
		//The dimensions of the dot
		static const int rectW = 200;
		static const int rectH = 200;
		int rectX = 0;
		int rectY = 0;
		int dotXprev = 0;
		int dotYprev = 0;
		int dotW = 20;
		int dotH = 20;
		std::string color = "red";
		std::string lastPose = "";
		bool grabbed = false;

		//Initializes the variables
		ZRect(std::string initColor, int initX, int initY, int initDotW, int initDotH){
			color = initColor;
			rectX = initX;
			rectY = initY;
			dotW = initDotW;
			dotH = initDotH;
		};

		//Moves the dot
		void updateCoords(float xPerc, float yPerc, float initX){

		};

		void checkGrab(std::string curPose, int dotX, int dotY, myo::Myo* myo){
			int dotR = dotX + dotW;
			int dotB = dotY + dotH;
			int rectR = rectX + rectW;
			int rectB = rectY + rectH;
			// they collide
			if(rectR >= dotX && rectX <= dotR && rectB >= dotY && rectY <= dotB) {
				printf("\rCOLLIDE");
				if (lastPose != curPose) {
					if (lastPose == "fist") {
						grabbed = false;
						printf("\rlet go");
					}
					if (curPose == "fist") {
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

		//Shows the dot on the screen
		void render(){
			SDL_Rect fillRect = { rectX, rectY, rectW, rectH };
      if (grabbed) {
      	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
      }
      else {
      	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
      }
      SDL_RenderFillRect( gRenderer, &fillRect );
		};

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;
};

