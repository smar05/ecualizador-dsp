#include <Arduino.h>

#define ADC_PIN 34  // Pin ADC del ESP32
#define DAC_PIN 25  // Pin DAC del ESP32
#define FS 2000     // Frecuencia de muestreo en Hz
#define N 51        // Orden del filtro FIR (impar)

// Coeficientes FIR generados con MATLAB usando la ventana Hamming
float h[N] = {
    -0.0010, -0.0017, -0.0027, -0.0040, -0.0055, -0.0070, -0.0082, -0.0088, -0.0084, -0.0065,
    -0.0026,  0.0034,  0.0116,  0.0215,  0.0323,  0.0428,  0.0517,  0.0578,  0.0597,  0.0563,
     0.0469,  0.0316,  0.0119, -0.0107, -0.0341, -0.0560, -0.0741, -0.0867, -0.0925, -0.0911,
    -0.0828, -0.0688, -0.0505, -0.0294, -0.0074,  0.0139,  0.0323,  0.0464,  0.0550,  0.0574,
     0.0535,  0.0438,  0.0298,  0.0135, -0.0035, -0.0193, -0.0320, -0.0400, -0.0421, -0.0383,
    -0.0290
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
