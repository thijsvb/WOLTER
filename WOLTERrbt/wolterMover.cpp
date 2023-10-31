#include "Arduino.h"
#include "wolterMover.h"
#include <FastLED.h>
#include <Servo.h>

wolterMover::wolterMover() {
  copy(pos, pos0);
}

void wolterMover::init() {
  servoA.attach(servoApin);
  servoB.attach(servoBpin);
}

int wolterMover::getMode() {
  return mode;
}

void wolterMover::setMode(int m) {
  mode = m;
}

void wolterMover::debug() {
  int alpha = robPar[0] - alpha0;
  int beta = robPar[1];

  alpha = constrain(alpha, 0, 180);
  beta = constrain(beta, 0, 180);

  // Serial print for debugging/visualisation
  Serial.print(robPar[0]); Serial.print(',');Serial.print(robPar[1]); Serial.print(',');
  Serial.print(pos[0]); Serial.print(','); Serial.println(pos[1]);
  Serial.print("#"); // anything after '#' will be printed in Processing
}

/*
 * ROBOT
 */
void wolterMover::calcAngles(float* out) {
  float r = sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
  float a = atan(pos[1]/pos[0]);
  if(pos[0] < 0) a += PI;
  float b = atan(r/z0);

  a = degrees(a);
  b = degrees(b);
  int alpha = (int(a)+360)%360;
  int beta = int(b);

  out[0] = alpha;
  out[1] = beta;
}

void wolterMover::setServos() {
  
  calcAngles(robPar);
  
  int alpha = robPar[0] - alpha0;
  int beta = robPar[1];

  // servo calibration, based on measurements
  alpha = alpha * 180/160;
  beta = beta * 79/90 + 8;
  
  alpha = constrain(alpha, 0, 180);
  beta = constrain(beta, 0, 180);

  servoA.write(alpha);
  servoB.write(beta);
}

/*
 * MOVEMENT FUNCTIONS
 */

void wolterMover::move() {
  // wait to slow down to framerate
  while(millis() - frameTimer < frameDelay);
//  float measuredFps = 1000/(millis() - frameTimer);
//  Serial.print("fps: ");Serial.print(measuredFps);Serial.print("/");Serial.print(fps);Serial.print("#");
  frameTimer = millis();

  /*
   * VELOCITY
   */
  float pvel[2];
  copy(pvel, vel);
  
  switch(mode) {
    case 0: // bouncing mode
      // send dot off if not moving yet
      if(vel[0]==0 && vel[1]==0) {
        vel[0] = vmax/2; 
        vel[1] = 0;
        float a = TWO_PI*random(1000)/1000.0;
        rotate(vel, a);
      }
      break;

    case 2: // joystick mode
      int joyX = analogRead(joyXpin);
      int joyY = analogRead(joyYpin);
      
      // adjust for joystick deadzones, max and min values; spits out -100 to 100 values
      if(abs(joyX-joyXmid)>joyXdead) {
        if(joyX>joyXmid) joyX = map(joyX, joyXmid+joyXdead, joyXmax, 0, 100);
        else joyX = map(joyX, joyXmid-joyXdead, joyXmin, 0, -100);
      } else { joyX = 0; }
      if(abs(joyY-joyYmid)>joyYdead) {
        if(joyY>joyYmid) joyY = map(joyY, joyYmid+joyYdead, joyYmax, 0, 100);
        else joyY = map(joyY, joyYmid-joyYdead, joyYmin, 0, -100);
      } else { joyY = 0; }
      
      // set velocity according to joystick values
      vel[0] = vmax * float(joyX)/100.0;
      vel[1] = vmax * float(joyY)/100.0;
      break;
  }

  /*
   * OBSTACLE / WALLS BOUNCING
   */
  if(mode==2) {
    // for joystick mode just avoid region out of servo reach
    // obstacle B
    if(pos[1] > mb*pos[0]) {
      vel[0] = min(0, vel[0]);
      vel[1] = min(0, vel[1]);
    }
  } else {
    boolean bounced = false;
    
    // walls
    if(pos[0] < xmin || pos[0] > xmax) {
      vel[0] = -vel[0];
      bounced = true;
    }
    if(pos[1] < ymin || pos[1] > ymax) {
      vel[1] = -vel[1];
      bounced = true;
    }
  
    // obstacle A
    if(pos[0] < xa && pos[1] < ya) {
      if((pos[0]-xa) > (pos[1]-ya)) {
        vel[0] = -vel[0];
      } else {
        vel[1] = -vel[1];
      }
      bounced = true;
    }
  
    // obstacle B
    if(pos[1] > mb*pos[0]) {
      // v = v-2(v*n)n
      float f = 2*dot(vel, nb);
      float n2[2];
      copy(n2, nb);
      mult(n2, f);
      sub(vel, n2);
      bounced = true;
    }
  
    // step back to avoid edge cases
    if(bounced) {
      mult(pvel, -1);
      add(pos, pvel);
    }

    if(mode==0) setMag(vel, vmax/2); // vel might drop due to floating point math
  }
  // movement step
  add(pos, vel);
}

/*
 * 2D VECTOR FUNCTIONS
 */
 
void wolterMover::copy(float* out, float* in) {
  out[0] = in[0];
  out[1] = in[1];
}

void wolterMover::rotate(float* out, float a) {
  float temp[2];
  copy(temp, out);

  out[0] = temp[0]*cos(a) - temp[1]*sin(a);
  out[1] = temp[0]*sin(a) + temp[1]*cos(a);
}

float wolterMover::dot(float* a, float* b) {
  return a[0]*b[0] + a[1]*b[1];
}

void wolterMover::mult(float* out, float f) {
  out[0] *= f;
  out[1] *= f;
}

void wolterMover::add(float* out, float* in) {
  out[0] += in[0];
  out[1] += in[1];
}

void wolterMover::sub(float* out, float* in) {
  out[0] -= in[0];
  out[1] -= in[1];
}

void wolterMover::setMag(float* in, float setM) {
  double currMag = sqrt(in[0]*in[0] + in[1]*in[1]);
//  Serial.print("vx=");Serial.print(vel[0],4);Serial.print("#");
//  Serial.print("vy=");Serial.print(vel[1],4);Serial.print("#");
//  Serial.print("|v|=");Serial.print(currMag,4);Serial.print("#");
  in[0] *= setM/currMag;
  in[1] *= setM/currMag;
}
