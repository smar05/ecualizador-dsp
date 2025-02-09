#define MIC_PIN 34 // Pin donde esta conectado el MAX4466
#define DAC_PIN 25   // Salida analogica al amplificador

void setup() {
    Serial.begin(115200);
    analogReadResolution(12); // ADC a 12 bits (0-4095)
}

void loop() {
    // Lectura de la señal
    const int micValueProm = getMicrofonoInput();
    // Serial.println(micValueProm); 
    const int outputParlante = parlanteOutput(micValueProm);
    Serial.println(outputParlante); 
    // Enviar señal analógica al amplificador
    dacWrite(DAC_PIN, outputParlante);
}

// Promedio de la señal
int getMicrofonoInput() {
    const int n_muestras = 10;
    int suma = 0;
    int readValue = 0;
    
    for (int i = 0; i < n_muestras; i++) {
        // readValue = analogRead(MIC_PIN); // constrain(analogRead(MIC_PIN), 0, 4095);        
        readValue = limitValue(analogRead(MIC_PIN), 0, 4095);
        suma += readValue; // Lectura del valor del microfono
        delayMicroseconds(100);
    }
  
    return suma / n_muestras;
}

// Mapeo para la salida del parlante
int parlanteOutput(int lectura) {
    // Convertir a 8 bits (0-255)
    int salida = map(lectura, 500, 3500, 0, 255); // map(lectura, 930, 2170, 0, 255); // map(lectura, 0, 4095, 0, 255);
    salida = limitValue(salida, 0, 255);
    // salida = constrain(salida, 0, 255);
    
    return salida;
}

int limitValue(int value, int minValue, int maxValue) {
    if (value < minValue) {
      value = minValue;
    } else if (value > maxValue) {
      value = maxValue;
    } 

    return value;
}





/*const int MIC_PIN = 34; //A1;  // Pin donde esta conectado el MAX4466

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // ADC a 12 bits (0-4095)
}

void loop() {
  int micValue = analogRead(MIC_PIN);  // Lectura del valor del microfono
  Serial.println(micValue);
  delay(10);
}*/
