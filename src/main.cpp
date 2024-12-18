/*
    Embedded Challenge
    Lucas Liu       - xl4600
    Yipeng Wang     - yw6514
    Nazar Vaskiv    - nv2121
*/

#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>
#include <SPI.h>



/* ===Function Prototypes=== */
// Data Collection
void GreenBlink();
void ButtonInit();
void RedBlink();
void PixelsInit();
void AccInit();
void ReadAcc(int16_t, int16_t, int16_t);
void CollectAcc(int16_t[60][3]);

// Pattern Matching
void SmoothData(int16_t[60][3]);
float ComputeDist(int16_t*, int16_t*, int);
float DTW3D(int16_t[60][3], int16_t[60][3]);
void CompareData(int16_t[60][3], int16_t[60][3]);



/* ===Global Variables=== */
// 2D array of size [60, 3] - 60 sets of (a_x, a_y, a_z)
int16_t LockData[60][3];    // Lock Pattern, 
int16_t UnlockData[60][3];  // Unlock Pattern

// Locking State Flag
bool LockFlag = false;



/* ===Function Definitions=== */
void setup() {
    PixelsInit();
    ButtonInit();
    AccInit();
}

void loop() {
    // If LB pressed, start recording
    if (((PIND & (1<<PD4)) != 0) & (!LockFlag)){
        CollectAcc(LockData);

        // for (int i = 0; i < 60; i++) {
        //     Serial.print(LockData[i][0]);
        //     Serial.print(", ");
        //     Serial.print(LockData[i][1]);
        //     Serial.print(", ");
        //     Serial.println(LockData[i][2]);
        // }

        GreenBlink();
        LockFlag = true;
    }

    // If RB pressed, start comparing
    if ((PINF & (1<<PF6)) != 0){

        CollectAcc(UnlockData);
        CompareData(LockData, UnlockData);

        if (LockFlag){
            RedBlink();
        } else {
            GreenBlink();
        }
    }
}

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

void SmoothData(int16_t data[60][3]) {
    // Moving Average Filter for each axis
    // average 5 datas per iteration
    for (int ax = 0; ax < 3; ++ax) {
        for (int i = 2; i < 58; ++i) {
            data[i][ax] = (data[i-2][ax]
                            + data[i-1][ax]
                            + data[i][ax]
                            + data[i+1][ax]
                            + data[i+2][ax]) / 5;
        }
    }
}

float ComputeDist(int16_t* seq1, int16_t* seq2, int length) {
    // In this version of DTW distance calculation
    // we update each row for 60 iterations rather than using a DTW matix
    // so that the memory would not crash

    float prevRow[61] = {INFINITY};
    float currRow[61];
    prevRow[0] = 0;

    for (int i = 1; i <= length; ++i) {
        currRow[0] = INFINITY;
        for (int j = 1; j <= length; ++j) {
            // Formula for each slot of DTW Matrix (or each slot of currRow)
            // M(i, j) = |S1(i) - S2(j)| + min(M(i-1, j-1), M(i-1, j) M(i, j-1))
            // M = DTW Matrix
            //            S1 = sequence #1
            //                    S2 = sequence #2
            //                                 M(i-1, j-1) = prevRow[j-1]
            //                                              M(i-1, j) = prevRow[j]
            //                                                        M(i, j-1) = currRow[j-1]
            float base = abs(seq1[i-1] - seq2[j-1]);
            currRow[j] = base + min(prevRow[j], min(currRow[j-1], prevRow[j-1]));
        }
        // update matrix row
        memcpy(prevRow, currRow, sizeof(currRow));
    }

    return currRow[length];
}

float DTW3D(int16_t data1[60][3], int16_t data2[60][3]) {
    // Extract data of different axis
    int16_t seq1x[60], seq1y[60], seq1z[60];
    int16_t seq2x[60], seq2y[60], seq2z[60];

    for (int i = 0; i < 60; ++i) {
        seq1x[i] = data1[i][0];
        seq1y[i] = data1[i][1];
        seq1z[i] = data1[i][2];

        seq2x[i] = data2[i][0];
        seq2y[i] = data2[i][1];
        seq2z[i] = data2[i][2];
    }

    // sum the distances of each axis
    float distX = ComputeDist(seq1x, seq2x, 60);
    float distY = ComputeDist(seq1y, seq2y, 60);
    float distZ = ComputeDist(seq1z, seq2z, 60);

    float totalDist = distX + distY + distZ;
    
    return totalDist;
}

void CompareData(int16_t LD[60][3], int16_t ULD[60][3]) {
    // smoothen Lock & Unlock Data
    SmoothData(LD);
    SmoothData(ULD);

    // Serial.println("Data Smoothen");

    float distance = DTW3D(LD, ULD);

    // Typical Difference Threshold selected is 200000
    if (distance < 200000) {
        LockFlag = false;
    } else {
        LockFlag = true;
    }
}
