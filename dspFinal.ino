/*

Bandas de Frecuencia:

Banda 1: 250 Hz – 500 Hz
Banda 2: 500 Hz – 1 kHz
Banda 3: 1 kHz – 2 kHz
Banda 4: 2 kHz – 4 kHz
Banda 5: 4 kHz – 8 kHz
Banda 6: 8 kHz – 16 kHz

*/

#define MIC_PIN 34 // Pin donde esta conectado el MAX4466
#define DAC_PIN 25   // Salida analogica al amplificador

// Dac de 12 bits
#define BIT_11 26  // Bit mas significativo
#define BIT_10 27
#define BIT_9 14
#define BIT_8 12
#define BIT_7 13
#define BIT_6 15
#define BIT_5 2
#define BIT_4 4
#define BIT_3 5
#define BIT_2 18
#define BIT_1 19
#define BIT_0 21

const long firCoeffs[1][31] = {
    // Coeficientes banda 1: 61 coeficientes simetricos
    {
      -0.00232930548097097,-0.00217113731499309,-0.00210003947650071,-0.00206689420202346,-0.00201851928327830,-0.00189967008767668,  
      -0.00165514834435025,-0.00123194486434227,-0.000581341413003793,0.000339102392917111,0.00156374803334678,0.00311732798143570,
      0.00501370356054620,0.00725497762523418,0.00983099706678311,0.0127192669715876,0.0158852842332660,0.0192832840237476,
      0.0228573782673111,0.0265430516350476,0.0302689680658489,0.0339590298669535,0.0375346224423591,0.0409169709638863,
      0.0440295310813033,0.0468003342208359,0.0491642092081112,0.0510648058385685,0.0524563524769591,0.0533050885783724,  
      0.0535903238837743
    }
};
// Numero de coeficientes
const int numTaps = 31;  // Numero de coeficientes por filtro (mitad de los 61 originales) Orden 60
float delayLine[numTaps * 2 - 1] = {0}; // Tamaño 61 retardos  // Linea de retardo para el filtro FIR (61 muestras)

void setup() {
    Serial.begin(115200);
    analogReadResolution(12); // ADC a 12 bits (0-4095)

    pinMode(BIT_11, OUTPUT);
    pinMode(BIT_10, OUTPUT);
    pinMode(BIT_9, OUTPUT);
    pinMode(BIT_8, OUTPUT);
    pinMode(BIT_7, OUTPUT);
    pinMode(BIT_6, OUTPUT);
    pinMode(BIT_5, OUTPUT);
    pinMode(BIT_4, OUTPUT);
    pinMode(BIT_3, OUTPUT);
    pinMode(BIT_2, OUTPUT);
    pinMode(BIT_1, OUTPUT);
    pinMode(BIT_0, OUTPUT);
}

void loop() {
    // Lectura de la señal
    const int micValueProm = getMicrofonoInput();
    //Serial.print("Entrada ADC: ");
    //Serial.println(micValueProm);

    /*
    // Aplicar los 6 filtros FIR (una por cada banda)
    long output = 0;
    
    for (int band = 0; band < sizeof(firCoeffs); band++) {
        output += applyFIRFilter(micValueProm, firCoeffs[band]);
    }
    Serial.print("Salida Filtro: ");
    Serial.println(output);

    const int dacValue = parlanteOutput(map(output, -1000, 1000, 0, 4095));
    Serial.print("DAC: ");
    Serial.println(dacValue);
    */
    
    //setDACValue(dacValue);
    setDACValue(micValueProm);

    long output = micValueProm;

    dacWrite(DAC_PIN, map(output, 0 , 4092, 0, 255));
}

// Promedio de la señal
int getMicrofonoInput() {
    const int n_muestras = 1;//10;
    int suma = 0;

    for (int i = 0; i < n_muestras; i++) {
        suma += limitValue(analogRead(MIC_PIN), 0, 4095); // Lectura del valor del microfono
        delayMicroseconds(31.25); // 100 // Para una frecuencia de muestreo de 32KHz o 31.25us -> 1/(32K*3)
    }
  
    return suma / n_muestras;
}

// Mapeo para la salida del parlante
int parlanteOutput(int lectura) {
    return limitValue(lectura, 0, 4095); // map(lectura, 500, 3500, 0, 255), 0, 255 // map(lectura, 930, 2170, 0, 255); // map(lectura, 0, 4095, 0, 255);
}

// Aplicar el filtro FIR (optimizado para simetría)
float applyFIRFilter(int input, const float* coeffs) {
    // Desplazar la línea de retardo
    for (int i = numTaps * 2 - 2; i > 0; i--) {
        delayLine[i] = delayLine[i - 1];
    }
    delayLine[0] = input;

    // Calcular la salida del filtro (aprovechando la simetría)
    float output = 0;
    for (int i = 0; i < numTaps; i++) {
        output += (delayLine[i] + delayLine[numTaps * 2 - 2 - i]) * coeffs[i];
    }

    return output;
}

// Funcion para enviar un valor de 12 bits al DAC R-2R
void setDACValue(int value) {
    digitalWrite(BIT_11, (value >> 11) & 0x01);
    digitalWrite(BIT_10, (value >> 10) & 0x01);
    digitalWrite(BIT_9, (value >> 9) & 0x01);  
    digitalWrite(BIT_8, (value >> 8) & 0x01);  
    digitalWrite(BIT_7, (value >> 7) & 0x01);  
    digitalWrite(BIT_6, (value >> 6) & 0x01);  
    digitalWrite(BIT_5, (value >> 5) & 0x01);  
    digitalWrite(BIT_4, (value >> 4) & 0x01);  
    digitalWrite(BIT_3, (value >> 3) & 0x01);  
    digitalWrite(BIT_2, (value >> 2) & 0x01);  
    digitalWrite(BIT_1, (value >> 1) & 0x01);  
    digitalWrite(BIT_0, value & 0x01);         
}

int limitValue(int value, int minValue, int maxValue) {
    if (value < minValue) {
      value = minValue;
    } else if (value > maxValue) {
      value = maxValue;
    } 

    return value;
}
