// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <time.h>
#include <string.h>
#include <random>

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

// include sdl file
#include "26_motion/26_motion.cpp"
#include "26_motion/zRect.cpp"
#include "zSprite.cpp"

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DataCollector : public myo::DeviceListener {
public:
    bool bigMoveHappened = false;
    
    DataCollector()
    : onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
    {
    }

    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
        onArm = false;
        isUnlocked = false;
    }

    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;

        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

        // Convert the floating point angles in radians to a scale from 0 to 18.
        roll_perc = ((roll + (float)M_PI)/(M_PI * 2.0f));
        pitch_perc = ((pitch + (float)M_PI/2.0f)/M_PI);
        yaw_perc = ((yaw + (float)M_PI)/(M_PI * 2.0f));
        
        // Convert the floating point angles in radians to a scale from 0 to 18.
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
        
        if (yaw_init < 0) {
            yaw_init = yaw_perc;
        }
    }

    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;

        if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
            // Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
            // Myo becoming locked.
            myo->unlock(myo::Myo::unlockHold);

            // Notify the Myo that the pose has resulted in an action, in this case changing
            // the text on the screen. The Myo will vibrate.
            myo->notifyUserAction();
        } else {
            // Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
            // are being performed, but lock after inactivity.
            myo->unlock(myo::Myo::unlockTimed);
        }
        poseString = currentPose.toString();

    }

    // onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
    {
        onArm = true;
        whichArm = arm;
    }

    // onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }

    // onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
    void onUnlock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = true;
    }

    // onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
    void onLock(myo::Myo* myo, uint64_t timestamp)
    {
//        myo::UnlockType r = myo::unlockTimed;
//        myo->unlock(r);
        myo->unlock(myo::Myo::unlockTimed);
    }

    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.
    void onAccelerometerData (myo::Myo *myo, uint64_t timestamp, const myo::Vector3 < float > &accel){
        //printf("\r%f %f %f %f", accel[0], accel[1], accel[2], accel[3]);
        if (fabs(accel[0]) > 1.2) {
            bigMoveHappened = true;
        }
        if (fabs(accel[1]) > 2.0) {
        }
        
//        if (fabs(accel[2]) > 1.0) {
//            printf("\rBIG 2");
//        }
    }
    
    
    // We define this function to print the current values that were updated by the on...() functions above.
    void print()
    {
        // Clear the current line
        std::cout << '\r';
        
        std::cout << yaw_perc;
        std::cout << ' ' << yaw_init << ' ';
        //std::cout << ' ' << pitch_perc << ' ' << yaw_perc << ' ';
        
        // Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
        std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
                  << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
                  << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';

        if (onArm) {
            // Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.

            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().

            std::cout << '[' << (isUnlocked ? "unlock" : "lock  ") << ']'
                      << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
                      << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
        }

        std::cout << std::flush;
    }

    // These values are set by onArmSync() and onArmUnsync() above.
    bool onArm;
    myo::Arm whichArm;

    // This is set by onUnlocked() and onLocked() above.
    bool isUnlocked;

    // These values are set by onOrientationData() and onPose() above.
    int roll_w, pitch_w, yaw_w;
    float roll_perc, pitch_perc, yaw_perc;
    float yaw_init = -1.0;
    myo::Pose currentPose;
    std::string poseString;
};

