/*
IDEA:TE 2016
Networked Fabrications: An IoT Primer
Brett Ian Balogh
brettbalogh@gmail.com
June 20-22, 2016
*/
//
// Includes
//
#include "neopixel/neopixel.h"
//
// Preprocessor directives
//
#define BUTTON_PIN 6
#define STATUS_PIN 7
#define PIXEL_PIN 0
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812B
//
// Create objects
//
Timer buttonTimer(2000, timerStop);
Timer fadeInTimer(100, fadeIn, 1);
Timer fadeOutTimer(100, fadeOut, 1);
Adafruit_NeoPixel ring = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
//
// Variable Declarations
//
int pastTime = 0;
int currentTime = 0;
int interval = 1500;
bool buttonState = false;
bool lastButtonState = false;
int buttonPushCounter = 0;
int buttonPresses = 0;
bool busy = false;
//
// Setup
//
void setup() {
    //
    // Register cloud stuff
    //
    Particle.subscribe("B-EVENT", buttonEventHandler);
    Particle.function("B-FUNCTION", buttonFunction);
    Particle.variable("B-COUNTER", buttonPresses);
    //
    // Setup hardware
    //
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
    pinMode(STATUS_PIN, OUTPUT);
    //
    // Setup neopixels
    //
    ring.begin();
    ring.setBrightness(255);
        for(int i = 0; i < PIXEL_COUNT; i++){
        ring.setPixelColor(i,0,0,0);
    }
    ring.show();
}
//
// Main Loop
//
void loop() {
    
    buttonState = digitalRead(BUTTON_PIN);
    if(buttonState == 1){
        // start a timer to capture number of button presses
        buttonTimer.start();
        // turn our status led for debugging
        digitalWrite(STATUS_PIN, HIGH);
        // while the timer is active, count button presses
        while(buttonTimer.isActive()){
            buttonState = digitalRead(BUTTON_PIN);
            // compare the buttonState to its previous state
            if (buttonState != lastButtonState) {
                // if the state has changed, increment the counter
                if (buttonState == HIGH) {
                    // if the current state is HIGH then the button
                    // wend from off to on:
                    buttonPushCounter++;
                    
                }
                // Delay a little bit to avoid bouncing
                delay(50);
            }
            // save the current state as the last state,
            //for next time through the loop
            lastButtonState = buttonState;
        }
        digitalWrite(STATUS_PIN, LOW);
    }
}
//
// Comet
//
void comet(){
    busy = true;
    int maxBrightness=255;
    int sinx;
    int rads;
    int brightness;
    for(int k = 0; k < 5; k++){
        for(int j = 0; j < PIXEL_COUNT; j++){
            for(int i = 0; i < PIXEL_COUNT; i++){
                brightness = (maxBrightness/(PIXEL_COUNT))*i;
                ring.setColorDimmed(pixelPos(j+i),255, 255, 255, brightness);
            }
            ring.show();
            delay(40);
        }
    }
    clearPixels();
    busy = false;
}
//
// Wipe
//
void randomColorWipe(){
    
    busy = true;
    
    // Pick random 8-bit values for each color component
    
    int red = random(255);
    int grn = random(255);
    int blu = random(255);
    
    // Wipe In
    
    for(int i = 0; i < PIXEL_COUNT; i++){
        ring.setPixelColor(i,red,grn,blu);
        ring.show();
        delay(25);
    }
    
    //Wipe Out
    
    for(int i = 0; i < PIXEL_COUNT; i++){
        ring.setPixelColor(i,0,0,0);
        ring.show();
        delay(25);
    }
    clearPixels();
    busy = false;
    
}
//
// Twinkle
//
void twinkle(){
    
    busy = true;
    
    for(int i = 0; i < 256; i ++){
        int red = random(255);
        int grn = random(255);
        int blu = random(255);
        int pixel = random(PIXEL_COUNT);
        ring.setColorDimmed(pixel, red,grn, blu, i);
        ring.show();
        delay(5);
    }
    for(int i = 255; i > 0; i --){
        int red = random(255);
        int grn = random(255);
        int blu = random(255);
        int pixel = random(PIXEL_COUNT);
        ring.setColorDimmed(pixel, red,grn, blu, i);
        ring.show();
        delay(5);
    }
    for(int i = 0; i < PIXEL_COUNT; i++){
        ring.setPixelColor(i,0,0,0);
        ring.show();
        delay(25);
    }
    clearPixels();
    busy = false;
    
}
//
// Rainbow
//
void rainbow(uint8_t wait) {
    
    busy = true;
    
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel((i+j) & 255));
    }
    ring.show();
    delay(wait);
  }
  for(int i = 255; i > 0; i--){
      ring.setBrightness(i);
      ring.show();
      delay(5);
  }
  clearPixels();
  busy = false;
}
//
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
//
// Handle timer expiration
//
void timerStop(){
    //uhh
    buttonTimer.stop();
    Particle.publish("BUTTON-EVENT", String(buttonPushCounter));
    switch(buttonPushCounter){
        case 1:
            comet();
        break;
        case 2:
            twinkle();
        break;
        case 3:
            rainbow(10);
        break;
    }
    buttonPresses += buttonPushCounter;
    buttonPushCounter=0;
}
//
// Handle a button event
//
void buttonEventHandler(const char *event, const char *data){
    String stringData = String(data);
    int intData = stringData.toInt();
    switch(intData){
        case 1:
            comet();
        break;
        case 2:
            twinkle();
        break;
        case 3:
            rainbow(10);
        break;
    }
}
//
// Do something in repsonse to a function call
//
int buttonFunction(String args){
    int intArg = args.toInt();
    switch(intArg){
        case 1:
            comet();;
        break;
        case 2:
            twinkle();
        break;
        case 3:
            rainbow(10);
        break;
    }
}
//
// Utility for comet
//
int pixelPos(int i){
    int p;
    if(i < 0){
        p = PIXEL_COUNT - i;
    }
    else if(i >= PIXEL_COUNT){
        p = i - PIXEL_COUNT;
    }
    else{
        p = i;
    }
    return p;
}
//
// Utility to clear all the pixels
//
void clearPixels(){
    for(int i = 0; i < PIXEL_COUNT; i++){
        ring.setPixelColor(i, 0, 0, 0);
    }
    ring.show();
    ring.setBrightness(255);
}

void fadeIn(){
    
}
void fadeOut(){
    
}

