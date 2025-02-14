#include <Arduino.h>

#define ADC_PIN 34  // Pin ADC del ESP32
#define DAC_PIN 25  // Pin DAC del ESP32
#define FS 8000     // Aumentar la frecuencia de muestreo a 8 kHz
#define N 71        // Aumentar el orden del filtro para mejor respuesta

// Coeficientes FIR ajustados con ventana de Hamming para un pasa banda de 250 a 500 Hz
float h[N] = {
    -0.0010, -0.0012, -0.0009,  0.0003,  0.0023,  0.0042,  0.0042,  0.0007, -0.0059, -0.0123,
    -0.0133, -0.0055,  0.0098,  0.0239,  0.0258,  0.0093, -0.0205, -0.0485, -0.0532, -0.0228,
     0.0435,  0.1138,  0.1510,  0.1294,  0.0460, -0.0685, -0.1678, -0.2032, -0.1678, -0.0685,
     0.0460,  0.1294,  0.1510,  0.1138,  0.0435, -0.0228, -0.0532, -0.0485, -0.0205,  0.0093,
     0.0258,  0.0239,  0.0098, -0.0055, -0.0133, -0.0123, -0.0059,  0.0007,  0.0042,  0.0042,
     0.0023,  0.0003, -0.0009, -0.0012, -0.0010
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
