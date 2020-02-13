#if defined(ARDUINO)
SYSTEM_MODE(MANUAL);
#endif

#include "Credentials.h"
#include "MQTT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

const int optimalPin = D8;
const int rgbBluePin = D2;
const int rgbGreenPin = D3;
const int rgbRedPin = D4;
const int connectionLedPin = D13;
const int buttonPin = D5;
const int modePin = D10;
const int brightnessPin = D17;

int previousButtonState = 0;
int buttonState = 0;
bool latchOn = false;

int connectionLedState = LOW;
unsigned int luxValue = 0;
unsigned long previousMillis = 0;
unsigned long previousMillisWiFi = 0;
unsigned int optimalValue = 0;
void callback(char* topic, byte* payload, unsigned int length);
bool skyQualityMode = false;

// Declare sensor object.
Adafruit_TSL2591 tslDevice = Adafruit_TSL2591(2591);

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
  /*
   * Callback function for when receiving new feed values.
   */
  Serial.println(topic);
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String response = String(p);
  Serial.print("Received: ");
  Serial.println(response);
  // compareLuxValues(luxValue, response);
}

MQTT mqttClient("io.adafruit.com", 1883, callback);
  
unsigned int getLuxValue(){
  /* 
   *Returns the Luminosity value from the sensor as a float.
   */
  sensors_event_t event;
  tslDevice.getEvent(&event);
  
  /* Display the results (light is measured in lux) */
  // Serial.println(event.light); 
  if (event.light) {
    return event.light;
    }
}

void setup() {
  /*
   * Setup function for program code.
   */
   
  // Set Up Pin Modes for IO
  pinMode(brightnessPin, OUTPUT);
  pinMode(optimalPin, INPUT);
  pinMode(rgbBluePin, OUTPUT);
  pinMode(rgbGreenPin, OUTPUT);
  pinMode(rgbRedPin, OUTPUT);
  pinMode(connectionLedPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLDOWN);
  pinMode(modePin, OUTPUT);

  // Set up Default LuxSensor settings.
  tslDevice.setGain(TSL2591_GAIN_MED);
  tslDevice.setTiming(TSL2591_INTEGRATIONTIME_100MS);
  
  Serial.begin(9600);

  // Connect to Access Point and Adafruit
  connectToWifi();
  connectToAdafruit();

}

void loop() {
  /*
   * Main Loop function for program code.
   */
   // Get Optimal Value
   optimalValue = analogRead(optimalPin);
   setBrightness(optimalValue/17); // Mapping 0-4096 to fit to 0-255

   // Aggregate LuxValue
   luxValue = getLuxValue();
   compareLuxValues(luxValue, optimalValue/4); // Mapping 0-4096 to fit to 0-1024
   
  if (mqttClient.isConnected()) {
    mqttClient.loop();
    Serial.println("Connected.");
    digitalWrite(connectionLedPin, HIGH);

    // Check if Sky Quality Mode and Publish
    buttonState = digitalRead(buttonPin);
    if (buttonState!= previousButtonState){
      if (buttonState == HIGH && latchOn == false) {
        digitalWrite(modePin, HIGH);
        latchOn = true;
        // Sky Quality Mode is On
        skyQualityMode = true;
        tslDevice.setGain(TSL2591_GAIN_HIGH); // Set Gain to High
        tslDevice.setTiming(TSL2591_INTEGRATIONTIME_300MS); // Set Integration Time to 300ms
        Serial.println("Sky Quality Mode: ON");
      }
      else if (buttonState == HIGH && latchOn == true){
        digitalWrite(modePin, LOW);
        latchOn = false;
        // Sky Quality Mode is Off
        skyQualityMode = false;
        tslDevice.setGain(TSL2591_GAIN_MED); // Set Gain to Medium
        tslDevice.setTiming(TSL2591_INTEGRATIONTIME_100MS); // Set Integration Time to 100ms
        Serial.println("Sky Quality Mode: OFF");
      }
    }
    previousButtonState = buttonState;

    if (skyQualityMode == true) {
      publishToAdafruitSky(String(optimalValue/4), String(luxValue), 4000);
    }
    else {
      publishToAdafruitNormal(String(optimalValue/4), String(luxValue), 4000);
    }
  
  } else {
    Serial.println("Not connected to Adafruit.");
    digitalWrite(connectionLedPin, LOW);
    connectToAdafruit();
    digitalWrite(connectionLedPin, HIGH);
  }
}

void publishToAdafruitSky(String optimalValue, String luxValue, int delayInMs) {
  /*
   * Publish to Adafruit Sky Quality Feeds.
   */
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= delayInMs) {
    previousMillis = currentMillis;
    Serial.println("Publishing " + luxValue + " to lightlevelssky.");
    mqttClient.publish("IndiePotato/feeds/lightlevelssky", luxValue, MQTT::QOS1);
    Serial.println("Publishing " + optimalValue + " to optimalvaluesky.");
    mqttClient.publish("IndiePotato/feeds/optimalvaluesky", optimalValue, MQTT::QOS1);
  }
}


void publishToAdafruitNormal(String optimalValue, String luxValue, int delayInMs) {
  /*
   * Publish to Adafruit Sky Quality Feeds.
   */
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= delayInMs) {
    previousMillis = currentMillis;
    Serial.println("Publishing " + luxValue + " to lightlevels.");
    mqttClient.publish("IndiePotato/feeds/lightlevels", luxValue, MQTT::QOS1);
    Serial.println("Publishing " + optimalValue + " to optimalvalue.");
    mqttClient.publish("IndiePotato/feeds/optimalvalue", optimalValue, MQTT::QOS1);
  }
}

void compareLuxValues (unsigned int luxValue, unsigned int optimalValue) {
  /* 
   *Compares Lux Values against Optimal Value and sets RGB LED accordingly.
   */
  
  if (luxValue <= 0.5 * optimalValue || luxValue >= 1.5 * optimalValue) {
    // Check outside of 50% Leeway
    setRGB(255, 0, 0);
  } else if (luxValue <= 0.75 * optimalValue || luxValue >= 1.25 * optimalValue) {
    // Check outside of 25% Leeway
    setRGB(0, 0, 255);
  } else if (luxValue >= 0.9 * optimalValue || luxValue <= 1.1*optimalValue) {
    // Check outside of 10% Leeway
    setRGB(0, 255, 0);
  }
}

void setBrightness (unsigned int optimalValue) {
  /* 
   *Compares Lux Values against Optimal Value and sets RGB LED accordingly.
   */
  analogWrite(brightnessPin, optimalValue);
}

void setRGB(int redValue, int greenValue, int blueValue) {
  /*
   * Sets RGB Led according to provided parameters.
   */
  digitalWrite(rgbRedPin, redValue);
  digitalWrite(rgbGreenPin, greenValue);
  digitalWrite(rgbBluePin, blueValue);
}

void qoscallback(unsigned int messageid) {
  /*
   * Quality of Service callback for Ack's.
   */
  Serial.print("Ack Message Id:");
  Serial.println(messageid);
}

void connectToAdafruit() {
  /*
   * Connects to Adafruit.
   */
  Serial.println("Connecting to Adadfruit.");
  mqttClient.connect("io.adafruit.com", USERNAME, IO_KEY);
}

void connectToWifi(){
  /*
   * Connects to Wi-Fi access point.
   */
   unsigned long currentMillis = 0;
  Serial.println("Connecting to W-Fi.");
  WiFi.on();
  WiFi.setCredentials(ACCESS_POINT, PASSWORD);
  WiFi.connect();
  Serial.println("Wi-Fi Connected.");
  delay(500);
}
