#include <IRremote.h>
#include <Adafruit_LiquidCrystal.h>

#define PINO_IR 2
Adafruit_LiquidCrystal lcd_1(0);

struct Tecla {
  uint32_t codigo;
  const char* nome;
};

Tecla teclas[] = {
  {0xEE11FB04, "1"},
  {0xED12FB04, "2"},
  {0xEC13FB04, "3"},
  {0xEB14FB04, "4"},
  {0xEA15FB04, "5"},
  {0xE916FB04, "6"},
  {0xE817FB04, "7"},
  {0xE718FB04, "8"},
  {0xE619FB04, "9"},
  {0xEF10FB04, "0"},
  {0xF609FB04, "MUTE"},
  {0xE51AFB04, "REVIEW"},
  {0xFF00FB04, "CH+"},
  {0xFE01FB04, "CH-"},
  {0xFD02FB04, "VOL+"},
  {0xFC03FB04, "VOL-"},
  {0xF708FB04, "POWER"},
  {0xBB44FB04, "ENTER"}
};

const int TOTAL_TECLAS = sizeof(teclas) / sizeof(teclas[0]);

uint32_t ultimoCodigo = 0;
const char* ultimoNome = nullptr;

void setup() {
  lcd_1.begin(16, 2);
  Serial.begin(9600);
  IrReceiver.begin(PINO_IR, ENABLE_LED_FEEDBACK);
  lcd_1.print("Controle pronto");
}

void loop() {
  if (IrReceiver.decode()) {
    uint32_t codigo = IrReceiver.decodedIRData.decodedRawData;
    bool repeticao = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

    // Se segurou o botão, reaproveita o último válido
    if (repeticao || codigo == 0xFFFFFFFF) {
      if (ultimoCodigo == 0) {
        IrReceiver.resume();
        return;
      }
      codigo = ultimoCodigo;
    }

    const char* nomeEncontrado = nullptr;
    for (int i = 0; i < TOTAL_TECLAS; i++) {
      if (codigo == teclas[i].codigo) {
        nomeEncontrado = teclas[i].nome;
        break;
      }
    }

    // Se não é repetição, atualiza o último
    if (!repeticao && nomeEncontrado) {
      ultimoCodigo = codigo;
      ultimoNome = nomeEncontrado;
    }

    if (repeticao) {
      nomeEncontrado = ultimoNome;
      codigo = ultimoCodigo;
    }

    if (nomeEncontrado) {
      Serial.print("Tecla: ");
      Serial.print(nomeEncontrado);
      Serial.print(" | Codigo: 0x");
      Serial.println(codigo, HEX);

      lcd_1.clear();
      lcd_1.setCursor(0, 0);
      lcd_1.print("Tecla: ");
      lcd_1.print(nomeEncontrado);
      lcd_1.setCursor(0, 1);
      lcd_1.print("0x");
      lcd_1.print(codigo, HEX);
    } else {
      Serial.print("Desconhecido: 0x");
      Serial.println(codigo, HEX);
    }

    IrReceiver.resume();
  }
}
