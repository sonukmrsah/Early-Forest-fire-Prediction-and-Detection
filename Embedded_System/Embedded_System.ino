#include <TinyGPSPlus.h>
#include "DHT.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h> 
#include"addons/TokenHelper.h"
#include"addons/RTDBHelper.h"

#define WIFI_SSID "Goldspear"
#define WIFI_PASSWORD "Sonu@772"
#define API_KEY "AIzaSyDX7pIGxpoUuibRxuzaCkz2qJEGBzaqLvs"
#define DB_URL "https://forestfire-7e17e-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;  
FirebaseConfig config;

// Dependencies variables
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

#define AO_PIN 35
#define DHTPIN 14
#define DHTTYPE DHT11
#define smoke 27

TinyGPSPlus gps;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(3000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DB_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  pinMode(AO_PIN,INPUT);
  dht.begin();
}

void DHT_11(){
  delay(2000);
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", h)){
    Serial.println("PASSED");
    Serial.println("PATH" + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Temperature", t)){
    Serial.println("PASSED");
    Serial.println("PATH" + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void flame(){
  int infrared_value = analogRead(AO_PIN);
  infrared_value = map(infrared_value, 0, 4095, 0, 100);
  Serial.print("The AO value: ");
  Serial.println(infrared_value);

  if (Firebase.RTDB.setFloat(&fbdo, "Flame_Sensor/Infrared_Value", infrared_value)){
    Serial.println("PASSED");
    Serial.println("PATH" + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void Smoke() {
  int value = analogRead(smoke);
  // value = map(value, 0, 4095, 0, 100);
  Serial.print("Smoke: ");
  Serial.println(value);
  if (Firebase.RTDB.setFloat(&fbdo, "Smoke_Sensor/Smoke_value", value)){
    Serial.println("PASSED");
    Serial.println("PATH" + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}


void loop() {
  flame();
  DHT_11();
  Smoke();
  // GPS();
}
