#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WLAN-Einstellungen
const char* ssid = "Your_SSID";       // Hier deinen WLAN-Namen eintragen
const char* password = "Your_PASSWORD"; // Hier dein WLAN-Passwort eintragen

// Sensor-Pin
const int lightSensorPin = A0;    // Lichtsensor am analogen Pin A0

// Ersetzen der festen ESP-ID durch die MAC-Adresse
String espId = WiFi.macAddress(); // MAC-Adresse als eindeutige ID verwenden

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
    
    // MAC-Adresse ausgeben
    Serial.print("ESP-ID (MAC-Adresse): ");
    Serial.println(espId);
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        // Lichtwert vom Sensor lesen
        int lightLevel = analogRead(lightSensorPin);
        
        // Lichtwert ausgeben
        Serial.print("Helligkeit: "); 
        Serial.println(lightLevel);
        
        // HTTP-Client und WiFiClient erstellen
        WiFiClient client;
        HTTPClient http;
        
        // Verbindung zum Server herstellen
        http.begin(client, "http://3r6nuss.de:7456/esp_server/sensor_data.php");
        
        // Header für POST-Anfrage setzen
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        // POST-Anfrage mit Lichtwert und MAC-Adresse vorbereiten
        String postData = "light_level=" + String(lightLevel) + "&esp_id=" + espId;
        
        // Anfrage senden
        Serial.println("Sende Daten an Server...");
        int httpResponseCode = http.POST(postData);
        
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("HTTP Response Code: " + String(httpResponseCode));
            Serial.println("Antwort vom Server: " + response);
        } else {
            Serial.print("Fehler bei HTTP-Anfrage: ");
            Serial.println(httpResponseCode);
        }
        
        http.end(); // Verbindung schließen
    } else {
        Serial.println("WLAN-Verbindung verloren");
    }
    
    delay(10000); // 10 Sekunden warten bis zur nächsten Messung
}