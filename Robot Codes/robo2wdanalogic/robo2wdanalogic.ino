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

const int pwmMax = 255;
const int pwmMin = 80; // garante que o motor gira
typedef struct {
  int pwmEsq;
  int pwmDir;
  bool frenteEsq;
  bool frenteDir;
} ComandoMotores;

void setupHardware() {
  pinMode(MOTOR_ESQUERDO_IN1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN2, OUTPUT);
  pinMode(MOTOR_DIREITO_IN3, OUTPUT);
  pinMode(MOTOR_DIREITO_IN4, OUTPUT);
  ledcSetup(CANAL_PWM_ESQUERDO, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcSetup(CANAL_PWM_DIREITO, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(MOTOR_ESQUERDO_ENA, CANAL_PWM_ESQUERDO);
  ledcAttachPin(MOTOR_DIREITO_ENB, CANAL_PWM_DIREITO);
}

ComandoMotores interpretarComandoJoystick(int x, int y) {
  // Dead zone
  if (abs(y) <= 1 && abs(x) <= 1) {
    return {0, 0, true, true}; // parado (sinal só para frente por padrão)
  }

  int pwmBase = map(abs(y), 0, 7, 0, pwmMax - pwmMin) + pwmMin;
  int pwmEsq = pwmBase, pwmDir = pwmBase;
  bool frenteEsq, frenteDir;

  if (y > 1) { // Frente
    frenteEsq = true;
    frenteDir = true;
    if (x > 1) {
      pwmDir = pwmBase - map(x, 0, 7, 0, pwmBase); // curva direita
    } else if (x < -1) {
      pwmEsq = pwmBase - map(x, 0, -7, 0, pwmBase); // curva esquerda
    }
  } else if (y < -1) { // Ré
    frenteEsq = false;
    frenteDir = false;
    if (x > 1) {
      pwmDir = pwmBase - map(x, 0, 7, 0, pwmBase);
    } else if (x < -1) {
      pwmEsq = pwmBase - map(x, 0, -7, 0, pwmBase);
    }
  } else if (x > 1) { // Giro local direita
    pwmEsq = pwmMax; pwmDir = pwmMax;
    frenteEsq = true; frenteDir = false;
  } else { // x < -1, Giro local esquerda
    pwmEsq = pwmMax; pwmDir = pwmMax;
    frenteEsq = false; frenteDir = true;
  }
  return {constrain(pwmEsq, 0, pwmMax), constrain(pwmDir, 0, pwmMax), frenteEsq, frenteDir};
}

void aplicarComandoMotores(const ComandoMotores &cmd) {
  // Esquerdo
  if (cmd.pwmEsq > 0) {
    digitalWrite(MOTOR_ESQUERDO_IN1, cmd.frenteEsq ? HIGH : LOW);
    digitalWrite(MOTOR_ESQUERDO_IN2, cmd.frenteEsq ? LOW : HIGH);
  } else {
    digitalWrite(MOTOR_ESQUERDO_IN1, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN2, LOW);
  }
  ledcWrite(CANAL_PWM_ESQUERDO, cmd.pwmEsq);
  // Direito
  if (cmd.pwmDir > 0) {
    // Motor direito: IN3 invertido por montagem
    digitalWrite(MOTOR_DIREITO_IN3, cmd.frenteDir ? LOW : HIGH);
    digitalWrite(MOTOR_DIREITO_IN4, cmd.frenteDir ? HIGH : LOW);
  } else {
    digitalWrite(MOTOR_DIREITO_IN3, LOW);
    digitalWrite(MOTOR_DIREITO_IN4, LOW);
  }
  ledcWrite(CANAL_PWM_DIREITO, cmd.pwmDir);

  Serial.print("X: "); Serial.print(GamePad.getx_axis());
  Serial.print(" | Y: "); Serial.print(GamePad.gety_axis());
  Serial.print(" | Esq PWM: "); Serial.print(cmd.pwmEsq);
  Serial.print(cmd.frenteEsq ? " (F) " : " (R) ");
  Serial.print("| Dir PWM: "); Serial.print(cmd.pwmDir);
  Serial.println(cmd.frenteDir ? " (F)" : " (R)");
}

void setup() {
  Serial.begin(115200);
  Dabble.begin("Robo_2WD_ESP32");
  setupHardware();
  Serial.println("Bluetooth Robo analógico (funcional) iniciado.");
}

void loop() {
  Dabble.processInput();
  int x = GamePad.getx_axis();
  int y = GamePad.gety_axis();
  ComandoMotores comando = interpretarComandoJoystick(x, y);
  aplicarComandoMotores(comando);
  delay(25); // anti-ruído
}
