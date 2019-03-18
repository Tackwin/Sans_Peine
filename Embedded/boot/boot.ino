#include "MAG3110.h"
#include <stdio.h>
#include <stdint.h>

bool reading = false;
bool calibrate = false;
bool calibrating = false;
int timestamp = 0;
MAG3110 mag = MAG3110(); //Instantiate MAG3110

union float_as_byte {
  float f;
  char bytes[4];
};

union i32_as_byte{
  int32_t v;
  uint8_t bytes[4];
};

struct Vector3f {
  float_as_byte x;
  float_as_byte y;
  float_as_byte z;
};

struct Vector3i {
  i32_as_byte x;
  i32_as_byte y;
  i32_as_byte z;
};

void setup() {
  Serial.begin(115200);
  mag.initialize(); //Initializes the mag sensor
  mag.start();
}

void send_vector();
void send_sync_sequence();
void send_float(float_as_byte x);
void send_i32(i32_as_byte x);

void loop() {
  static int i = 0;
  static int n = 10;
  if (calibrate){
    calibrate = false;
    calibrating = true;
  }
  if (calibrating){
    if(!mag.isCalibrated()) //If we're not calibrated
    {
      if(!mag.isCalibrating()) //And we're not currently calibrating
      {
        mag.enterCalMode(); //This sets the output data rate to the highest possible and puts the mag sensor in active mode
      }
      else
      {
        //Must call every loop while calibrating to collect calibration data
        //This will automatically exit calibration
        //You can terminate calibration early by calling mag.exitCalMode();
        mag.calibrate(); 
      }
    } else {
      calibrating = false;
    }    
  }
  else {
    if (mag.dataReady()) {
      if (i <= 0){
        send_sync_sequence();
        i = n;
      }
      i--;
      send_vector();
    }
  }
}

void serialEvent(){
  while(Serial.available()){
    char in = (char)Serial.read();
    if (in == 125) reading = true;
    if (in == 126) calibrate = true;
    else reading = false;
  }
}

void send_vector(){
  static int N = 0;
    Vector3i vec;
    
    int a, b, c;
    mag.readMag(&a, &b, &c);

    if (a > +10000) a = +10000;
    if (a < -10000) a = -10000;
    if (b > +10000) b = +10000;
    if (b < -10000) b = -10000;
    if (c > +10000) c = +10000;
    if (c < -10000) c = -10000;
    
    vec.x.v = a;
    vec.y.v = b;
    vec.z.v = c;
    
    send_i32(vec.x);
    //Serial.print(", ");
    send_i32(vec.y);
    //Serial.print(", ");
    send_i32(vec.z);
    //Serial.print("\n");
}
void send_sync_sequence(){
  int n = 10;
  char data[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

  for (int i = 0; i < n; ++i){
    Serial.write(data[i]);
  }
}

void send_float(float_as_byte x){
  //Serial.print(x.f);
  Serial.write(x.bytes[0]);
  Serial.write(x.bytes[1]);
  Serial.write(x.bytes[2]);
  Serial.write(x.bytes[3]);
}

void send_i32(i32_as_byte x){
  //Serial.print(x.f);
  Serial.write(x.bytes[0]);
  Serial.write(x.bytes[1]);
  Serial.write(x.bytes[2]);
  Serial.write(x.bytes[3]);
}
