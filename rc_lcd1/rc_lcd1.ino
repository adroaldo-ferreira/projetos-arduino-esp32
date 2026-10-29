#include <IRremote.h>

#define PINO_IR 2

struct Tecla {
  uint32_t codigo;
  char numero;
  int pinoLed;
};

Tecla teclas[] = {
  {0xBA45FF00, '1',  4},
  {0xB946FF00, '2',  5},
  {0xB847FF00, '3',  6},
  {0xBB44FF00, '4',  7},
  {0xBF40FF00, '5',  8},
  {0xBC43FF00, '6',  9},
  {0xF807FF00, '7', 10},
  {0xEA15FF00, '8', 11},
  {0xF609FF00, '9', 12},
  {0xE619FF00, '0',  3}
};

const int TOTAL_TECLAS = sizeof(teclas) / sizeof(teclas[0]);

char ultimaTecla = '\0';

void setup() {
  Serial.begin(9600);

  IrReceiver.begin(PINO_IR, ENABLE_LED_FEEDBACK);

  // Configura os LEDs nos pinos 3 a 12
  for (int pino = 3; pino <= 12; pino++) {
    pinMode(pino, OUTPUT);
    digitalWrite(pino, LOW);
  }

  Serial.println("Controle remoto pronto!");
}

void desligarLeds() {
  for (int pino = 3; pino <= 12; pino++) {
    digitalWrite(pino, LOW);
  }
}

void acenderLed(char numero) {
  for (int i = 0; i < TOTAL_TECLAS; i++) {
    if (teclas[i].numero == numero) {
      desligarLeds();

      digitalWrite(teclas[i].pinoLed, HIGH);

      Serial.print("Tecla pressionada: ");
      Serial.println(numero);

      Serial.print("LED aceso no pino: ");
      Serial.println(teclas[i].pinoLed);

      return;
    }
  }
}

void loop() {
  if (IrReceiver.decode()) {
    uint32_t codigo = IrReceiver.decodedIRData.decodedRawData;

    bool repeticao =
      IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

    if (repeticao) {
      if (ultimaTecla != '\0') {
        acenderLed(ultimaTecla);
      }

      IrReceiver.resume();
      return;
    }

    for (int i = 0; i < TOTAL_TECLAS; i++) {
      if (codigo == teclas[i].codigo) {
        ultimaTecla = teclas[i].numero;
        acenderLed(ultimaTecla);
        break;
      }
    }

    IrReceiver.resume();
  }
}