#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

// WLAN-Einstellungen
const char* ssid = "WLAN-782577";       // Hier deinen WLAN-Namen eintragen
const char* password = "60025416322051135176"; // Hier dein WLAN-Passwort eintragen

// Sensor-Pin
const int lightSensorPin = A0;    // Lichtsensor am analogen Pin A0

// DHT11-Einstellungen
#define DHT_TYPE DHT11 // DHT-Sensor DHT11
#define DHT_PIN 14 // GPIO14 als Datenpin des DHT-Sensors
#define DHT_POWER 16 // GPIO16 als Spannungsversorgung für den DHT-Sensor

DHT dht(DHT_PIN, DHT_TYPE); // DHT-Sensor erstellen

const long interval = 2000; // 2000ms = 2sec. Intervall der Datenerfassung

#define OUTPUT_PIN 16 // GPIO16 als Output-Pin

// Eigene Hash-Funktion zur Generierung eines 6-stelligen Codes aus der MAC-Adresse
uint32_t customHash(String macAddress) {
    uint32_t hash = 0;
    for (size_t i = 0; i < macAddress.length(); i++) {
        hash = (hash * 31) + macAddress[i]; // Einfache Hash-Berechnung
    }
    return hash % 1000000; // Auf 6-stellige Zahl begrenzen
}

String generateDeviceCode(String macAddress) {
    uint32_t code = customHash(macAddress); // Verwende die eigene Hash-Funktion
    return String(code);
}

void setup() {
    Serial.begin(115200);
    
    // Mit WLAN verbinden (Stationsmodus)
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    Serial.print("Verbinde mit WLAN");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nVerbunden mit WLAN!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());

    // MAC-Adresse abrufen
    String macAddress = WiFi.macAddress();

    // Einmaligen Code generieren
    String deviceCode = generateDeviceCode(macAddress);

    // Code ausgeben
    Serial.print("MAC-Adresse: ");
    Serial.println(macAddress);
    Serial.print("Geraete-Code: ");
    Serial.println(deviceCode);

    pinMode(DHT_POWER, OUTPUT); // Spannungsversorgung für DHT-Baustein
    digitalWrite(DHT_POWER, HIGH); // GPIO16 dauerhaft auf HIGH setzen

    dht.begin(); // Initialisierung des DHT-Sensors

    Serial.begin(115200);
    delay(500);
    Serial.println("\nDHT running...\n");

    pinMode(OUTPUT_PIN, OUTPUT); // GPIO16 als Ausgang setzen
    digitalWrite(OUTPUT_PIN, HIGH); // GPIO16 dauerhaft auf HIGH setzen
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        // Lichtwert vom Sensor lesen
        int lightLevel = analogRead(lightSensorPin);
        
        // Lichtwert ausgeben
        Serial.print("Helligkeit: "); 
        Serial.println(lightLevel);

        float h = dht.readHumidity(); // Lese Luftfeuchtigkeit
        float t = dht.readTemperature(); // Lese Temperatur in °C
        float f = dht.readTemperature(true); // Lese Temperatur in °F (Fahrenheit = true)

        if (isnan(h) || isnan(t) || isnan(f)) {
            Serial.println("Fehler beim Lesen des DHT-Sensors");
        } else {
            Serial.println("Luftfeuchtigkeit: " + String(h) + " %");
            Serial.println("Temperatur: " + String(t) + " °C");
            Serial.println("Temperatur: " + String(f) + " °F");
            Serial.println();
        }

        // HTTP-Client und WiFiClient erstellen
        WiFiClient client;
        HTTPClient http;
        
        // Verbindung zum Server herstellen
        http.begin(client, "http://192.168.2.194/Repos/Arduino-Wetterstation/esp_server/sensor_data.php");
        
        // Header für POST-Anfrage setzen
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        // POST-Anfrage mit Lichtwert, Temperatur und Luftfeuchtigkeit vorbereiten
        String macAddress = WiFi.macAddress();
        String deviceCode = generateDeviceCode(macAddress);
        String postData = "light_level=" + String(lightLevel) + "&temperature=" + String(t) + "&humidity=" + String(h) + "&device_code=" + deviceCode;
        
        // Send POST request
        int httpResponseCode = http.POST(postData);
        if (httpResponseCode > 0) {
            Serial.print("HTTP Response Code: ");
            Serial.println(httpResponseCode);
            String response = http.getString();
            Serial.println("Antwort vom Server: " + response);
        } else {
            Serial.print("Fehler bei der HTTP-Anfrage: ");
            Serial.println(httpResponseCode);
        }
        
        http.end(); // Verbindung schließen
    } else {
        Serial.println("WLAN-Verbindung verloren");
    }
    
    delay(interval); // Wartezeit zwischen den Messungen
}