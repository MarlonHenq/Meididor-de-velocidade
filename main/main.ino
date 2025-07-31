#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

MPU6050 mpu;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long startTime = 0;
bool started = false;
bool finished = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();

  mpu.initialize();
  if (!mpu.testConnection()) {
    lcd.print("MPU6050 Fail");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("Acel. 0 a 100km/h");
  delay(2000);
  lcd.clear();
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Conversão para g (aceleração gravitacional)
  float accel_x = ax / 16384.0;

  // calcula velocidade integrando 
  static float velocidade = 0.0;
  static unsigned long lastTime = millis();
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0; // em segundos
  lastTime = currentTime;

  if (fabs(accel_x) > 0.6) {  // ignorar ruído
    velocidade += accel_x * 9.81 * deltaTime; // m/s
  }

  float velocidade_kmh = velocidade * 3.6;

  // inicio da medição
  if (velocidade_kmh > 5 && !started) {
    started = true;
    startTime = millis();
  }

  // bater 100
  if (velocidade_kmh >= 100 && started && !finished) {
    unsigned long totalTime = (millis() - startTime) / 1000;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("0 a 100km/h em:");
    lcd.setCursor(0, 1);
    lcd.print(totalTime);
    lcd.print(" segundos");
    finished = true;
  }

  // to print
  if (!finished) {
    lcd.setCursor(0, 0);
    lcd.print("Acel:");
    lcd.print(accel_x, 2);
    lcd.print("g");

    lcd.setCursor(0, 1);
    lcd.print("Vel:");
    lcd.print(velocidade_kmh, 1);
    lcd.print("km/h ");
  }

  delay(100);
}
