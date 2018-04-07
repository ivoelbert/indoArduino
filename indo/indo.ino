#include <Wire.h>
#include <WireKinetis.h>

#include <DHT.h>
#include <LiquidCrystal.h>

#include <DS1307RTC.h>
#include <Time.h>
#include <TimeAlarms.h>

#define DHTPIN 10
#define DHTTYPE DHT11

#define INITPIN 2
#define ENDPIN 3

#define LUZPIN 13

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

// Boludes
volatile int sec;


// ventanas de media hora
int initTime, endTime;

void setup() {
  Serial.begin(9600);

  pinMode(LUZPIN, OUTPUT);

  pinMode(INITPIN, INPUT_PULLUP);
  pinMode(ENDPIN, INPUT_PULLUP);

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
  sec = 0;

  // Interrupciones
  attachInterrupt(digitalPinToInterrupt(INITPIN), raiseInit, RISING );
  attachInterrupt(digitalPinToInterrupt(ENDPIN), raiseEnd, RISING );
}

void loop() {
  digitalClockDisplay();

  lcd.clear();
  mostrarTemperaturaHumedad();
  mostrarHora();
  
  handleLuz();

  delay(1000 * 30);
}


// Cuando se toca el boton para la hora de encendido
void raiseInit()
{
  int segundo = second();
  if(segundo > sec)
  {
    // Informo en la pantalla
    lcd.clear();
    lcd.print("Luz se enciende:");
  
    // Cambio la hora de inicio
    initTime += 30;
    if(initTime >= 24*60)
      initTime = 0;
  
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
}

// Cuando se toca el boton para la hora de apagado
void raiseEnd()
{
  int segundo = second();
  if(segundo > sec)
  {
    // Informo en la pantalla
    lcd.print(" Luz se apaga:  ");
  
    // Cambio la hora de inicio
    endTime += 30;
    if(endTime >= 24*60)
      endTime = 0;
      
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
