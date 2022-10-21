#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//Servo settings
#define MIN_PULSE_WIDTH       800
#define MAX_PULSE_WIDTH       2200
#define DEFAULT_PULSE_WIDTH   1500
#define FREQUENCY             50

//Leg lengths
const float a = 110; //leg length in mm
const float b = 135;
const float c = 140;

//Default servo center positions
int servoBASErl[3] = {95, 140, 0};
int servoBASErr[3] = {90, 20, 160};
int servoBASEfl[3] = {90, 130, 0};
int servoBASEfr[3] = {100, 30, 160};
int servoBASEarm[4] = {90, 90, 90, 90};
int servoBASE[2] = {100, 10}; //Sitting position values
int servoSTAND[2] = {230, 6}; //Standing position values

//walking
float armY[4] = {140, 140, 200, 190};
float armX[4] = {0, 15, 15, 0};
int l = 0;

//Present position
int presentposition[2] = {servoBASE[0], servoBASE[1]};
bool check = false;


int pulseWidth(int angle){
  int pulse_wide, analog_value;
  pulse_wide   = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  return analog_value;
}

//triangle calculations
void servopos(int y, int x, int arm) {

  //calculating default triangle
  float Aa = acos((pow(b, 2) + pow(c, 2) - pow (a, 2)) / (2 * b * c)) * (180 / 3.142); //cos formula -> arccos to get in radians -> radians to degrees
  float Ba = acos((pow(a, 2) + pow(c, 2) - pow (b, 2)) / (2 * a * c)) * (180 / 3.142);
  float Ca = acos((pow(a, 2) + pow(b, 2) - pow (c, 2)) / (2 * a * b)) * (180 / 3.142);

  //calculating desired triangle
  int desiredposy = y;
  int desiredposx = x;
  float Ab = acos((pow(b, 2) + pow(desiredposy, 2) - pow (a, 2)) / (2 * b * desiredposy)) * (180 / 3.142); //cos formula -> arccos to get in radians -> radians to degrees
  float Bb = acos((pow(a, 2) + pow(desiredposy, 2) - pow (b, 2)) / (2 * a * desiredposy)) * (180 / 3.142);
  float Cb = acos((pow(a, 2) + pow(b, 2) - pow (desiredposy, 2)) / (2 * a * b)) * (180 / 3.142);

  float theta = atan(desiredposx / c) * (180 / 3.142) * 5; //control of the x axis //*5 coefficient

  //difference between the two triangles
  float difB = (Bb - theta) - Ba;
  if (arm == 1)
  {
    pwm.setPWM(1, 0, pulseWidth(servoBASErl[1] + difB));
  }

  else if (arm == 3)
  {
    pwm.setPWM(4, 0, pulseWidth(servoBASErr[1] - difB));
  }

  else if (arm == 2)
  {
    pwm.setPWM(7, 0, pulseWidth(servoBASEfl[1] + difB));
  }

  else if (arm == 4)
  {
    pwm.setPWM(10, 0, pulseWidth(servoBASEfr[1] - difB));
  }

  float difC = Cb - Ca;
  if (arm == 1)
  {
    pwm.setPWM(2, 0, pulseWidth(servoBASErl[2] + difC));
  }

  else if (arm == 3)
  {
    pwm.setPWM(5, 0, pulseWidth(servoBASErr[2] - difC));
  }

  else if (arm == 2)
  {
    pwm.setPWM(8, 0, pulseWidth(servoBASEfl[2] + difC));
  }

  else if (arm == 4)
  {
    pwm.setPWM(11, 0, pulseWidth(servoBASEfr[2] - difC));
  }

}

/*
void defaultpos() //starting position
{
 for(int i = 0; i < 16; i++)
 { pwm.setPWM(i, i, pulseWidth(servoBASE[i]));}
}

void standpos() //standing position
{
 for(int i = 0; i < 16; i++)
 { pwm.setPWM(i, i, pulseWidth(servoSTAND[i]));}
}
*/

