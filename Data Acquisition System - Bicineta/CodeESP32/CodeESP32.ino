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
double pinB = 27; // Pin sensor efecto hall analogo (No en uso)

double pinV = 33;  //Pin medir tension PM
double pinI = 32; // Pin medir corriente ACS712
double pin_I = 35; // Pin medir corriente PM (No en uso)
double pinT = 34; // Pin medir temperatura T-D500

int pinCS = 5; // Pin modulo memoria

int pinACC = 25; // Pin Acelerador

// Inicializacion variables de corriente, tension, temperatura y aceleracion
double corriente = 0;
double temperatura = 0;
double voltaje = 0;
double accelerator = 0;

// Inicializacion vaiables del GPS
double latGPS;
double longGPS;
double velGPS;
double numSat;
double altGPS;
int hourGPS;
int minGPS;
int yearGPS;

// Parametros sensor de temperatura
double a = -0.015769;
double b = 0.0032072;
double c = -0.00018051;
double d = 3.3808e-06;

// Inicializacion variables sensor de velocidad (HY-024)
volatile int PPSCounter = 0; // Contador de pulsos
unsigned int pulsos = 0;     // Cantidad de pulsos en 1 segundo 
unsigned Time = 0;           // Inicialización variable de tiempo
float RPM = 0;        // Inicializacion variable de velocidad actual
unsigned int RPM_ANT = 0;    // Inicializacion variable de velocidad anterior
int PPR = 2;                // Numero de pulsos que detecta el sensor por vuelta
double debounceDelay = 120;
double debounceTime = 0;
double lastDebounceTime = 0;

// Parametros sensor de corriente ACS712
double mVperAmp = 0.1;       // Sensibilidad sensor de corriente ACS712

// Numero de muestras para la medicion de datos
int muestras = 1000;

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

File archivo;
String datos;

void IRAM_ATTR magnetDetection();
int getRPM();
double getCurrent(int numMuestras);
double getVoltage(int numMuestras1);
double getTemp(int numMuestras1);
double getSignalAcceleration(int numMuestras1);

void setup() {

  Serial.begin(115200);

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);

  pinMode(pinA, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinA), magnetDetection, HIGH);

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

  archivo.println("Velocidad [RPM],Aceleracion [V_Acc], Voltaje[V],Corriente[A],Temperatura[°C],Latitud,Longitud,Velocidad[GPS: km/h],Satelites, Altitud, Hora, Minutos, Year");
  archivo.close();
}

void loop() {
  // Apertura del archivo "Registro"
  archivo = SD.open("/registro.CSV", FILE_APPEND);

  // Lectura de velocidad con el encoder
  RPM = getRPM();
  accelerator = getSignalAcceleration(muestras); 
  // Lectura de temperatura
  temperatura = getTemp(muestras);

  // lectura de datos de tension y corriente
  voltaje = getVoltage(muestras);

  if (voltaje == 0 || corriente < 0) {
    corriente = 0;
  }
  else {
    corriente = getCurrent(muestras);
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

        
      };
    };
  };
  // Creacion String datos para la generacion del formato .CSV
  if (archivo) {
          datos = String(String(RPM) +","+ String(accelerator) + "," + String(voltaje, 8) + "," + String(corriente, 8) + "," + String(temperatura, 8) +
                         "," + String(latGPS, 15) + "," + String(longGPS, 15) + "," + String(velGPS, 8) + "," + String(numSat) + "," + String(altGPS, 8) + "," + String(hourGPS) + "," + String(minGPS) +
                         "," + String(yearGPS));


          //Almacenar SD en formato .CSV
          archivo.println(datos);
          archivo.close();
        };

        Serial.print("Velocidad [RPM]: ");
        Serial.print(RPM);
        Serial.print(",");
        Serial.print("Aceleracion [V_Acc]: ");
        Serial.print(accelerator);
        Serial.print(",");
        Serial.print("Voltaje [V]: ");
        Serial.print(voltaje);
        Serial.print(",");
        Serial.print("Corriente [A]: ");
        Serial.print(corriente);
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
        
        delay(1000);
      
}


void IRAM_ATTR magnetDetection(){

    debounceTime = millis();

    if((debounceTime - lastDebounceTime) > debounceDelay){

      PPSCounter++;
      lastDebounceTime = debounceTime;

    }
    
    
}

int getRPM(){
  
  if (millis() - Time >= 1000) {

      pulsos = PPSCounter;
      RPM_ANT = RPM;
      RPM = (60 * pulsos / (PPR));
      PPSCounter = 0;
      pulsos = 0;
      Time = millis();

  }

  if(RPM <= 450){
    debounceDelay = 93;
  }
  else if(RPM > 450 && RPM <= 750){
      debounceDelay = 53;
  }
  else if(RPM > 750 && RPM <= 1000){
    debounceDelay = 33;
  }
  else if(RPM > 1000){
    debounceDelay = 5;
  }
  //Serial.println(debounceDelay);
  return RPM;

}

double getCurrent(int numMuestras){

  double vSensor = 0;
  double iSensor = 0;
  double error = 0;
  for (int i = 0; i < numMuestras; i++) {
    vSensor = analogRead(pinI) * (5.5 / 4096.0);
    iSensor += (vSensor-2.5)/mVperAmp;
  }
  
  iSensor = (iSensor/numMuestras);
  error = 0.6231*iSensor+6.1885;
  iSensor = iSensor - error;
  
  return iSensor;
}

double getVoltage(int numMuestras1){

  double vSensor1 = 0;
  double volt = 0;
  double error = 0;
  for (int j = 0; j < numMuestras1; j++) {
    vSensor1 = analogRead(pinV) * 2 / 100.0;
    volt += vSensor1;
  }
  volt = (volt / numMuestras1);
  error = 0.25729*volt-2.301628;
  volt = volt - error;
 
  if (volt < 0 || analogRead(pinV) == 0) {
    volt = 0;
  }
  return volt;
}

double getTemp(int numMuestras1){
  //double RT = 0;
  //double R2 = 150e3;
  //double Vin = 3.3;
  double Vout = 0;
  double V = 0;
  double T = 0;
  int numMuestras2 = numMuestras1 + 5000;
  for (int k = 0; k < numMuestras2; k++) {
    Vout = analogRead(pinT) * 3.3 / 4095;
    V += Vout;
  }

  V = V / numMuestras2;
  //RT = R2 / ((Vin / V) - 1);
  //T = (1 / (a + b * log(RT) + c * pow(log(RT), 2) + d * pow(log(RT), 3))) - 273.15;
  T = 153*exp(-0.87*V);
  return T;
}

double getSignalAcceleration(int numMuestras1){
  double SignalAcc = 0 ;
  double VAcc = 0;
  for(int i = 0; i < numMuestras1; i++ ){
    VAcc = analogRead(pinACC)*3.3/4095;
    SignalAcc += VAcc;
  }
  SignalAcc = SignalAcc / numMuestras1;
  return SignalAcc;
}