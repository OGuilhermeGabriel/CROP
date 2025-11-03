#include <Arduino.h>
#include <DabbleESP32.h>

#define MOTOR_ESQUERDO_ENA 25
#define MOTOR_ESQUERDO_IN1 26
#define MOTOR_ESQUERDO_IN2 27
#define MOTOR_DIREITO_ENB 14
#define MOTOR_DIREITO_IN3 33
#define MOTOR_DIREITO_IN4 32

#define CANAL_PWM_ESQUERDO 0
#define CANAL_PWM_DIREITO  1
#define FREQUENCIA_PWM     1000
#define RESOLUCAO_PWM      8

const int pwm_ativo = 255; //valor definido para o pwm | define a velocidade

void setup() {
  Serial.begin(115200); //canal para a comunicação serial
  Dabble.begin("Robo_2WD_ESP32");

  //instanciando todos os pinos de direção: IN1 & IN2 (motor esquerdo: HIGH/LOW) | IN3 & IN4 (motor direito: HIGH/LOW)
  pinMode(MOTOR_ESQUERDO_IN1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN2, OUTPUT);
  pinMode(MOTOR_DIREITO_IN3, OUTPUT);
  pinMode(MOTOR_DIREITO_IN4, OUTPUT);
  //inicializando os 2 Pwm: canal, frequência e resolução
  ledcSetup(CANAL_PWM_ESQUERDO, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcSetup(CANAL_PWM_DIREITO, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(MOTOR_ESQUERDO_ENA, CANAL_PWM_ESQUERDO);
  ledcAttachPin(MOTOR_DIREITO_ENB, CANAL_PWM_DIREITO);

  Serial.println("Bluetooth Robo iniciado.");
}

void loop() {
  Dabble.processInput(); //em loop, processa os comandos de input recebidos do dabble via BLE (bluetooth low energy)

  if (GamePad.isUpPressed()) {
    //cima = frente: HIGH = IN1,IN4 | LOW = IN3,IN2 
    Serial.println("Frente");
    digitalWrite(MOTOR_ESQUERDO_IN1, HIGH);
    digitalWrite(MOTOR_ESQUERDO_IN2, LOW);
    digitalWrite(MOTOR_DIREITO_IN3, LOW);
    digitalWrite(MOTOR_DIREITO_IN4, HIGH);
    ledcWrite(CANAL_PWM_ESQUERDO, pwm_ativo);
    ledcWrite(CANAL_PWM_DIREITO, pwm_ativo);
  }
  else if (GamePad.isDownPressed()) {
    //baixo = ré: HIGH = IN2,IN3 | LOW = IN1,IN4
    Serial.println("Ré");
    digitalWrite(MOTOR_ESQUERDO_IN1, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN2, HIGH);
    digitalWrite(MOTOR_DIREITO_IN3, HIGH);
    digitalWrite(MOTOR_DIREITO_IN4, LOW);
    ledcWrite(CANAL_PWM_ESQUERDO, pwm_ativo);
    ledcWrite(CANAL_PWM_DIREITO, pwm_ativo);
  }
  else if (GamePad.isRightPressed()) {
    //direita: HIGH = IN1, IN3 | LOW = IN2, IN4
    Serial.println("Direita");
    digitalWrite(MOTOR_ESQUERDO_IN1, HIGH);
    digitalWrite(MOTOR_ESQUERDO_IN2, LOW);
    digitalWrite(MOTOR_DIREITO_IN3, HIGH);
    digitalWrite(MOTOR_DIREITO_IN4, LOW);
    ledcWrite(CANAL_PWM_ESQUERDO, pwm_ativo);
    ledcWrite(CANAL_PWM_DIREITO, pwm_ativo);
  }
  else if (GamePad.isLeftPressed()) {
    //esquerda: HIGH = IN1, IN3 | LOW = IN2, IN4
    Serial.println("Esquerda");
    digitalWrite(MOTOR_ESQUERDO_IN1, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN2, HIGH);
    digitalWrite(MOTOR_DIREITO_IN3, LOW);
    digitalWrite(MOTOR_DIREITO_IN4, HIGH);
    ledcWrite(CANAL_PWM_ESQUERDO, pwm_ativo);
    ledcWrite(CANAL_PWM_DIREITO, pwm_ativo);
  }
  else {
    //parar: LOW = IN1, IN2, IN3, IN4
    Serial.println("Parar");
    ledcWrite(CANAL_PWM_ESQUERDO, 0);
    ledcWrite(CANAL_PWM_DIREITO, 0);
    digitalWrite(MOTOR_ESQUERDO_IN1, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN2, LOW);
    digitalWrite(MOTOR_DIREITO_IN3, LOW);
    digitalWrite(MOTOR_DIREITO_IN4, LOW);
  }
}
