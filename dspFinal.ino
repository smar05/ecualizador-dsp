#include <Arduino.h>

#define ADC_PIN 34  // Pin ADC del ESP32
#define DAC_PIN 25  // Pin DAC del ESP32
#define FS 2000     // Frecuencia de muestreo en Hz
#define N 51        // Orden del filtro FIR (impar)

// Coeficientes FIR generados para un pasa banda de 250 a 500 Hz
float h[N] = {
    0.0003, -0.0000, -0.0004,  0.0016,  0.0035, -0.0000, -0.0058, -0.0044,  0.0016, -0.0000,
   -0.0025,  0.0103,  0.0213, -0.0000, -0.0309, -0.0217,  0.0076, -0.0000, -0.0111,  0.0464,
    0.0989, -0.0000, -0.1750, -0.1566,  0.0927,  0.2496,  0.0927, -0.1566, -0.1750, -0.0000,
    0.0989,  0.0464, -0.0111, -0.0000,  0.0076, -0.0217, -0.0309, -0.0000,  0.0213,  0.0103,
   -0.0025, -0.0000,  0.0016, -0.0044, -0.0058, -0.0000,  0.0035,  0.0016, -0.0004, -0.0000,
    0.0003
};

float buffer[N] = {0};  // Buffer circular para almacenar muestras
int bufferIndex = 0;

void setup() {
    Serial.begin(115200);
}

void loop() {
    static unsigned long lastTime = 0;
    unsigned long now = micros();
    if (now - lastTime >= 1000000 / FS) {  // Controlar la frecuencia de muestreo
        lastTime = now;
        
        // Leer señal de entrada
        int rawValue = analogRead(ADC_PIN);
        float inputSignal = (rawValue / 4095.0) * 3.3;  // Convertir ADC (0-4095) a voltaje (0-3.3V)

        // Almacenar la nueva muestra en el buffer circular
        buffer[bufferIndex] = inputSignal;
        
        // Aplicar el filtro FIR
        float outputSignal = 0.0;
        for (int i = 0; i < N; i++) {
            int idx = (bufferIndex - i + N) % N;  // Manejar el buffer circular
            outputSignal += buffer[idx] * h[i];
        }
        
        // Convertir la salida a rango de DAC (0-255) y escribir al DAC
        int dacValue = (outputSignal / 3.3) * 255;
        dacValue = constrain(dacValue, 0, 255);
        dacWrite(DAC_PIN, dacValue);
        
        // Avanzar el índice del buffer
        bufferIndex = (bufferIndex + 1) % N;
    }
}
