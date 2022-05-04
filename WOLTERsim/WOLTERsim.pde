PVector pos, vel, n;

float xa = 300;
float ya = 400;
// yb = 5 xb - 2000

boolean wanderMode = true;

float maxvel = 4;
float minvel = 0.2;
float roff = 0;
float aoff = 1000;
float droff = 0.1;
float daoff = 0.03;

void setup() {
  size(500, 800);
  noStroke();
  pos = new PVector(250, 200);
  vel = new PVector(2, 0);
  vel.rotate(random(TWO_PI));
  
  n = new PVector(1, 0);
  float a = atan(5) + HALF_PI; // atan(slope) +/- 90 deg
  n.rotate(a);
}

void draw() {
  background(0);
  fill(127, 0, 0);
  rect(0, ya, xa, 400);
  triangle(400, 0, 500, 0, 500, 500);
  
  if(wanderMode) wander();
  move();
  show();
}

void show() {
  fill(0, 255, 0);
  ellipse(pos.x, pos.y, 20, 20);
}

void move() {
  PVector pvel = vel.copy();
  boolean bounced = false;
  // boundries
  if(pos.x < 0 || pos.x > width) {
    vel.set(-vel.x, vel.y);
    bounced = true;
  }
  if(pos.y < 0 || pos.y > height) {
    vel.set(vel.x, -vel.y);
    bounced = true;
  }
  
  // obstakel A
  if(pos.x < xa && pos.y > ya) {
    if( -(pos.x-xa) < (pos.y-ya)) {
      vel.set(-vel.x, vel.y);
    } else {
      vel.set(vel.x, -vel.y);
    }
    bounced = true;
  }
  
  // obstakel B
  if(pos.y < 5*pos.x - 2000) {
    float f = 2 * vel.dot(n);
    vel.sub(n.copy().mult(f));
    bounced = true;
  }
  
  if(bounced) {
    // step back to avoid edge cases
    pos.add(pvel.mult(-1));
  }
  
  pos.add(vel);
}

void wander() {
  roff += droff;
  aoff += daoff;
  float dr = (noise(roff) - noise(roff-droff))*maxvel;
  float da = (noise(aoff) - noise(aoff-daoff))*TWO_PI;
  vel.setMag(vel.mag() + dr);
  vel.rotate(da);
}

void mouseClicked() {
  pos.set(mouseX, mouseY);
}

void keyPressed() {
  wanderMode = !wanderMode;
  
  float r = noise(roff)*maxvel + minvel;
  float a = noise(aoff)*TWO_PI;
  
  vel.set(r,0).rotate(a);
}
