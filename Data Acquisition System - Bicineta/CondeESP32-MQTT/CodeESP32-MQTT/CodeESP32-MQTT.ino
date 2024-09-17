#include "SD.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <math.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#define RXD2 16 // Pin RX GPS
#define TXD2 17 // Pin TX GPS
#define GPS_BAUD 9600  // Velocidad de baudios del módulo GPS
#define NMEA 0

//---- WiFi settings
const char* ssid = "UTP";
const char* password = "tecnologica";
//---- MQTT Broker settings
const char* mqtt_server = "02c9970343eb456bac87e5836a2905c3.s1.eu.hivemq.cloud"; // replace with your broker url
const char* mqtt_username = "GCA_DAQ";
const char* mqtt_password = "#Control2005";
const int mqtt_port =8883;

WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

const char* sensor1_topic= "sensor1";
const char* sensor2_topic= "sensor2";

static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// =============================================================

int pinA = 26; // Pin sensor efecto hall
int pinB = 27; // Pin sensor efecto hall analogo (No en uso)

int pinV = 33;  //Pin medir tension PM
int pinI = 32; // Pin medir corriente ACS712
int pin_I = 35; // Pin medir corriente PM (No en uso)
int pinT = 34; // Pin medir temperatura T-D500

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

  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());

  while (!Serial) delay(1);

  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

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

    if (!client.connected()) reconnect();
  client.loop();


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
        
        publishMessage(sensor1_topic,String(voltaje),true);    
        publishMessage(sensor2_topic,String(corriente),true);
        delay(1000);
      
}

void reconnect() {
  // Loop until we’re reconnected
  while (!client.connected()) {
  Serial.print("Attempting MQTT connection…");
  String clientId = "ESP8266Client-"; // Create a random client ID
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("connected");

    client.subscribe(sensor1_topic);   // subscribe the topics here
    //client.subscribe(command2_topic);   // subscribe the topics here
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
    delay(5000);
  }
  }
}

//=======================================
// This void is called every time we have a message from the broker

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
  // check for other commands
  /* else if( strcmp(topic,command2_topic) == 0){
  if (incommingMessage.equals(“1”)) { } // do something else
  }
  */
}

//======================================= publising as string
void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
  Serial.println("Message published ["+String(topic)+"]: "+payload);
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