#include <Wire.h>
//#include <WireKinetis.h>

#include <DHT.h>
#include <LiquidCrystal.h>

#include <DS1307RTC.h>
#include <Time.h>
#include <TimeAlarms.h>

#define DHTPIN 10
#define DHTTYPE DHT11

#define SETPIN 2
#define UPPIN 3
#define DOWNPIN 8

#define LUZPIN 13

#define STATE_IDLE 0
#define STATE_CHANGE_INIT 1
#define STATE_CHANGE_END 2


DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

// ventanas de media hora
int initTime, endTime;

int state;
float ellapsedTime;
float ellapsedChangeTime;

void setup() {
  Serial.begin(9600);

  pinMode(LUZPIN, OUTPUT);

  pinMode(UPPIN, INPUT);
  pinMode(DOWNPIN, INPUT);
  pinMode(SETPIN, INPUT);

  pinMode(A4, OUTPUT);
  digitalWrite(A4, HIGH);
  pinMode(A5, OUTPUT);
  digitalWrite(A5, LOW);

  // Cargar la hora actual desde el RTC e indicar que esto suceda de forma automática durante loop()
  // Utilizamos el método RTC.get() de la libreria DS1307RTC. El RTC debe estar conectado como se
  // indica en el texto y debe tener la fecha y hora correctas
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet)
    Serial.println("Fallo de RTC");
  else
    Serial.println("Sincronizado con RTC");
  
  // Inicializo la pantalla
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  // Inicializo el sensor dht
  dht.begin();


  // Valores default para la luz
  initTime = 14 * 60;
  endTime = 8 * 60;

  state = STATE_IDLE;
  ellapsedTime = 0;
  ellapsedChangeTime = 0;
}

void loop() {
  //digitalClockDisplay();
  Serial.print("button 1: ");
  Serial.println(digitalRead(SETPIN));
  Serial.print("button 2: ");
  Serial.println(digitalRead(UPPIN));
  Serial.print("button 3: ");
  Serial.println(digitalRead(DOWNPIN));
  
  lcd.clear();

  handleState();
  
  switch(state)
  {
    case STATE_IDLE:
    mostrarTemperaturaHumedad();
    mostrarHora();
    break;

    case STATE_CHANGE_INIT:
    handleChangeInit();
    break;

    case STATE_CHANGE_END:
    handleChangeEnd();
    break;

    default:
    state = STATE_IDLE;
    Serial.println("CUALQUIERA!!!");
    break;
    
  }
  
  handleLuz();

  delay(100);
}

void handleChangeInit()
{  
  // Informo en la pantalla
  lcd.clear();
  lcd.print("Luz se enciende:");
  
  ellapsedChangeTime += (millis() / 1000);
  if(digitalRead(UPPIN) && ellapsedChangeTime > 0.5)
  {
    ellapsedChangeTime = 0;
    ellapsedTime = 0.5;

    initTime += 30;
    if(initTime >= 24*60)
      initTime = 0;
  }
  if(digitalRead(DOWNPIN) && ellapsedChangeTime > 0.5)
  {
    ellapsedChangeTime = 0;
    ellapsedTime = 0.5;

    initTime -= 30;
    if(initTime < 0)
      initTime = 24 * 60 - 30;
  }

  // La muestro
  int hora = floor(initTime / 60);
  int minuto = initTime % 60;
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  printDigitsLcd(hora);
  lcd.print(":");
  printDigitsLcd(minuto);
}

void handleChangeEnd()
{  
  // Informo en la pantalla
  lcd.clear();
  lcd.print(" Luz se apaga:  ");
  
  ellapsedChangeTime += (millis() / 1000);
  if(digitalRead(UPPIN) && ellapsedChangeTime > 0.5)
  {
    ellapsedChangeTime = 0;
    ellapsedTime = 0.5;

    endTime += 30;
    if(endTime >= 24*60)
      endTime = 0;
  }
  if(digitalRead(DOWNPIN) && ellapsedChangeTime > 0.5)
  {
    ellapsedChangeTime = 0;
    ellapsedTime = 0.5;

    endTime -= 30;
    if(endTime < 0)
      endTime = 24 * 60 - 30;
  }

  // La muestro
  int hora = floor(endTime / 60);
  int minuto = endTime % 60;
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  printDigitsLcd(hora);
  lcd.print(":");
  printDigitsLcd(minuto);
}


void handleState()
{
  ellapsedTime += (millis() / 1000);
  if(digitalRead(SETPIN) && ellapsedTime > 1)
  {
    ellapsedTime = 0;

    switch(state)
    {
      case STATE_IDLE:
      state = STATE_CHANGE_INIT;
      break;

      case STATE_CHANGE_INIT:
      state = STATE_CHANGE_END;
      break;

      default:
      state = STATE_IDLE;
      break;
    }
  }

  if(ellapsedTime > 5)
  {
    state = STATE_IDLE;
  }
}

void handleLuz()
{
  
  int correctedEndTime = endTime;
  if(endTime < initTime)
  {
    correctedEndTime += 24 * 60;
  }
  
  int hora = hour();
  int minuto = minute();

  int tiempo = hora * 60 + minuto;

  if(tiempo > initTime && tiempo < correctedEndTime)
  {
    digitalWrite(LUZPIN, HIGH);
  }
  else
  {
    digitalWrite(LUZPIN, LOW);
  }
}

/*
0000000000000000
00T:990000H:9900
*/
void mostrarTemperaturaHumedad()
{
  int h = (int)dht.readHumidity();
  int t = (int)dht.readTemperature();
  
  lcd.setCursor(2, 1);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(10, 1);
  lcd.print("H:");
  lcd.print(h);
  
}

void mostrarHora()
{
  lcd.setCursor(11, 0);
  printDigitsLcd(hour());
  lcd.print(":");
  printDigitsLcd(minute());
}

void printDigitsLcd(int digits) {
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

void digitalClockDisplay() {
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
