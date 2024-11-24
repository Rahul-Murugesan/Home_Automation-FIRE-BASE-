#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Include token generation and helper functions
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// WiFi Credentials
#define WIFI_SSID "R"
#define WIFI_PASSWORD "12"

// Firebase Configuration
#define API_KEY ""
#define DATABASE_URL ""

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variables for Firebase
bool signupOK = false;

// Relay Pin Configuration
#define RELAY_PIN1 4 // Pin for Relay 1
#define RELAY_PIN2 5 // Pin for Relay 2 (Adjust as needed)

// WiFi Connection Function
void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi");
}

// Setup Function
void setup() {
  Serial.begin(115200);

  // Initialize Relay Pins
  pinMode(RELAY_PIN1, OUTPUT);
  digitalWrite(RELAY_PIN1, HIGH); // Ensure Relay 1 is off initially
  pinMode(RELAY_PIN2, OUTPUT);
  digitalWrite(RELAY_PIN2, HIGH); // Ensure Relay 2 is off initially

  // Connect to WiFi
  connectToWiFi();

  // Firebase Initialization
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Firebase sign-up (optional for anonymous auth)
  if (Firebase.signUp(&config, &auth, "", "")) {
    signupOK = true;
    Serial.println("Firebase Sign-Up Successful");
  } else {
    Serial.printf("Sign-Up Error: %s\n", config.signer.signupError.message.c_str());
  }

  // Begin Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK) {
    // Fetch data for Relay 1
    String databasePath1 = "Weather_Monitor/room1/L1";
    if (Firebase.RTDB.getString(&fbdo, databasePath1)) {
      if (fbdo.dataType() == "string") {
        String stream_data1 = fbdo.stringData();

        // Remove unwanted characters
        stream_data1.replace("\\", ""); 
        stream_data1.replace("\"", "");

        Serial.println("Data fetched successfully for Relay 1:");
        Serial.println("relay1: " + stream_data1);

        // Convert stream_data to integer
        int stateRelay1 = stream_data1.toInt();

        // Control Relay 1
        if (stateRelay1 == 1) {
          digitalWrite(RELAY_PIN1, LOW); // Turn Relay 1 ON
          Serial.println("Relay 1 OFF");
        } else if (stateRelay1 == 0) {
          digitalWrite(RELAY_PIN1, HIGH); // Turn Relay 1 OFF
          Serial.println("Relay 1 ON");
        } else {
          Serial.println("Unexpected data value for Relay 1");
        }
      } else {
        Serial.println("Unexpected data type for Relay 1");
      }
    } else {
      Serial.printf("Firebase Get Error for Relay 1: %s\n", fbdo.errorReason().c_str());
    }

    // Fetch data for Relay 2
    String databasePath2 = "Weather_Monitor/room1/L2";
    if (Firebase.RTDB.getString(&fbdo, databasePath2)) {
      if (fbdo.dataType() == "string") {
        String stream_data2 = fbdo.stringData();

        // Remove unwanted characters
        stream_data2.replace("\\", ""); 
        stream_data2.replace("\"", "");

        Serial.println("Data fetched successfully for Relay 2:");
        Serial.println("relay2: " + stream_data2);

        // Convert stream_data to integer
        int stateRelay2 = stream_data2.toInt();

        // Control Relay 2
        if (stateRelay2 == 1) {
          digitalWrite(RELAY_PIN2, LOW); // Turn Relay 2 ON
          Serial.println("Relay 2 OFF");
        } else if (stateRelay2 == 0) {
          digitalWrite(RELAY_PIN2, HIGH); // Turn Relay 2 OFF
          Serial.println("Relay 2 ON");
        } else {
          Serial.println("Unexpected data value for Relay 2");
        }
      } else {
        Serial.println("Unexpected data type for Relay 2");
      }
    } else {
      Serial.printf("Firebase Get Error for Relay 2: %s\n", fbdo.errorReason().c_str());
    }
  }

  delay(2000); // Fetch data every 2 seconds
}
