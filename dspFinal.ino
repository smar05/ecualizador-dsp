#include <Arduino.h>

#define ADC_PIN 34  // Pin ADC del ESP32
#define DAC_PIN 25  // Pin DAC del ESP32
#define FS 2000     // Frecuencia de muestreo en Hz
#define N 51        // Orden del filtro FIR (impar)

// Coeficientes FIR generados para un pasa banda de 250 a 500 Hz
float h[N] = {
    -0.0021, -0.0027, -0.0031, -0.0029, -0.0018,  0.0003,  0.0034,  0.0072,  0.0111,  0.0143,
     0.0156,  0.0141,  0.0094,  0.0015, -0.0091, -0.0214, -0.0333, -0.0425, -0.0468, -0.0448,
    -0.0355, -0.0188,  0.0044,  0.0324,  0.0628,  0.0931,  0.1207,  0.1428,  0.1573,  0.1621,
     0.1573,  0.1428,  0.1207,  0.0931,  0.0628,  0.0324,  0.0044, -0.0188, -0.0355, -0.0448,
    -0.0468, -0.0425, -0.0333, -0.0214, -0.0091,  0.0015,  0.0094,  0.0141,  0.0156,  0.0143,
     0.0111
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