//transition between different walk phases
void walktransition()
{
  float sy[4] = {armY[3], armY[2], armY[1], armY[0]};
  float ey[4] = {armY[0], armY[3], armY[2], armY[1]};
  float sx[4] = {armX[3], armX[2], armX[1], armX[0]};
  float ex[4] = {armX[0], armX[3], armX[2], armX[1]};
  float dify[4] = {abs(ey[0] - sy[0]), abs(ey[1] - sy[1]), abs(ey[2] - sy[2]), abs(ey[3] - sy[3])};
  float difx[4] = {abs(ex[0] - sx[0]), abs(ex[1] - sx[1]), abs(ex[2] - sx[2]), abs(ex[3] - sx[3])};
  float countery[4];
  float counterx[4];

  for (int i = 0; i < 4; i++)
  {
    if (dify[i] == 0) {
      countery[i] = 0;
    }
    else if (dify[i] >= difx[i]) {
      countery[i] = 1;
      counterx[i] = difx[i] / dify[i];
    }
    if (difx[i] == 0) {
      counterx[i] = 0;
    }
    else if (dify[i] <= difx[i]) {
      countery[i] = dify[i] / difx[i];
      counterx[i] = 1;
    }
  }

  for (int i = 0; i < 4; i++)
  {
    while (String(sy[i], 0) != String(ey[i], 0) || String(sx[i], 0) != String(ex[i], 0))
    {
      if (sy[i] > ey[i])
      {
        sy[i] -= countery[i];
        servopos(sy[i], sx[i], i + 1);
      }
      else if (sy[i] < ey[i])
      {
        sy[i] += countery[i];
        servopos(sy[i], sx[i], i + 1);
      }

      if (sx[i] > ex[i])
      {
        sx[i] -= counterx[i];
        servopos(sy[i], sx[i], i + 1);
      }
      else if (sx[i] < ex[i])
      {
        sx[i] += counterx[i];
        servopos(sy[i], sx[i], i + 1);
      }
    }
  }

}

//transition between different phases
void transition(int present[2], int future[2])
{
  float sy = present[0];
  float ey = future[0];
  float sx = present[1];
  float ex = future[1];
  float dify = abs(ey - sy);
  float difx = abs(ex - sx);
  float countery;
  float counterx;

  if (dify == 0) {
    countery = 0;
  }
  else if (dify >= difx) {
    countery = 1;
    counterx = difx / dify;
  }
  if (difx == 0) {
    counterx = 0;
  }
  else if (dify <= difx) {
    countery = dify / difx;
    counterx = 1;
  }


  while (String(sy, 0) != String(ey, 0) || String(sx, 0) != String(ex, 0))
  {
    if (sy > ey)
    {
      sy -= countery;
      servopos(sy, sx, 1);
      servopos(sy, sx, 2);
      servopos(sy, sx, 3);
      servopos(sy, sx, 4);
    }
    else if (sy < ey)
    {
      sy += countery;
      servopos(sy, sx, 1);
      servopos(sy, sx, 2);
      servopos(sy, sx, 3);
      servopos(sy, sx, 4);
    }

    if (sx > ex)
    {
      sx -= counterx;
      servopos(sy, sx, 1);
      servopos(sy, sx, 2);
      servopos(sy, sx, 3);
      servopos(sy, sx, 4);
    }
    else if (sx < ex)
    {
      sx += counterx;
      servopos(sy, sx, 1);
      servopos(sy, sx, 2);
      servopos(sy, sx, 3);
      servopos(sy, sx, 4);
    }
  }
}


void kinematicsinit(){

  //Initialize PWM
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  
  //Shoulder initialization
  pwm.setPWM(0, 0, pulseWidth(servoBASErl[0]));
  pwm.setPWM(3, 0, pulseWidth(servoBASErr[0]));
  pwm.setPWM(6, 0, pulseWidth(servoBASEfl[0]));
  pwm.setPWM(9, 0, pulseWidth(servoBASEfr[0]));

  for (int i = 1; i <= 4; i++) {
    servopos(servoBASE[0], servoBASE[1], i);
  }
}
