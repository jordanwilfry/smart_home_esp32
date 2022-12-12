#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// temperature sensor setting start
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4

#define DHTTYPE DHT11


DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

// temperature sensor setting end

/* 1. Define the WiFi credentials */
#define WIFI_SSID "jordan"
#define WIFI_PASSWORD "naoussi001"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyBTxzDByhFhyFfwAzYcyfbX475juyn8JWs"

/* 3. Define the RTDB URL */
#define DATABASE_URL "smart-home-15df3-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "jordanwilfry02@gmail.com"
#define USER_PASSWORD "0123456789"


// defining led PIN
#define LIVING_ROOM_PIN 16
#define FAMILY_ROOM_PIN 17
#define KITCHEN_PIN 18
#define BATH_ROOM_PIN 19
#define GARAGE_PIN 21
#define BED_ROOM_PIN 22


// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

void setup()
{

  Serial.begin(115200);
  pinMode(LIVING_ROOM_PIN, OUTPUT);
  pinMode(BED_ROOM_PIN, OUTPUT);
  pinMode(KITCHEN_PIN, OUTPUT);
  pinMode(GARAGE_PIN, OUTPUT);
  pinMode(BATH_ROOM_PIN, OUTPUT);
  pinMode(FAMILY_ROOM_PIN, OUTPUT);

  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h


#if defined(ESP8266)
  // In ESP8266 required for BearSSL rx/tx buffer for large data handle, increase Rx size as needed.
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;



  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;


}

void loop()
{

  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    Serial.printf("Get bulbs ga... %s\n", Firebase.RTDB.getString(&fbdo, F("/Bulbs/Garage")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.printf("Get bulbs li... %s\n", Firebase.RTDB.getString(&fbdo, F("/Bulbs/Living room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.printf("Get bulbs be... %s\n", Firebase.RTDB.getString(&fbdo, F("/Bulbs/Bed room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.printf("Get bulbs ba... %s\n", Firebase.RTDB.getString(&fbdo, F("/Bulbs/Bath room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.printf("Get bulbs fa... %s\n", Firebase.RTDB.getString(&fbdo, F("/Bulbs/Family room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.printf("Get bulbs ki... %s\n", Firebase.RTDB.getString(&fbdo, F("/Bulbs/Kitchen")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());

    String garage_buld = Firebase.RTDB.getString(&fbdo, F("/Bulbs/Garage")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    String living_room_bulb = Firebase.RTDB.getString(&fbdo, F("/Bulbs/Living room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    String bed_room_bulb = Firebase.RTDB.getString(&fbdo, F("/Bulbs/Bed room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    String bath_room_bulb = Firebase.RTDB.getString(&fbdo, F("/Bulbs/Bath room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    String family_room_buld = Firebase.RTDB.getString(&fbdo, F("/Bulbs/Family room")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    String kitchen_buld = Firebase.RTDB.getString(&fbdo, F("/Bulbs/Kitchen")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    
    // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino
    if(garage_buld == "true"){
      digitalWrite(GARAGE_PIN, HIGH);
    }else{
      digitalWrite(GARAGE_PIN, LOW);
    }

    if(living_room_bulb == "true"){
      digitalWrite(LIVING_ROOM_PIN, HIGH);
    }else{
      digitalWrite(LIVING_ROOM_PIN, LOW);
    }

    if(kitchen_buld == "true"){
      digitalWrite(KITCHEN_PIN, HIGH);
    }else{
      digitalWrite(KITCHEN_PIN, LOW);
    }

    if(bed_room_bulb == "true"){
      digitalWrite(BED_ROOM_PIN, HIGH);
    }else{
      digitalWrite(BED_ROOM_PIN, LOW);
    }

    if(bath_room_bulb == "true"){
      digitalWrite(BATH_ROOM_PIN, HIGH);
    }else{
      digitalWrite(BATH_ROOM_PIN, LOW);
    }

    if(family_room_buld == "true"){
      digitalWrite(FAMILY_ROOM_PIN, HIGH);
    }else{
      digitalWrite(FAMILY_ROOM_PIN, LOW);
    }



    // FirebaseJson json;

    // if (count == 0)
    // {
    //   // json.set("value/round/"+String(count), F("cool!"));
    //   json.set(F("value/ts/.sv"), F("timestamp"));
    //   Serial.printf("Set json... %s\n", Firebase.RTDB.set(&fbdo, F("/test/json"), &json) ? "ok" : fbdo.errorReason().c_str());
    // }
    // else
    // {
    //   json.add(String(count), F("smart!"));
    //   Serial.printf("Update node... %s\n", Firebase.RTDB.updateNode(&fbdo, F("/test/json/value/round"), &json) ? "ok" : fbdo.errorReason().c_str());
    // }

    // Serial.println();
    // count++;




  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    Serial.printf("settinf Temperature... %s\n", Firebase.RTDB.setFloat(&fbdo, F("/house/temperature"), event.temperature) ? "ok" : fbdo.errorReason().c_str());

  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    Serial.printf("setting Humidity... %s\n", Firebase.RTDB.setFloat(&fbdo, F("/house/humidity"), event.relative_humidity) ? "ok" : fbdo.errorReason().c_str());
  }
  }
}
