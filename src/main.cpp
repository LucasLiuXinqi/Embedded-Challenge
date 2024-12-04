#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>

void BlueLoad();
void GreenBlink();
void ButtonInit();
void RedBlink();

void setup() {
    CircuitPlayground.begin();    // Board init
    ButtonInit();
}

void loop() {
    CircuitPlayground.clearPixels();
    // If LB pressed, start recording
    if ((PIND & (1<<PD4)) != 0){
        BlueLoad();
        GreenBlink();
    }

    // If RB pressed, start comparing
    if ((PINF & (1<<PF6)) != 0){
        BlueLoad();
        RedBlink();
    }
}


void BlueLoad(){
    CircuitPlayground.clearPixels();
    for(int8_t i = 0; i < 10; i++){
        CircuitPlayground.setPixelColor(i, 0, 0, 255);
        delay(300);
    }
}

void GreenBlink(){
    CircuitPlayground.clearPixels();
    for (int8_t j=0; j < 3; j++){
        for(int8_t i = 0; i < 10; i++){
            CircuitPlayground.setPixelColor(i, 0, 255, 0);
        }
        delay(100);
    }

}

void RedBlink(){
    CircuitPlayground.clearPixels();
    for (int8_t j=0; j < 3; j++){
        for(int8_t i = 0; i < 10; i++){
            CircuitPlayground.setPixelColor(i, 255, 0, 0);
        }
        delay(100);
    }

}

void ButtonInit(){
    // Left Button: PD4, Right Button PF6
    DDRD &= ~(1<<PD4);
    DDRF &= ~(1<<PF6);
}