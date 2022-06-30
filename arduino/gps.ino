#include <HMC5883L.h>

#include <SoftwareSerial.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <Servo.h>
#define addr 0x1E  
// pin 
int TXPin = 4;
int RXPin = 3;
int motorPin = 9;
// baud rate
int gpsBaud = 9600;
// global variables
float _lat = 25.01;   // latitude of gps measurement
float _lng = 121.54;   // longtitude of gps measurement
float tar_lat = 46.95;
float tar_lng = 2.95;
float degree = 0;
// module
SoftwareSerial gpsSerial(TXPin, RXPin); // bluetooth
TinyGPSPlus gps;
HMC5883L compass; 
Servo servo;
void parseBlueToothData(float&,float&);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  gpsSerial.begin(gpsBaud);
  Serial.println("Starting the I2C interface.");
  Wire.begin();
  Wire.beginTransmission(addr); //start talking
  Wire.write(0x02);             // Set the Register
  Wire.write(0x00);             // Tell the HMC5883 to Continuously Measure
  Wire.endTransmission();
    // Start the I2C interface.
      

  servo.attach(9); 
  servo.write(0);
  delay(1000);
  servo.write(150);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  // get gps info
  if (gpsSerial.available() > 0){
        byte k;
       _lat = gpsSerial.parseFloat();
       _lng = gpsSerial.parseFloat();
        k = gpsSerial.read();
       Serial.println(_lat);
       Serial.println(_lng);
       degree = atan2(_lat-tar_lat,_lng-tar_lng);
  }
  Serial.print("degree");
  Serial.print(degree);
  Serial.print("\n");
//  else{
//    Serial.println("gps not available");
//   }
  // get magnetmeter info 
  Wire.beginTransmission(addr);
  Wire.write(0x03);       //start with register 3.
  Wire.endTransmission();
  int x,y,z; 
  //Read the data.. 2 bytes for each axis.. 6 total bytes
  Wire.requestFrom(addr, 6);
  if(6 <= Wire.available()){
  
    x = Wire.read() <<8;    //MSB  x 
    x |= Wire.read();      //LSB  x
    z = Wire.read() <<8;   //MSB  z
    z |= Wire.read();      //LSB z
    y = Wire.read() <<8;   //MSB y
    y |= Wire.read();      //LSB y
  }
  Serial.print("direction: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.print(z);
  Serial.println();  
//  if (z >0){
//    x = -x;
//    y = -y;
//  }
  int heading = atan2((double)y,(double)x)*180.0/PI;
  
  
  //  float declinationAngle = 0.0215;  //地磁偏角
  //  heading += declinationAngle;
  //heading = heading*180.0/PI; // degree
  Serial.print("heading: ");
  Serial.println(heading);
  // angle calculate
  int angle = int(degree-heading); 
  angle = angle % 360;
  if (angle < 0)
  
    angle += 360;
  if (angle > 360)
    angle -= 360;
  angle = int(angle*5/12);
  Serial.print("angle: ");
  Serial.println(angle);
  servo.write(angle);
  // write to motor
  Serial.write("\n\n");
  delay(1000);
}
void displayinfo(){
  if(gps.location.isValid()){
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  }
  else{
    Serial.println("Location: Not Available");
  }
  delay(1000);
}
//float angle(float x1,float y1,float x2,float y2){
//  // normalize
//  x1 = x1/(max(abs(x1),abs(y1)));
//  y1 = y1/(max(abs(x1),abs(y1)));
//  return 0;
//}
bool checkGY271Connect(){
  byte error;
  Serial.println("Scanning");
  Wire.beginTransmission(addr);
  error = Wire.endTransmission();
  if(error == 0){
    Serial.println("compass device found!!");
    return true;
  }
  else if (error == 4){
    Serial.println("unknown error at addr!!");
    return false;
  }
  else{
    Serial.println("not found at addr!!");
    return false;
  }
}
