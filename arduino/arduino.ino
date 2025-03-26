#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WLAN-Einstellungen
const char* ssid = "WLAN-782577";       // Hier deinen WLAN-Namen eintragen
const char* password = "60025416322051135176"; // Hier dein WLAN-Passwort eintragen

// Sensor-Pin
const int lightSensorPin = A0;    // Lichtsensor am analogen Pin A0

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
        http.begin(client, "http://192.168.2.194/Repos/Arduino-Wetterstation/esp_server/sensor_data.php");
        
        // Header für POST-Anfrage setzen
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        // POST-Anfrage mit Lichtwert und Geräte-Code vorbereiten
        String macAddress = WiFi.macAddress();
        String deviceCode = generateDeviceCode(macAddress);
        String postData = "light_level=" + String(lightLevel) + "&device_code=" + deviceCode;
        
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
    
    delay(10000); // 10 Sekunden warten bis zur nächsten Messung
}