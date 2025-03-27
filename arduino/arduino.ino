#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_SSD1306.h>

// WLAN-Einstellungen
const char* ssid = "Your_SSID";       // Hier deinen WLAN-Namen eintragen
const char* password = "Your_PASSWORD"; // Hier dein WLAN-Passwort eintragen

// Sensor-Pin
const int lightSensorPin = A0;    // Lichtsensor am analogen Pin A0

// Ersetzen der festen ESP-ID durch die MAC-Adresse
String espId = WiFi.macAddress(); // MAC-Adresse als eindeutige ID verwenden

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LED-Pins für Temperaturanzeige
#define LED_LOW 12    // LED für niedrige Temperatur - GPIO12
#define LED_MEDIUM 13 // LED für mittlere Temperatur - GPIO13
#define LED_HIGH 15   // LED für hohe Temperatur - GPIO15

// Temperatur-Schwellenwerte für LED-Anzeige
#define TEMP_LOW 15.0    // Unter diesem Wert leuchtet nur die niedrige Temperatur-LED
#define TEMP_MEDIUM 25.0 // Über diesem Wert leuchtet die hohe Temperatur-LED

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

// Funktion zur LED-Steuerung basierend auf der Temperatur
void updateTemperatureLEDs(float temperature) {
    // Alle LEDs zuerst ausschalten
    digitalWrite(LED_LOW, LOW);
    digitalWrite(LED_MEDIUM, LOW);
    digitalWrite(LED_HIGH, LOW);

    // Je nach Temperatur die entsprechende LED einschalten
    if (temperature < TEMP_LOW) {
        digitalWrite(LED_LOW, HIGH);
    } else if (temperature >= TEMP_LOW && temperature < TEMP_MEDIUM) {
        digitalWrite(LED_MEDIUM, HIGH);
    } else {
        digitalWrite(LED_HIGH, HIGH);
    }
}

// Funktion zur Anzeige von Wetterdaten auf dem OLED-Display
void showWeatherData(float temperature, float humidity, int lightLevel) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("Wetterdaten:");
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");
    display.print("Luft: ");
    display.print(humidity);
    display.println(" %");
    display.print("Licht: ");
    display.println(lightLevel);

    display.display();
}