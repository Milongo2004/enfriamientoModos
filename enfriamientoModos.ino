

/*
  Fecha de creación: 29 DE MAYO DE 2024
  por: Juan Camilo Alvarez Jaramillo

  Permite programar tiempo de cuenta regresiva a voluntad, inicia la cuenta regresiva con la presión de un botón, se programa con otro
  y se para con otro. 3 botones en total, tres salidas a Relé, + display lcd.



*/
//línea de prueba
//defino las entradas,

#define reset 39
#define programmer 34
#define inicio 35

//defino las salidas,

#define cilindroEstira 18
#define cilindroRecoge 21
#define expulsa 23


bool modoManual = false;
//incluyo librerías

// DISPLAY LCD

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_SDA 15
#define I2C_SCL 2

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//defino variables

int tiempoSegundos;
int confirmacion; //se debe llevar nuevamente a 0 cuando se presione Reset.
int esperaConfirmacion;
int tiempoDeAccion; // lo que se puede tolerar funcionando un cilindro después de realizada la labor.
String cuenta;
byte cuentaStart = 0; //cuento las veces que presiono start para realizar las dos funciones de extender y contraer cilindro con el botón de start



void setup() {

  // defino pines como entradas y salidas

  //entradas

  pinMode(reset, INPUT);
  pinMode(programmer, INPUT);
  pinMode(inicio, INPUT);
  //salidas
  pinMode(cilindroEstira, OUTPUT);
  pinMode(cilindroRecoge, OUTPUT);
  pinMode(expulsa, OUTPUT);

  //estado inicial de los relevos

  estadoInicial();

  int tiempoSegundos = 0;
  int confirmacion = 0; //se debe llevar nuevamente a 0 cuando se presione Reset.
  int esperaConfirmacion = 5000;
  int tiempoDeAccion = 2; // lo que se puede tolerar funcionando un cilindro después de realizada la labor.
  String cuenta = "0";
  Serial.begin (115200);

  Wire.begin(I2C_SDA, I2C_SCL);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Inicio");
  delay(1000);

}

void loop() {
  if (modoManual == false) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Modo Automatico");

    delay(1000);

    lcd.clear();

    lcd.setCursor(0, 1);
    lcd.print("Tiempo:" + cuenta + "Seg      ");
  }
  while (modoManual == false) {
    verSiPresionaReset();

    if (digitalRead(programmer) == HIGH) {
      while (digitalRead(programmer) == HIGH) {
        delay(500);
      }
      confirmacion = 0;
      tiempoSegundos += 30;
      cuenta = String(tiempoSegundos);
      lcd.setCursor(0, 1);
      lcd.print("Tiempo:" + cuenta + "Seg      ");
      Serial.print("tiempo programado: ");
      Serial.println(tiempoSegundos);
    }

    if ((digitalRead(inicio) == HIGH) && tiempoSegundos > 0) {
      while (digitalRead(inicio) == HIGH) {
        delay(500);
      }
      if (confirmacion == 0) {
        confirmacion++;
        Serial.print("confirmación");
        Serial.println(confirmacion);
        for (int i = 0; i < 3; i++) {
          lcd.clear();
          delay(300);
          lcd.setCursor(0, 1);
          lcd.print("Tiempo:" + cuenta + "Seg      ");
          lcd.setCursor(0, 0);
          lcd.print("confirmar-->");
          delay(700);
        }
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("inicio-->");
        delay(700);
        lcd.clear();
        digitalWrite(cilindroEstira, LOW);
        digitalWrite(cilindroRecoge, HIGH);
        for (int i = tiempoSegundos; i >= 0; i--) {
          cuenta = String(i);
          lcd.setCursor(0, 1);
          lcd.print("Tiempo:" + cuenta + "Seg      ");
          Serial.print("tiempo: ");
          Serial.println(cuenta);
          delay(1000);
          verSiPresionaReset();
        }
        digitalWrite(cilindroEstira, HIGH);
        digitalWrite(cilindroRecoge, LOW);

        for (int i = 5; i > 0; i--) {
          cuenta = String(i);
          lcd.setCursor(0, 0);
          lcd.print("Expulsando en: ");
          lcd.setCursor(4, 1);
          lcd.print(cuenta + "Seg          ");
          Serial.print("expulsando en: ");
          Serial.println(cuenta);
          delay(1000);
          verSiPresionaReset();
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Expulsar");

        digitalWrite(expulsa, LOW);
        delay(1500);
        digitalWrite(expulsa, HIGH);

        cuenta = String(tiempoSegundos);

        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Tiempo:" + cuenta + "Seg      ");

      }
    }
  }

  if (modoManual == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Modo Manual");

    estadoInicial();
  }
  while (modoManual == true) { //cuando se activa modo manual.



    if (digitalRead(inicio) == HIGH) {
      while (digitalRead(inicio) == HIGH) {
        delay(500);
      }
      if (cuentaStart == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Extiende");
        digitalWrite(cilindroEstira, LOW);
        digitalWrite(cilindroRecoge, HIGH);
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Contrae");

        digitalWrite(cilindroEstira, HIGH);
        digitalWrite(cilindroRecoge, LOW);
      }
      cuentaStart++;
    }

    if (digitalRead(programmer) == HIGH) {
      while (digitalRead(programmer) == HIGH) {
        delay(500);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Expulsar");

      digitalWrite(expulsa, LOW);
      delay(1500);
      digitalWrite(expulsa, HIGH);


    }

    verSiPresionaReset();

  }
}

void verSiPresionaReset() {
  if (digitalRead(reset) == HIGH) {
    unsigned long resetTime = millis();
    while (digitalRead(reset) == HIGH) {
      if (millis() - resetTime > 2000) {
        modoManual = !modoManual;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CAMBIO DE MODO");
        delay(1000);
        lcd.clear();
        return;

      }
      delay(500);

    }

    Serial.println("presionó Stop");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET");
    estadoInicial();


  }
  else {

  }
}
void estadoInicial() {
  digitalWrite(cilindroEstira, HIGH);
  digitalWrite(cilindroRecoge, LOW);
  digitalWrite(expulsa, HIGH);
}
