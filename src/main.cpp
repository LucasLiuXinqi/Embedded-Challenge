#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>
#include <SPI.h>

// void BlueLoad();
void GreenBlink();
void ButtonInit();
void RedBlink();
void PixelsInit();
void AccInit();
void ReadAcc(int16_t, int16_t, int16_t);
void CollectAcc(int16_t[60][3]);
// void CopyData(int16_t *, int16_t *);

// int16_t AccData[60][3]; // 2D array: 60 rows, 3 columns (X, Y, Z)
int16_t LockData[60][3]; // Store lock pattern
int16_t UnlockData[60][3]; // Store unlock pattern



bool LockFlag = false;

void setup() {
    PixelsInit();
    ButtonInit();
    AccInit();
}

void loop() {

    // todo:
    // Implement the unlock matching part.


    // If LB pressed, start recording
    if (((PIND & (1<<PD4)) != 0) & (!LockFlag)){
        CollectAcc(LockData);
        // for (int i = 0; i < 60; i++) {
        //     Serial.print("Sample "); Serial.print(i); Serial.print(": ");
        //     Serial.print("X = "); Serial.print(AccData[i][0]);
        //     Serial.print(", Y = "); Serial.print(AccData[i][1]);
        //     Serial.print(", Z = "); Serial.println(AccData[i][2]);
        // }
        for (int i = 0; i < 60; i++) {
            Serial.print("Sample "); Serial.print(i); Serial.print(": ");
            Serial.print("X = "); Serial.print(LockData[i][0]);
            Serial.print(", Y = "); Serial.print(LockData[i][1]);
            Serial.print(", Z = "); Serial.println(LockData[i][2]);
        }
        GreenBlink();
        LockFlag = true;
    }

    // If RB pressed, start comparing
    if ((PINF & (1<<PF6)) != 0){
        CollectAcc(UnlockData);

        // Compare LockData and UnlockData
        


        // GreenBlink();
        // LockFlag = false;

        // RedBlink();

    }


}

// void CopyData(int16_t *src, int16_t *dest){
//     for (int i = 0; i < 60; i++){
//         for (int j = 0; j < 3; j++){
//             dest[i][j] = src[i][j];
//         }
//     }
// }

// void BlueLoad(){
//     CircuitPlayground.clearPixels();
//     for(int8_t i = 0; i < 10; i++){
//         CircuitPlayground.setPixelColor(i, 0, 0, 255);
//         delay(300);
//     }
// }

void GreenBlink(){
    CircuitPlayground.clearPixels();
    for (int8_t j=0; j < 3; j++){
        for(int8_t i = 0; i < 10; i++){
            CircuitPlayground.setPixelColor(i, 0, 255, 0);
        }
        delay(100);
        CircuitPlayground.clearPixels();
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
        CircuitPlayground.clearPixels();
        delay(100);
    }

}

void ButtonInit(){
    // Left Button: PD4, Right Button PF6
    DDRD &= ~(1<<PD4);
    DDRF &= ~(1<<PF6);
}

void AccInit(){
    DDRB |= (1<<PIN4);
    SPI.begin();

    // Initialize CTRL_REG1 (0x20) to enable axes and set data rate
    PORTB &= ~(1 << PIN4);
    SPI.transfer(0x20); // Write to CTRL_REG1
    SPI.transfer(0x37); // Enable all axes, set 20Hz data rate
    PORTB |= (1 << PIN4);

    // Initialize CTRL_REG4 (0x23) for data alignment
    PORTB &= ~(1 << PIN4);
    SPI.transfer(0x23); // Write to CTRL_REG4
    SPI.transfer(0x00); // ±2g, continuous update
    PORTB |= (1 << PIN4);
}

void PixelsInit(){
    CircuitPlayground.begin();    // Board init
}

void ReadAcc(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t buffer[6];

    // Set chip select low
    PORTB &= ~(1 << PIN4);

    // Send the address of OUT_X_L (start of X, Y, Z data) with auto-increment
    SPI.transfer(0x28 | (1 << 7) | (1 << 6)); // Read, auto-increment

    // Read 6 bytes (X_L, X_H, Y_L, Y_H, Z_L, Z_H)
    for (int i = 0; i < 6; i++) {
        buffer[i] = SPI.transfer(0xFF); // Send dummy byte to read
    }

    // Set chip select high
    PORTB |= (1 << PIN4);

    // Combine high and low bytes for each axis
    *x = (int16_t)(buffer[1] << 8 | buffer[0]);
    *y = (int16_t)(buffer[3] << 8 | buffer[2]);
    *z = (int16_t)(buffer[5] << 8 | buffer[4]);
}

void CollectAcc(int16_t dst[60][3]) {
    for (int i = 0; i < 60; i++) {
        // Read X, Y, Z values
        int16_t x, y, z;
        ReadAcc(&x, &y, &z); // Pass by reference to get the values

        // Store values in the buffer
        dst[i][0] = x; // X-axis
        dst[i][1] = y; // Y-axis
        dst[i][2] = z; // Z-axis

        // Wait for the next sample (50 ms for 20 Hz sampling rate)

        CircuitPlayground.setPixelColor(i/6, 0, 0, 255);

        delay(50);

    }
}