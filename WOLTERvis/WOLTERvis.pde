import processing.serial.*;
Serial port;

float x, y;
float alx1, aly1, alx2, aly2, alx3, aly3;
float blx1, bly1, blx2, bly2;
float xa, ya;
float mb, xb1, yb1, xb2, yb2;

void setup() {
  port = new Serial(this, Serial.list()[0], 9600);
  port.bufferUntil('\n');
  
  // 1cm = 1px
  size(630, 745);
  
  // obstacle constants, converted to pixelspace
  // A
  xa = map(-0.5, -4.5, 1.8, 0, width);
  ya = map(-1.25, -4.95, 2.5, height, 0);
  // B
  mb = -3.72;
  yb1 = 2.5;
  xb1 = yb1/mb;
  xb2 = 1.8;
  yb2 = mb*xb2;
  yb1 = map(yb1, -4.95, 2.5, height, 0);
  yb2 = map(yb2, -4.95, 2.5, height, 0);
  xb1 = map(xb1, -4.5, 1.8, 0, width);
  xb2 = map(xb2, -4.5, 1.8, 0, width);
  
  // robot constants
  alx1 = map(0, -4.5, 1.8, 0, width);
  aly1 = map(0, -4.95, 2.5, height, 0);
  blx1 = alx1 + 100;
  bly1 = aly1;
}



void draw() {
  background(0);
  
  noStroke();
  fill(0,127,0);
  rect(0, ya, xa, height-ya);
  triangle(width, 0, xb1, yb1, xb2, yb2);
  
  stroke(0,0,255);
  strokeWeight(3);
  line(alx1, aly1, alx2, aly2);
  line(blx1, bly1, blx2, bly2);
  strokeWeight(1);
  line(alx1, aly1, alx3, aly3);
  
  noStroke();
  fill(255, 0, 0);
  ellipse(x, y, 10, 10);
}

void serialEvent(Serial port) {
  String rawInString = port.readStringUntil('\n');
  // print debug strings
  String[] inStrings = split(rawInString, '#');
  for(int i=0; i!=inStrings.length-1; ++i) {
    println(inStrings[i]);
  }
  float[] input = float(split(inStrings[inStrings.length-1], ','));
  
  if(input.length == 4) {
    println(input[0], input[1]);
    x = map(input[2], -4.5, 1.8, 0, width);
    y = map(input[3], -4.95, 2.5, height, 0);
    float a = -radians(input[0]);
    float b = radians(input[1]) + HALF_PI;
    alx2 = alx1 + 50*cos(a);
    aly2 = aly1 + 50*sin(a);
    blx2 = blx1 + 50*cos(b);
    bly2 = bly1 + 50*sin(b);
    
    alx3 = alx1 + 1000*cos(a);
    aly3 = aly1 + 1000*sin(a);
  }
}
