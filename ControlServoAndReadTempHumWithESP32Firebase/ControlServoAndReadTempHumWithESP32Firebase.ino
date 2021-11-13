
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

/** This example will show how to authenticate using 
 * the legacy token or database secret with the new APIs (using config and auth data).
*/
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

// Library DHT
#include <DHT.h>
#define DHTTYPE DHT11
#define DHT_PIN 4
DHT dht(DHT_PIN, DHTTYPE); // Inisilisasi DHT dengan parameter DHT_PIN dan DHTTYPE

// Library Servo
#include <Servo.h>
Servo microServo;
static const int servoPin = 12;

// Sensor Cahaya
#define PIN_SENSOR_CAHAYA 36

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "RHEMA"
#define WIFI_PASSWORD "rhema878"

/* 2. If work with RTDB, define the RTDB URL and database secret */
#define DATABASE_URL "https://control-servo-based-on-temp-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "MIpS89V0ppguJQIthPvqXvdFq38G7EHDaXUTUdOn"

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long dataMillis = 0;

void setup()
{
    Serial.begin(115200);

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

    /* Assign the certificate file (optional) */
    //config.cert.file = "/cert.cer";
    //config.cert.file_storage = StorageType::FLASH;

    /* Assign the database URL and database secret(required) */
    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);

    //Or use legacy authenticate method
    //Firebase.begin(DATABASE_URL, DATABASE_SECRET);

    // DHT 11
    dht.begin();
    
    // Servo
    microServo.attach(servoPin);
   
}

void loop()
{
  String power;
  String on = "ON";
  if (millis() - dataMillis > 2000)
  {
      Firebase.getString(fbdo,"/FirebaseIoT/Power");
      power = fbdo.stringData();
      dataMillis = millis();
      sensorSuhu();
      Serial.print("Power: ");
      Serial.print(power);
      Serial.println();
      int analogValue = analogRead(PIN_SENSOR_CAHAYA);
      Serial.print("LDR: ");
      Serial.print(analogValue);
      Serial.println();
      if (Firebase.setFloat(fbdo, "/FirebaseIoT/LDR", analogValue))
      {
        Serial.print("PASSED");
        Serial.print(" || PATH: " + fbdo.dataPath());
        Serial.print(" || TYPE: " + fbdo.dataType());
        Serial.println();
      }
      else
      {
        Serial.print("FAILED");
        Serial.print(" || REASON: " + fbdo.errorReason());
        Serial.println();
      }
      if(analogValue == 0){
        microServo.write(0);
      } else {
        microServo.write(90);
      }
      delay(100);
  }   
}

void sensorSuhu()
{
    // Suhu dan Kelembaban (Humidity)
    float suhu = dht.readTemperature(); //memanggil function readTemperature untuk membaca suhu
    float humidity = dht.readHumidity(); //memanggil function readHumidity untuk membaca kelembaban
    if(isnan(suhu) || isnan(humidity)){
      Serial.println("Gagal membaca suhu dan kelembaban");
    }
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print("°C || ");
    
    Serial.print("Kelembaban: ");
    Serial.print(humidity);
    Serial.print("%");
    Serial.println();

    // Kirim Data Suhu dan Kelembaban (Humidity) ke Firebase
    if (Firebase.setFloat(fbdo, "/FirebaseIoT/Temperature", suhu))
    {
      Serial.print("PASSED");
      Serial.print(" || PATH: " + fbdo.dataPath());
      Serial.print(" || TYPE: " + fbdo.dataType());
      Serial.println();
    }
    else
    {
      Serial.print("FAILED");
      Serial.print(" || REASON: " + fbdo.errorReason());
      Serial.println();
    }
  
    if (Firebase.setFloat(fbdo, "/FirebaseIoT/Humidity", humidity))
    {
      Serial.print("PASSED");
      Serial.print(" || PATH: " + fbdo.dataPath());
      Serial.print(" || TYPE: " + fbdo.dataType());
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println(" || REASON: " + fbdo.errorReason());
      Serial.println();
    }
}
