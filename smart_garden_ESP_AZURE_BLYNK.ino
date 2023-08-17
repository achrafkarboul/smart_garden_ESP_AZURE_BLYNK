/* ------------------------------------------------------------------------------------------------------------
  déclaration des bibliothèques : 
1/ WiFi
2/ MySQL
3/ DHT
4/ BlynkSimpleEsp32
--------------------------------------------------------------------------------------------------------------*/
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "src/iotc/common/string_buffer.h"
#include "src/iotc/iotc.h"

/* ------------------------------------------------------------------------------------------------------------
  déclaration des varibles
1/ DHT
2/ SOIL
3/ Lumière
4/ électrovanne
--------------------------------------------------------------------------------------------------------------*/
#define DHTPIN D5
#define DHTTYPE DHT11
#define SOILPIN A0
#define RELAY_PIN D4
#define light D2
#define BLYNK_AUTH_TOKEN "sfW3L3vcIwVrxQH392bu89PziXJldM6O"
WidgetLED led(V0);

/* ------------------------------------------------------------------------------------------------------------
  intitialiser les varibles de
1/ la connexion BLYNK
2/ la connexion AZURE
3/ la valeur d'humidité 
--------------------------------------------------------------------------------------------------------------*/

DHT dht(DHTPIN, DHTTYPE);
const char* ssid = "Airbox-343A";
const char* password = "EJJBHJmJ";
char auth_blynk[] = BLYNK_AUTH_TOKEN;

const char* SCOPE_ID = "0ne00A25F44";
const char* DEVICE_ID = "smartgarden";
const char* DEVICE_KEY = "1fEd4v8eTMMsEvCocEAA2JZZL4yz3DSPLvG9TEPyzOY=";

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "src/connection.h"
void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo) {
  // ConnectionStatus
  if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
    LOG_VERBOSE("Is connected ? %s (%d)",
                callbackInfo->statusCode == IOTC_CONNECTION_OK ? "YES" : "NO",
                callbackInfo->statusCode);
    isConnected = callbackInfo->statusCode == IOTC_CONNECTION_OK;
    return;
  }

  AzureIOT::StringBuffer buffer;
  if (callbackInfo->payloadLength > 0) {
    buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
  }

  LOG_VERBOSE("- [%s] event was received. Payload => %s\n",
              callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
  }
}

/* ------------------------------------------------------------------------------------------------------------
  setUp
1/ debuter la liaison avec le DHTPIN
2/ debuter la liaison avec SOILPIN
3/ intialiser la valeur d'humidité 
4/ activer la connexion Wifi et blynk
5/ se connecter à azure
--------------------------------------------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  pinMode(SOILPIN, INPUT);
  led.off();
  pinMode(RELAY_PIN, OUTPUT);
   digitalWrite(RELAY_PIN, LOW);
  led.off();
  delay(5000);
  Blynk.begin(auth_blynk, ssid, password);
  connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);
  if (context != NULL) {
    lastTick = 0;  // set timer in the past to enable first telemetry a.s.a.p
  }
}

void loop() {
  /* ------------------------------------------------------------------------------------------------------------
  récupérer les données reçues par les capteurs:
1/ d'humidité
2/ de température
3/ de la lumiere embiante 
4/ d'humidité du SOL
--------------------------------------------------------------------------------------------------------------*/
  double humidity = dht.readHumidity();
  double temperature = dht.readTemperature();
  double lightLevel = digitalRead(light);
  double soilMoisture = analogRead(SOILPIN);
     /*humidity = 95;
   temperature = 31;
   lightLevel = 1002;
  soilMoisture = 15;*/
  /* ------------------------------------------------------------------------------------------------------------
  exploitation de données prises par les capteurs: 
1/ calculer le stress temp 
2/ calculer le point de rosée
3/ calculer l'évapotranspiration
--------------------------------------------------------------------------------------------------------------*/
  double stressIndex = (1.8 * temperature + 32) -( (0.55 - 0.0055 * humidity) * (1.8 * temperature - 26));
 
  double dewPoint = temperature - ((100 - humidity) / 5);
 
  //    double etp = coeffTemp * temperature + coeffHumidite * humidite + coeffVent * vent + coeffRayonnement * rayonnement;
  /* ------------------------------------------------------------------------------------------------------------
    mise à jour de la base de donnée :
1/ insertion des nouvelles valeurs
2/ tester la reception 
3/ envoye d'erreur en cas d'erreur
4/ reconnexion
--------------------------------------------------------------------------------------------------------------*/
  if (isConnected) {

    unsigned long ms = millis();
    if (ms - lastTick > 10000) {  // send telemetry every 10 seconds
      char msg[256] = { 0 };
      int pos = 0, errorCode = 0;

      lastTick = ms;
      if (loopId++ % 2 == 0) {  // send telemetry
        pos = snprintf(msg, sizeof(msg) - 1, "{\"tempRature\": %f, \"humidit\": %f, \"soilhum\": %f, \"lumiere\": %f}", temperature, humidity, soilMoisture, lightLevel);
        errorCode = iotc_send_telemetry(context, msg, pos);


      } else {  // send property
      }

      msg[pos] = 0;

      if (errorCode != 0) {
        LOG_ERROR("Sending message has failed with error code %d", errorCode);
      }
    }
if (stressIndex > 80) {
  digitalWrite(RELAY_PIN, HIGH);
  led.on();
} else {
  digitalWrite(RELAY_PIN, LOW);
  led.off();
}
/* ------------------------------------------------------------------------------------------------------------
  Envoyer les données à  Blynk
--------------------------------------------------------------------------------------------------------------*/
Blynk.virtualWrite(V1, humidity);
Blynk.virtualWrite(V2, temperature);
Blynk.virtualWrite(V3, stressIndex);
Blynk.virtualWrite(V4, dewPoint);
// Blynk.virtualWrite(V5, evapotranspiration);
Blynk.virtualWrite(V7, lightLevel);
Blynk.virtualWrite(V6, soilMoisture);
    iotc_do_work(context);  // do background work for iotc
  }
   else {
    iotc_free_context(context);
    context = NULL;
    connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);
  }
/* ------------------------------------------------------------------------------------------------------------
  Prendre des mesures d'arrosage en fonction des conditions calculées
1/ Activer l'arrosage par électrovanne
2/  Désactiver l'arrosage 
--------------------------------------------------------------------------------------------------------------*/


}


