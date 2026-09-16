#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

// Pinos de controle XSHUT dos sensores
#define PIN_XSHUT_1 8
#define PIN_XSHUT_2 9
#define PIN_XSHUT_3 10

// Pinos dos Servos (D5, D6, D7 via biblioteca Servo)
#define PIN_SERVO_1 5
#define PIN_SERVO_2 6
#define PIN_SERVO_3 7

// Novos enderecos I2C (o padrao de fabrica e 0x29)
#define ADDR_SENSOR_1 0x30
#define ADDR_SENSOR_2 0x31
#define ADDR_SENSOR_3 0x32

VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;

Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Configura pinos XSHUT como saida
  pinMode(PIN_XSHUT_1, OUTPUT);
  pinMode(PIN_XSHUT_2, OUTPUT);
  pinMode(PIN_XSHUT_3, OUTPUT);

  // Coloca todos os sensores em estado de RESET (nivel baixo)
  digitalWrite(PIN_XSHUT_1, LOW);
  digitalWrite(PIN_XSHUT_2, LOW);
  digitalWrite(PIN_XSHUT_3, LOW);
  delay(10);

  // --- Inicializacao do Sensor 1 ---
  digitalWrite(PIN_XSHUT_1, HIGH);
  delay(10);
  sensor1.setTimeout(500);
  if (!sensor1.init()) {
    Serial.println(F("Falha ao iniciar Sensor 1"));
    while (1);
  }
  sensor1.setAddress(ADDR_SENSOR_1);

  // --- Inicializacao do Sensor 2 ---
  digitalWrite(PIN_XSHUT_2, HIGH);
  delay(10);
  sensor2.setTimeout(500);
  if (!sensor2.init()) {
    Serial.println(F("Falha ao iniciar Sensor 2"));
    while (1);
  }
  sensor2.setAddress(ADDR_SENSOR_2);

  // --- Inicializacao do Sensor 3 ---
  digitalWrite(PIN_XSHUT_3, HIGH);
  delay(10);
  sensor3.setTimeout(500);
  if (!sensor3.init()) {
    Serial.println(F("Falha ao iniciar Sensor 3"));
    while (1);
  }
  sensor3.setAddress(ADDR_SENSOR_3);

  // Inicia leitura continua em cada sensor
  sensor1.startContinuous();
  sensor2.startContinuous();
  sensor3.startContinuous();

  // Conecta os Servos
  servo1.attach(PIN_SERVO_1);
  servo2.attach(PIN_SERVO_2);
  servo3.attach(PIN_SERVO_3);

  Serial.println(F("Sistema pronto."));
}

void loop() {
  uint16_t d1 = sensor1.readRangeContinuousMillimeters();
  if (sensor1.timeoutOccurred()) { Serial.print(F("TIMEOUT S1 | ")); }

  uint16_t d2 = sensor2.readRangeContinuousMillimeters();
  if (sensor2.timeoutOccurred()) { Serial.print(F("TIMEOUT S2 | ")); }

  uint16_t d3 = sensor3.readRangeContinuousMillimeters();
  if (sensor3.timeoutOccurred()) { Serial.print(F("TIMEOUT S3 | ")); }

  // 1. ACUMULANDO AS LEITURAS (Média perfeita de 1 segundo)
  static unsigned long ultimoTempo = 0;
  static long somaD1 = 0, somaD2 = 0, somaD3 = 0;
  static int qtdLeituras = 0;

  somaD1 += d1;
  somaD2 += d2;
  somaD3 += d3;
  qtdLeituras++;

  // A cada 1000 milissegundos (1 segundo), calculamos a média e movemos os servos
  if (millis() - ultimoTempo >= 1000) {
    int mediaD1 = somaD1 / qtdLeituras;
    int mediaD2 = somaD2 / qtdLeituras;
    int mediaD3 = somaD3 / qtdLeituras;

    // Mapeia a distancia media (50mm a 500mm) para angulo do servo (0 a 180 graus)
    int angulo1 = map(constrain(mediaD1, 50, 500), 50, 500, 0, 180);
    int angulo2 = map(constrain(mediaD2, 50, 500), 50, 500, 0, 180);
    int angulo3 = map(constrain(mediaD3, 50, 500), 50, 500, 0, 180);

    // 2. APLICANDO ZONA MORTA (Deadband)
    static int ultimoAngulo1 = -1;
    static int ultimoAngulo2 = -1;
    static int ultimoAngulo3 = -1;
    
    // Aviso Servo 1 (360): o valor 90 para ele, mas a física exige um de 180 pra parar certo.
    if (abs(angulo1 - ultimoAngulo1) >= 2) {
      servo1.write(angulo1);
      ultimoAngulo1 = angulo1;
    }
    
    if (abs(angulo2 - ultimoAngulo2) >= 2) {
      servo2.write(angulo2);
      ultimoAngulo2 = angulo2;
    }
    
    if (abs(angulo3 - ultimoAngulo3) >= 2) {
      servo3.write(angulo3);
      ultimoAngulo3 = angulo3;
    }

    // Telemetria via Monitor Serial (impressa 1x por segundo)
    Serial.print(F("S1: ")); Serial.print(mediaD1); Serial.print(F("mm | "));
    Serial.print(F("S2: ")); Serial.print(mediaD2); Serial.print(F("mm | "));
    Serial.print(F("S3: ")); Serial.print(mediaD3); Serial.println(F("mm"));

    // Reseta os acumuladores para o próximo 1 segundo
    somaD1 = 0; somaD2 = 0; somaD3 = 0;
    qtdLeituras = 0;
    ultimoTempo = millis();
  }

  // O loop roda rápido (a cada ~30ms), colhendo várias medidas por segundo!
  delay(30);
}