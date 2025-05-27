#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

// Motor pinleri
int sag_enable = 18;
int sag_ileri  = 19;
int sag_geri   = 21;
int sol_ileri  = 22;
int sol_geri   = 23;
int sol_enable = 5;

// Çizgi sensörleri
int sol_sensor  = 12;
int orta_sensor = 13;
int sag_sensor  = 26;

// Ultrasonik sensör
#define TRIG 17
#define ECHO 16

// Hız
const int HIZ = 55;

// Modlar
enum Mode { LINE, OBSTACLE };
Mode aktifMod = LINE;

void setup() {
  pinMode(sag_ileri, OUTPUT); pinMode(sag_geri, OUTPUT);
  pinMode(sol_ileri, OUTPUT); pinMode(sol_geri, OUTPUT);
  pinMode(sag_enable, OUTPUT); pinMode(sol_enable, OUTPUT);

  pinMode(sol_sensor, INPUT);
  pinMode(orta_sensor, INPUT);
  pinMode(sag_sensor, INPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.begin(115200);
  SerialBT.begin("SullyBot");
  Serial.println("Bluetooth hazır. 'L' veya 'O' gönderin.");

  randomSeed(analogRead(0)); // rastgelelik için
}

void loop() {
  int sol  = digitalRead(sol_sensor);
  int orta = digitalRead(orta_sensor);
  int sag  = digitalRead(sag_sensor);
  long mesafe = mesafeOku();

  // Bluetooth komutu
  if (SerialBT.available()) {
    char komut = SerialBT.read();
    if (komut == 'L') {
      aktifMod = LINE;
      SerialBT.println("Çizgi İzleme (L) aktif");
    } else if (komut == 'O') {
      aktifMod = OBSTACLE;
      SerialBT.println("Engelden Kaçınma (O) aktif");
    }
  }

  if (aktifMod == LINE) {
    if (mesafe <= 30) {
      dur();  // Engel varsa dur
    } else {
      if (orta == 1) {
        ileri();
      } else if (sol == 1) {
        saga();
      } else if (sag == 1) {
        sola();
      } else {
        ileri();
      }
    }
  }

  else if (aktifMod == OBSTACLE) {
    if (mesafe <= 15) {
      int rastgeleYon = random(0, 2); // 0 veya 1
      if (rastgeleYon == 0) {
        saga();
      } else {
        sola();
      }
      delay(500);
    } else {
      ileri();
    }
  }

  delay(10);
}

// Motor Fonksiyonları 

void ileri() {
  digitalWrite(sag_ileri, HIGH); digitalWrite(sag_geri, LOW);
  digitalWrite(sol_ileri, LOW);  digitalWrite(sol_geri, HIGH);
  analogWrite(sag_enable, HIZ);
  analogWrite(sol_enable, HIZ);
}

void sola() {
  digitalWrite(sag_ileri, HIGH); digitalWrite(sag_geri, LOW);
  digitalWrite(sol_ileri, LOW);  digitalWrite(sol_geri, LOW);
  analogWrite(sag_enable, HIZ);
  analogWrite(sol_enable, 0);
}

void saga() {
  digitalWrite(sag_ileri, LOW);  digitalWrite(sag_geri, LOW);
  digitalWrite(sol_ileri, LOW);  digitalWrite(sol_geri, HIGH);
  analogWrite(sag_enable, 0);
  analogWrite(sol_enable, HIZ);
}

void dur() {
  analogWrite(sag_enable, 0);
  analogWrite(sol_enable, 0);
}

// === Mesafe ölçümü ===
long mesafeOku() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long sure = pulseIn(ECHO, HIGH, 30000); // max 30ms bekle
  return (sure == 0) ? 999 : sure / 58;
}
