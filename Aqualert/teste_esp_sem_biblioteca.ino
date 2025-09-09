// Defina os pinos analógicos do ESP32
#define SENSOR1_PIN 34
#define SENSOR2_PIN 35

// Distância entre os dois sensores em cm
const int distanciaSensoresCM = 45;  

// Limites de distância válidos (em cm)
const int distanciaMin = 100;  
const int distanciaMax = 200;  

// Calibração (ajuste conforme medições reais)
const float fatorConversao = 0.125;  
// Exemplo: se o sensor mede 200 cm e o ADC leu ~1600,
// fatorConversao = 200 / 1600 = 0.125

unsigned long tempoAtivacaoSensor1 = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Leitura bruta dos sensores
  int leitura1 = analogRead(SENSOR1_PIN);
  int leitura2 = analogRead(SENSOR2_PIN);

  // Converte leitura ADC em distância (cm)
  int range1 = leitura1 * fatorConversao;
  int range2 = leitura2 * fatorConversao;

  if (range1 >= distanciaMin && range1 <= distanciaMax) {
    if (tempoAtivacaoSensor1 == 0) {
      tempoAtivacaoSensor1 = millis();
    }
  }

  if (range2 >= distanciaMin && range2 <= distanciaMax) {
    if (tempoAtivacaoSensor1 != 0) {
      unsigned long tempoAtivacaoSensor2 = millis();
      unsigned long intervalo = tempoAtivacaoSensor2 - tempoAtivacaoSensor1;
      tempoAtivacaoSensor1 = 0; // Reseta

      if (intervalo > 0) {
        // Velocidade em cm/ms
        float velocidadeCMporMS = (float)distanciaSensoresCM / intervalo;

        // Converte para km/h
        float velocidadeKMH = velocidadeCMporMS * 0.036;

        Serial.print("VEL:");
        Serial.println(velocidadeKMH);
      }
    }
  }

  delay(100);
}
