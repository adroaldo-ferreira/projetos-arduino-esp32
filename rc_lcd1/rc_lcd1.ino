
#include <IRremote.h>
#include <Adafruit_LiquidCrystal.h>

#define PINO_IR 2

Adafruit_LiquidCrystal lcd_1(0);

struct Tecla {
  uint32_t codigo;
  const char* nome;
};

// Novo layout do controle remoto
Tecla teclas[] = {
  {0xBA45FF00, "1"},
  {0xB946FF00, "2"},
  {0xB847FF00, "3"},
  {0xBB44FF00, "4"},
  {0xBF40FF00, "5"},
  {0xBC43FF00, "6"},
  {0xF807FF00, "7"},
  {0xEA15FF00, "8"},
  {0xF609FF00, "9"},
  {0xE619FF00, "0"},
  {0xE916FF00, "*"},
  {0xF20DFF00, "#"}
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

    uint32_t codigo =
      IrReceiver.decodedIRData.decodedRawData;

    bool repeticao =
      IrReceiver.decodedIRData.flags &
      IRDATA_FLAGS_IS_REPEAT;

    // Trata repetição ao segurar uma tecla
    if (repeticao || codigo == 0xFFFFFFFF) {
      if (ultimoCodigo == 0) {
        IrReceiver.resume();
        return;
      }

      codigo = ultimoCodigo;
    }

    const char* nomeEncontrado = nullptr;

    // Procura a tecla correspondente ao código
    for (int i = 0; i < TOTAL_TECLAS; i++) {
      if (codigo == teclas[i].codigo) {
        nomeEncontrado = teclas[i].nome;
        break;
      }
    }

    // Guarda o último código válido
    if (!repeticao && nomeEncontrado != nullptr) {
      ultimoCodigo = codigo;
      ultimoNome = nomeEncontrado;
    }

    // Recupera o nome durante a repetição
    if (repeticao) {
      nomeEncontrado = ultimoNome;
      codigo = ultimoCodigo;
    }

    if (nomeEncontrado != nullptr) {

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
