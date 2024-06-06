#include "SD.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <math.h>
#define RXD2 16 // Pin RX GPS
#define TXD2 17 // Pin TX GPS
#define GPS_BAUD 9600  // Velocidad de baudios del módulo GPS
#define NMEA 0

double pinA = 26; // Pin sensor efecto hall
double pinB = 27; // Pin sensor efecto hall analogo
double pinV = 33;  //Pin medir tension
double pinI = 32; // Pin medir corriente ACS712
double pin_I = 35; // Pin medir corriente PM
double pinT = 34; // Pin medir temperatura T-D500


int i = 0;

char datoCmd = 0;

int pinCS = 5; // Pin modulo memoria

double corriente = 0;
double corriente1 = 0;
double temperatura = 0;
double voltaje = 0;
double latGPS;
double longGPS;
double velGPS;
double numSat;
double altGPS;
int hourGPS;
int minGPS;
int yearGPS;
double a = -2.2389e-3;
double b = 5.0061e-4;
double c = -5.9854e-7;

volatile int PPSCounter = 0;
unsigned int pulsos = 0;
unsigned Time = 0;
unsigned int RPM = 0; // Almacenador de valores de encoder
int PPR = 1;

double mVperAmp = 0.100;
int muestras = 1500;

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

File archivo;
String datos;

void setup() {

  Serial.begin(115200);

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  pinMode(pinB, INPUT);
  attachInterrupt(pinB, encoderMedicion, FALLING);

  Time = millis();

  pinMode(pinCS, OUTPUT);

  pinMode(pinV, INPUT);
  pinMode(pinI, INPUT);

  if (!SD.begin(pinCS)) {
    Serial.println("ERROR:\n Puerto CS no inicializado.");
    ESP.restart();
  }
  Serial.println("Modulo CS inicializado.");

  archivo = SD.open("/registro.CSV", FILE_WRITE);
  if (!archivo) {
    Serial.println("ERROR:\n No se ha podido crear el archivo.");
    return;
  }

  archivo.println("Velociad [RPM],Voltaje[V],Corriente[A],Corriente1[A],Temperatura[°C],Latitud,Longitud,Velocidad[GPS: km/h],Satelites, Altitud, Hora, Minutos, Year");
  archivo.close();
}

void loop() {
  // Apertura del archivo "Registro"
  archivo = SD.open("/registro.CSV", FILE_APPEND);

  // Lectura de velocidad con el encoder
  
    //Serial.println(PPSCounter);
  if (millis() - Time >= 1000) {
    pulsos = PPSCounter;
    RPM = (60 * pulsos / PPR);
    PPSCounter = 0;
    pulsos = 0;
    Time = millis();
  }
  
  

  // lectura de datos de tension y corriente
  temperatura = getTemp(muestras);
  voltaje = getVoltage(muestras);
  if (voltaje == 0) {
    corriente = 0;
  }
  else {
    corriente = getCurrent(muestras);
    //corriente1 = getCurrent1(muestras);
  }



  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isUpdated()) {
        // Si se ha recibido una nueva actualización de ubicación, muestra la información
        latGPS = gps.location.lat();
        longGPS = gps.location.lng();
        velGPS = gps.speed.kmph();
        numSat = gps.satellites.value();
        altGPS = gps.altitude.meters();
        hourGPS = gps.time.hour();
        minGPS = gps.time.minute();
        yearGPS = gps.date.year();

        
      }
    }
  }

  // Creacion String datos para la generacion del formato .CSV
        if (archivo) {
          datos = String(String(RPM) + "," + String(voltaje) + "," + String(corriente) + "," + String(corriente1) + "," + String(temperatura) +
                         "," + String(latGPS) + "," + String(longGPS) + "," + String(velGPS) + "," + String(numSat) + "," + String(altGPS) + "," + String(hourGPS) + "," + String(minGPS) +
                         "," + String(yearGPS));


          //Almacenar SD en formato .CSV
          archivo.println(datos);
          archivo.close();
        };

        Serial.print("Velocidad [RPM]: ");
        Serial.print(RPM);
        Serial.print(",");
        Serial.print("Voltaje [V]: ");
        Serial.print(voltaje);
        Serial.print(",");
        Serial.print("Corriente [A]: ");
        Serial.print(corriente);
//        Serial.print(",");
//        Serial.print("Corriente1 [A]: ");
//        Serial.print(corriente1);
        Serial.print(",");
        Serial.print("Temperatura [°C]: ");
        Serial.print(temperatura);
        Serial.print(",");
        // Lectura de datos GPS
        Serial.print("Latitud: ");
        Serial.print(latGPS);
        Serial.print(",");
        Serial.print("Longitud: ");
        Serial.print(longGPS);
        Serial.print(",");
        Serial.print("Velocidad [km/h]: ");
        Serial.print(velGPS);
        Serial.print(",");
        Serial.print("Satelites: ");
        Serial.print(numSat);
        Serial.print(",");
        Serial.print("Altitud: ");
        Serial.print(altGPS);
        Serial.print(",");
        Serial.print("Hora: ");
        Serial.print(hourGPS);
        Serial.print(",");
        Serial.print("Minutos: ");
        Serial.print(minGPS);
        Serial.print(",");
        Serial.print("Año: ");
        Serial.println(yearGPS);
        
//        Serial.print("pinB: ");
//        Serial.println(analogRead(pinB));
//        Serial.print("Pulsos: ");
//        Serial.println(pulsos);
        delay(100);

}

void encoderMedicion() {
  
  PPSCounter++;
  
}

double getCurrent(int numMuestras)
{
  double vSensor = 0;
  double iSensor = 0;

  for (int i = 0; i < numMuestras; i++) {
    vSensor += analogRead(pinI);
  }
  double Vmed = ((vSensor / numMuestras) * (3.3 / 4096.0));
  double error = 0;
  iSensor = ((Vmed - 1.494) / mVperAmp);
  error = (0.8338 * iSensor + 0.231);
  iSensor = iSensor + error;
  return iSensor;
}

double getVoltage(int numMuestras1)
{
  double vSensor1 = 0;
  double volt = 0;
  for (int j = 0; j < numMuestras1; j++) {
    vSensor1 = analogRead(pinV) * 2 / 100.0;
    volt += vSensor1;
  }
  volt = (volt / numMuestras1) - 0.47;
  if (volt < 0) {
    volt = 0;
  }
  return volt;
}

//double getCurrent1(int numMuestras1)
//{
//  double iSensor1 = 0;
//  double current = 0;
//  for (int j = 0; j < numMuestras1; j++) {
//    iSensor1 = analogRead(pin_I);
//    current += iSensor1;
//  }
//  current = (current / numMuestras1);
//
//  return current;
//}

double getTemp(int numMuestras1)
{
  double RT = 0;
  double R2 = 150e3;
  double Vin = 3.3;
  double Vout = 0;
  double V = 0;
  double T = 0;

  for (int k = 0; k < numMuestras1; k++) {
    Vout = analogRead(pinT) * 3.3 / 4095;
    V += Vout;
  }

  V = V / numMuestras1;
  RT = R2 / ((Vin / V) - 1);
  T = (1 / (a + b * log(RT) + c * pow(log(RT), 3))) - 273.15;
  return T;
}
