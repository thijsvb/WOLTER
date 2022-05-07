#ifndef wolterMover_h
#define wolterMover_h
#include "Arduino.h"
#include <FastLED.h>
#include <Servo.h>

class wolterMover {
  public:
    wolterMover();
    void init();
    void move();
    void setServos();
    void debug();
    
  private:
    /*
     * ROBOT
     */
    Servo servoA;
    Servo servoB;
    // pins
    const int servoApin = 9;
    const int servoBpin = 10;
    // functions
    void calcAngles(float* out);
    float robPar[2];
    
    /*
     * CONSTANTS
     */
    // obstacle A corner
    const float xa = -0.5;
    const float ya = -1.25;
    // obstacle B slope (yb = mb*xb) and normal
    const float mb = -3.72;
    const float nAngle = atan(mb) - HALF_PI;
    const float nb[2] = {cos(nAngle), sin(nAngle)};
    // walls
    const float xmax = 1.8;
    const float xmin = -4.5;
    const float ymax = 2.5;
    const float ymin = -4.95;
    // robot
    const float z0 = 2.0;
    const float alpha0 = degrees(atan(1 / -mb)) + 90;

    /*
     * MOVEMENT VARS
     */
    // position, velocity, initial position
    float pos[2], vel[2];
    const float pos0[2] = { -2.25, 1.88};
    // velocity boundries
    const float vmax = 0.06;
    const float vmin = 0.003;
    // noise vars
    uint32_t roff = 0;
    uint32_t aoff = 2e9; // offset so r and a are indep
    uint32_t dr = 100;
    uint32_t da = 30;
    // framerate
    const float fps = 35.0;
    const int frameDelay = int(1000.0/fps); // miliseconds
    uint32_t frameTimer = 0;

    /*
     * 2D VECTOR FUNCTIONS
     */
    void copy(float* out, float* in);
    void rotate(float* out, float a);
    float dot(float* a, float* b);
    void mult(float* out, float f);
    void add(float* out, float* in);
    void sub(float* out, float* in);
    void setMag(float* in, float minM, float maxM);
};

#endif
