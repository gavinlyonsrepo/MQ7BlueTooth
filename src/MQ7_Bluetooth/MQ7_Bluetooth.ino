// Read an MQ-7 data and transmit it with HM-10 bluetooth module.
// https://github.com/gavinlyonsrepo/MQ7BlueTooth


#include "MQ7.h" // https://github.com/fjebaker/MQ7
#include <SoftwareSerial.h> // for bluetooth comms

#define A_PIN 0 // Analog pin to read sensor A0 output
#define VOLTAGE 5 // VCC of sensor
#define PWM_PIN 5 // PWN pin to pulse Sensor power

#define PWM_HIGH_STATE 255 // 5V 
// #define PWM_LOW_STATE 107 // jfet 1.4 + .7 = 2.1 V , 2,1/5 = 42% of 255  
#define PWM_LOW_STATE 72 // MOSFET 1.4V 

#define HEAT_ON_TIME 60000 // seconds , High voltage heating  period
#define HEAT_LOW_TIME 90000 // seconds , Low voltage heating period
#define READ_DELAY 1000 // seconds, delay between starting each sensor read cycle 
#define READ_DELAY_TWO 50 // delay between switching  sensor back to 5V and reading it after low heating cycle

#define STATUS_LED_PIN 13 // Status LED pin , for this example we use onboard NANO LED

#define MCU_RX_PIN 4  // TX on bluetooth module
#define MCU_TX_PIN 6  // RX on bluetooth module

// Init MQ7 device
MQ7 mq7(A_PIN, VOLTAGE);

// Init bluetooth comms,  RX(TX on module) , TX(RX on module)
SoftwareSerial BTserial( MCU_RX_PIN, MCU_TX_PIN); 

void setup() {
	// Serial Setup
  Serial.begin(9600);
	while (!Serial) {
		;	// wait for serial connection
	}

  // Bluetooth setup
  // Set baud rate of HC-06 that you set up using the FTDI USB-to-Serial module
  BTserial.begin(9600);

  // Status LED setup
	pinMode(STATUS_LED_PIN, OUTPUT);
  
  // PWM Sensor setup
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, PWM_HIGH_STATE); // 255 is DC 5V output
	
  // Calibrate sensor optional see notes
  Serial.println(""); 	// blank new line
  Serial.println("Calibrating MQ7");
  mq7.calibrate();		// calculates R0
	Serial.println("Calibration done!");
}
 
void loop() {
	
  float SensorPPM = 0.0;

  // (1) 5 Volt PWM on Sensor
  Serial.println("5 volt cycle Start");
  analogWrite(PWM_PIN, PWM_HIGH_STATE); // 255 is DC 5V output
  delay(HEAT_ON_TIME);            // heat for 60 second  
  
  // (2) 1.4 Volt signal now reduce the heating power  
  Serial.println("1.4 volt cycle Start");
  analogWrite(PWM_PIN, PWM_LOW_STATE);  // turn the heater to approx 1,4V  
  delay(HEAT_LOW_TIME);            // wait for 90 seconds  
 
  // (3a) we need to read the sensor at 5V. 
  analogWrite(PWM_PIN, PWM_HIGH_STATE); // 255 is DC 5V output 
  delay (READ_DELAY_TWO);   
  SensorPPM = mq7.readPpm();
  
  // (3b)  send PPM data to serial port
  Serial.print("PPM = "); 
  Serial.println(SensorPPM);
	Serial.println(""); 	// blank new line

  // (3c)  send PPM data to bluetooth 
  BTserial.println(SensorPPM);

  // (4) wait a while & toggle LED and start again
	delay(READ_DELAY); //wait 
  digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
}