int main(int argc, char** argv)
{
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try {

    // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
    // publishing your application. The Hub provides access to one or more Myos.
    myo::Hub hub("com.example.hello-myo");

    std::cout << "Attempting to find a Myo..." << std::endl;

    // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
    // immediately.
    // waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
    // if that fails, the function will return a null pointer.
    myo::Myo* myo = hub.waitForMyo(10000);
    hub.setLockingPolicy(myo::Hub::lockingPolicyNone);

    // If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }

    // We've found a Myo.
    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

    // Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
    DataCollector collector;

    // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
    // Hub::run() to send events to all registered device listeners.
    hub.addListener(&collector);

        
        
    // SDL stuff here
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Main loop flag
            bool quit = false;
            time_t start,end;
            double dif;
            int curTime;
            char curTimeStr[30];
            char timeStr[30];
            bool gameOver = false;
            time(&start);
            int gameStateNum = 0; // 0 - normal, 1 - close to dying, 2 - game over
            int tempGameStateNum;
            int zomTimes[10] = {0,5,8,14,23,31,43,51,61,79};
            
            //Event handler
            SDL_Event e;
            
            //The dot that will be moving around on the screen
            Dot dot;
            //ZRect zRect("red", 200, 200, dot.DOT_WIDTH, dot.DOT_HEIGHT);
            //ZSprite zSprite(200, 200, dot.DOT_WIDTH, dot.DOT_HEIGHT);
            //ZSprite zSprite2(300, 100, dot.DOT_WIDTH, dot.DOT_HEIGHT);
            ZSprite zombies[10];
            int numZoms = 10;
            
            for (int i = 0; i < numZoms; i++) {
                zombies[i].loadMedia();
                zombies[i].initZSprite(dot.DOT_WIDTH, dot.DOT_HEIGHT);
            }
            
            numZoms = 1;
            
            //While application is running
            while( !quit )
            {
                time(&end);
                dif = difftime(end, start);
                
                // In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
                // In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
                hub.run(1000/20);
                // After processing events, we call the print() member function we defined above to print out the values we've
                // obtained from any events that have occurred.
                //collector.print();
                
                
                //Handle events on queue for dot
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    
                    //Handle input for the dot
                    dot.handleEvent( e );
                }
                
                //Move the dot
                //dot.move();
                dot.updateCoords(1-collector.yaw_perc, 1-collector.pitch_perc, 1-collector.yaw_init);
                for (int i = 0; i < numZoms; i++) {
                    zombies[i].checkGrab(collector.poseString, dot.mPosX, dot.mPosY, myo);
                    collector.bigMoveHappened = collector.bigMoveHappened || zombies[i].checkThrow(collector.bigMoveHappened);
                }

                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );
                
                //Render objects
                if (gameStateNum != 1) gForestTexture.render(0, 0);
                else gRedForestTexture.render(0, 0);

                gameStateNum = 0;  // assume we're good
                for (int i = 0; i < numZoms; i++) {
                    if (!gameOver) {
                        tempGameStateNum = zombies[i].render();
                        if (tempGameStateNum == 2) {
                            gameOver = true;
                        }
                        else if (!gameOver and tempGameStateNum == 1) {
                            gameStateNum = 1;
                        }
                    }
                }
                dot.render();
                
                // prep time for screen
                SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
                
                if (gameOver) {
                    strcpy(timeStr, "GAME OVER - ");
                    strcat(timeStr, "Final Time: ");
                    strcat(timeStr, curTimeStr);
                    gTextTexture.loadFromRenderedText( timeStr, textColor );
                    gTextTexture.render( ( SCREEN_WIDTH/2 - gTextTexture.getWidth() / 2 ), ( SCREEN_HEIGHT/2 - gTextTexture.getHeight()/2 ) );
                }
                else {
                    curTime = (int)dif;
                    // based on curTime, increase zombie number
                    for (int i = 0; i < 10; i++) {
                        if (zomTimes[i] >= curTime) {
                            numZoms = i;
                            break;
                        }
                    }
                    strcpy(curTimeStr, std::to_string(curTime).c_str());
                    strcpy(timeStr, "Time: ");
                    strcat(timeStr, curTimeStr);
                    gTextTexture.loadFromRenderedText( timeStr, textColor );
                    gTextTexture.render( ( SCREEN_WIDTH - gTextTexture.getWidth() - 40 ), ( 20 ) );
                }
                
                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }
    
    //Free resources and close SDL
    close();
        
    return 0;


    // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}
