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

//parametros do PWM
int pwmValor = 180; // valor inicial (ajustável)
const int pwmMax = 255;
const int pwmMin = 80;
const int pwmStep = 20;

//funções para acionar os motores esquerdo e direito 
void acionarMotorEsquerdo(bool frente, int pwm) {
  //motor esquerdo vai pra frente: IN1 = HIGH, IN2 = LOW 
  if (frente) {
    digitalWrite(MOTOR_ESQUERDO_IN1, HIGH);
    digitalWrite(MOTOR_ESQUERDO_IN2, LOW);
  //motor esquerdo vai pra tras: IN1 = LOW, IN2 = HIGH   
  } else {
    digitalWrite(MOTOR_ESQUERDO_IN1, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN2, HIGH);
  }
  ledcWrite(CANAL_PWM_ESQUERDO, pwm);
}

void acionarMotorDireito(bool frente, int pwm) {
  //motor direito vai pra frente: IN3 = LOW, IN4 = HIGH
  if (frente) {
    digitalWrite(MOTOR_DIREITO_IN3, LOW);   
    digitalWrite(MOTOR_DIREITO_IN4, HIGH);
  //motor direito vai pra tras: IN3 = HIGH, IN4 = LOW  
  } else {
    digitalWrite(MOTOR_DIREITO_IN3, HIGH);
    digitalWrite(MOTOR_DIREITO_IN4, LOW);
  }
  ledcWrite(CANAL_PWM_DIREITO, pwm);
}

//funções de movimento do robô (usam como base as funções de acionamento do robô)
void moverFrente() {
  acionarMotorEsquerdo(true, pwmValor);
  acionarMotorDireito(true, pwmValor);
  Serial.print("Movimento: Frente | PWM: "); Serial.println(pwmValor);
}

void moverTras() {
  acionarMotorEsquerdo(false, pwmValor);
  acionarMotorDireito(false, pwmValor);
  Serial.print("Movimento: Ré | PWM: "); Serial.println(pwmValor);
}

void girarDireita() {
  acionarMotorEsquerdo(true, pwmValor);
  acionarMotorDireito(false, pwmValor);
  Serial.print("Movimento: Direita | PWM: "); Serial.println(pwmValor);
}

void girarEsquerda() {
  acionarMotorEsquerdo(false, pwmValor);
  acionarMotorDireito(true, pwmValor);
  Serial.print("Movimento: Esquerda | PWM: "); Serial.println(pwmValor);
}

//função pra parar o robô, desliga os canais e deixa tudo low
void pararRobo() {
  ledcWrite(CANAL_PWM_ESQUERDO, 0);
  ledcWrite(CANAL_PWM_DIREITO, 0);
  digitalWrite(MOTOR_ESQUERDO_IN1, LOW);
  digitalWrite(MOTOR_ESQUERDO_IN2, LOW);
  digitalWrite(MOTOR_DIREITO_IN3, LOW);
  digitalWrite(MOTOR_DIREITO_IN4, LOW);
  Serial.println("Movimento: Parar");
}

void setup() {
  Serial.begin(115200); //saida serial
  Dabble.begin("Robo_2WD_ESP32_1");

  //setando os pinos, io's e canais PWM 
  pinMode(MOTOR_ESQUERDO_IN1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN2, OUTPUT);
  pinMode(MOTOR_DIREITO_IN3, OUTPUT);
  pinMode(MOTOR_DIREITO_IN4, OUTPUT);
  ledcSetup(CANAL_PWM_ESQUERDO, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcSetup(CANAL_PWM_DIREITO, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(MOTOR_ESQUERDO_ENA, CANAL_PWM_ESQUERDO);
  ledcAttachPin(MOTOR_DIREITO_ENB, CANAL_PWM_DIREITO);

  Serial.println("Bluetooth Robo iniciado. Use GamePad para testar. A/B para velocidade.");
}

void loop() {
  Dabble.processInput();

  //controle de velocidade: Botão quadrado aumenta, bola diminui
  if(GamePad.isSquarePressed()){
    pwmValor += pwmStep;  //pwm aumentando de acordo com o step definido
    if(pwmValor > pwmMax) pwmValor = pwmMax; //trava no pwmmax definido
    Serial.print("Aumentando velocidade: "); Serial.println(pwmValor);
    delay(200); // debounce
  } 
  else if(GamePad.isCirclePressed()){
    pwmValor -= pwmStep; //pwm diminuindo de acordo com o step definido 
    if(pwmValor < pwmMin) pwmValor = pwmMin; //trava no pwmmin definido 
    Serial.print("Diminuindo velocidade: "); Serial.println(pwmValor);
    delay(200); // debounce
  }

  //movimentos determinados no dabble sendo associadas com as funções de movimentação do robô
  if (GamePad.isUpPressed())       moverFrente();
  else if (GamePad.isDownPressed())moverTras();
  else if (GamePad.isRightPressed())girarDireita();
  else if (GamePad.isLeftPressed()) girarEsquerda();
  else                              pararRobo();
}