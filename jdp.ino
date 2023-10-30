#include <SevSeg.h>
SevSeg sevseg; 
const int trigPin = 12;
const int echoPin = 13;
const int trigPin_ex = A4;
const int echoPin_ex = A5;
int count = 0;
int max_occ = 20;
int min_occ = 0;
int sensorpin = 0;
int LED_g = A2;
int LED_r = A3;
int LED_y = A1;

void setup(){
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin_ex, OUTPUT);
  pinMode(echoPin_ex, INPUT);
  pinMode(LED_g, OUTPUT);
  pinMode(LED_r, OUTPUT);
  byte numDigits = 2;
  byte digitPins[] = {10, 11};
  byte segmentPins[] = {9, 2, 3, 5, 6, 8, 7};

  bool resistorsOnSegments = true; 
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE; 
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}

void loop(){

  int reading = analogRead(sensorpin);  
 
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF); Serial.println(" degrees F");
  
  digitalWrite(LED_r, LOW);
  digitalWrite(LED_g, LOW);

  if (temperatureF > 85){
    digitalWrite(LED_r, HIGH);
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2.0;
  
  if (distance < 20) {
    if (count/10 < max_occ){
      delay(1);
      count++;
    }
    else{
      count/10 == max_occ;
      digitalWrite(LED_r, HIGH);
    }
  }


  digitalWrite(trigPin_ex, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_ex, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_ex, LOW);

  long duration2 = pulseIn(echoPin_ex, HIGH);
  float distance2 = duration2 * 0.034 / 2.0;
  
  if (distance2 < 60) {
    if (count/10 > min_occ){
      delay(1);
      count--;
    }
    else{
      count/10 == min_occ;
    }
  }

  
    if(count/10>0 && count/10<10){
      digitalWrite(LED_g, HIGH);
    }
    if(count/10>10 && count/10<20){
      digitalWrite(LED_y, HIGH);
    }
    if(count/10==20){
      digitalWrite(LED_r, HIGH);
      digitalWrite(LED_y, LOW);
    }
  sevseg.setNumber(count/10, 1);
  sevseg.refreshDisplay(); 
  delay(1);
}
