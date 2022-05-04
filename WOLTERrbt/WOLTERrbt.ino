#include <Servo.h>
#include <FastLED.h>

/*
 * CONSTANTS
 */
// obstacle A corner 
const float xa = -0.5;
const float ya = -1.25;
// obstacle B slope (yb = mb*xb) and normal
const float mb = -3.72;
const float nAngle = atan(mb)-HALF_PI;
const float nb[2] = {cos(nAngle), sin(nAngle)};
// walls
const float xmax = 1.8;
const float xmin = -4.5;
const float ymax = 2.5;
const float ymin = -4.95;
// robot
const float z0 = 2.0;
const float alpha0 = degrees(atan(1/-mb)) + 90;



/*
 * MOVEMENT VARS
 */
// position, velocity, initial position
float pos[2], vel[2];
const float pos0[2] = {-2.25, 1.88};
// velocity boundries
const float vmax = 0.06;
const float vmin = 0.003;
// noise vars
uint32_t roff = 0;
uint32_t aoff = 2e9; // offset so r and a are indep
uint32_t dr = 100;
uint32_t da = 30;

/*
 * ROBOT
 */
float robPar[2];
Servo servoA;
Servo servoB;
// pins
const int servoApin = 9;
const int servoBpin = 10;
const int laserPin = 8;


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  copy(pos, pos0);
  vel[0] = vmax; 
  vel[1] = 0;
  float a = TWO_PI*random(1000)/1000.0;
  rotate(vel, a);

  servoA.attach(servoApin);
  servoB.attach(servoBpin);
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, HIGH);
}

void loop() {
  // Serial print for debugging/visualisation
  Serial.print(robPar[0]); Serial.print(',');Serial.print(robPar[1]); Serial.print(',');
  Serial.print(pos[0]); Serial.print(','); Serial.println(pos[1]);
//  Serial.print(robPar[0]); Serial.print(" - "); Serial.print(alpha0); Serial.print(" = "); Serial.println(robPar[0]-alpha0);
  
  move();
  calcAngles(robPar);
  setServos();
}

/*
 * ROBOT
 */
void calcAngles(float* out) {
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

void setServos() {
  int alpha = robPar[0] - alpha0;
  int beta = robPar[1];

  alpha = constrain(alpha, 0, 180);
  beta = constrain(beta, 0, 180);

  servoA.write(alpha);
  servoB.write(beta);
}

/*
 * MOVEMENT FUNCTIONS
 */

void move() {
  float pvel[2];
  copy(pvel, vel);
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

  // movement step
  add(pos, vel);
}

/*
 * 2D VECTOR FUNCTIONS
 */
 
void copy(float* out, float* in) {
  out[0] = in[0];
  out[1] = in[1];
}

void rotate(float* out, float a) {
  float temp[2];
  copy(temp, out);

  out[0] = temp[0]*cos(a) - temp[1]*sin(a);
  out[1] = temp[0]*sin(a) + temp[1]*cos(a);
}

float dot(float* a, float* b) {
  return a[0]*b[0] + a[1]*b[1];
}

void mult(float* out, float f) {
  out[0] *= f;
  out[1] *= f;
}

void add(float* out, float* in) {
  out[0] += in[0];
  out[1] += in[1];
}

void sub(float* out, float* in) {
  out[0] -= in[0];
  out[1] -= in[1];
}
