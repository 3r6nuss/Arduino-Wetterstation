#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "DHT.h"

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WLAN-Einstellungen
const char* ssid = "WLAN-782577";       // Hier deinen WLAN-Namen eintragen
const char* password = "60025416322051135176"; // Hier dein WLAN-Passwort eintragen

// Webserver für Redirect
ESP8266WebServer server(80);

// Sensor-Pin
const int lightSensorPin = A0;    // Lichtsensor am analogen Pin A0

// DHT11-Einstellungen
#define DHT_TYPE DHT11 // DHT-Sensor DHT11
#define DHT_PIN 14 // GPIO14 als Datenpin des DHT-Sensors
#define DHT_POWER 16 // GPIO16 als Spannungsversorgung für den DHT-Sensor

// LED-Pins für Temperaturanzeige
#define LED_LOW 12    // LED für niedrige Temperatur - GPIO12
#define LED_MEDIUM 13 // LED für mittlere Temperatur - GPIO13
#define LED_HIGH 15   // LED für hohe Temperatur - GPIO15

// Temperatur-Schwellenwerte für LED-Anzeige
#define TEMP_LOW 15.0    // Unter diesem Wert leuchtet nur die niedrige Temperatur-LED
#define TEMP_MEDIUM 25.0 // Über diesem Wert leuchtet die hohe Temperatur-LED

DHT dht(DHT_PIN, DHT_TYPE); // DHT-Sensor erstellen

const long interval = 30000; // 30000ms = 30sec. Intervall der Datenerfassung
const int display_timeout = 20000; // 20 Sekunden für jede Anzeige

// Bitmap meines Gesichtes
const unsigned char epd_bitmap_bild [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0xc1, 0x80, 0x03, 0xc0, 0x7f, 0xff, 0xc0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xf7, 0xff, 0x01, 0x80, 0x13, 0xc0, 0x7f, 0xff, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xf7, 0xfe, 0x01, 0x00, 0x17, 0xe0, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xe7, 0xfc, 0x00, 0x00, 0x27, 0xe1, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0xcf, 0xfc, 0x10, 0x00, 0x2f, 0xc0, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x03, 0xe0, 0x00, 
	0x07, 0xcf, 0xfe, 0x00, 0x00, 0x4f, 0xc2, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x07, 
	0x8f, 0xff, 0x80, 0x00, 0x1e, 0x03, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x07, 0x9f, 
	0xff, 0x80, 0x00, 0x18, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x03, 0x1f, 0xff, 
	0xc0, 0x00, 0x08, 0x07, 0xff, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x03, 0x3f, 0xff, 0xc0, 
	0x01, 0x00, 0x0f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x01, 0x3f, 0xff, 0x80, 0x01, 
	0x00, 0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x02, 0x00, 
	0x1f, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x3f, 
	0xff, 0xfc, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x04, 0x08, 0x3f, 0xff, 
	0xf8, 0x00, 0x00, 0x03, 0x7f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xf8, 
	0x00, 0x00, 0x07, 0xff, 0xff, 0xc0, 0x00, 0x7f, 0xff, 0x00, 0x08, 0x00, 0x7f, 0xff, 0xf0, 0x80, 
	0x00, 0x06, 0xbf, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xff, 0x71, 0x00, 0x00, 
	0x00, 0xbf, 0xfd, 0xfc, 0x00, 0x3f, 0xff, 0x80, 0x10, 0x00, 0xff, 0xff, 0xe1, 0xc0, 0x00, 0x00, 
	0x3f, 0xfc, 0x7e, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0xff, 0xfe, 0xe6, 0x40, 0x00, 0x00, 0x3b, 
	0xf0, 0x1c, 0x00, 0x3f, 0xff, 0x00, 0x20, 0x01, 0xff, 0xfe, 0xc6, 0xc0, 0x00, 0x00, 0x1b, 0xf0, 
	0x00, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xfd, 0xd3, 0xc0, 0x00, 0x00, 0x1d, 0xf0, 0x00, 
	0x00, 0x0f, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xfd, 0x81, 0xc0, 0x00, 0x00, 0x0d, 0xe0, 0x00, 0x00, 
	0x07, 0xfe, 0x00, 0x00, 0x03, 0xff, 0xfb, 0x80, 0xc0, 0x00, 0x00, 0x0d, 0xe0, 0x00, 0x00, 0x07, 
	0xfc, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0xc0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xfc, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0xc0, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x03, 0xf8, 0x00, 
	0x00, 0x0e, 0x03, 0xfe, 0x00, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 
	0x0f, 0x81, 0xfe, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfe, 0x00, 0x00, 
	0x00, 0xfc, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xf8, 0x00, 0x00, 
	0x7c, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xef, 0xe0, 0x00, 0x1c, 
	0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe4, 0xfe, 0x00, 0x18, 0x01, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x7f, 0x00, 0x18, 0x00, 0xc0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf7, 0xff, 0xc0, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf7, 0xff, 0xc0, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x80, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xb7, 0xbf, 0x81, 0xff, 0x80, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0x77, 0x3f, 0x81, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0x26, 0x3f, 0xc7, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3f, 0x26, 0x3f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x3f, 
	0x66, 0x3f, 0xff, 0xff, 0x80, 0x01, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3f, 0xa7, 
	0x3f, 0xff, 0xff, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x3f, 0xef, 0x3f, 
	0xff, 0xff, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1f, 0xef, 0x7f, 0xff, 
	0xff, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1f, 0xef, 0xff, 0x83, 0xff, 
	0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0xef, 0xff, 0x83, 0xff, 0x00, 
	0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0xef, 0xff, 0x01, 0xff, 0x00, 0x01, 
	0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0xff, 0xff, 0x01, 0xff, 0x00, 0x01, 0xf0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0xff, 0x01, 0xfe, 0x00, 0x01, 0xf8, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0xff, 0xff, 0x81, 0xfe, 0x00, 0x01, 0x83, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xff, 0xff, 0x83, 0xfc, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x80, 0x7f, 0xff, 0x8b, 0xfc, 0x00, 0x03, 0xc2, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x40, 0x3f, 0xff, 0x9f, 0xf8, 0x00, 0x03, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x20, 0x0f, 0xff, 0xbf, 0xf0, 0x00, 0x03, 0x87, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xff, 0xf0, 0x00, 0x03, 0x87, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x03, 0x43, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7c, 0x00, 0x00, 0x02, 0x42, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Optimierte Hash-Funktion zur Generierung eines 6-stelligen Codes aus der MAC-Adresse
uint32_t customHash(String macAddress) {
    uint32_t hash = 0;
    for (char c : macAddress) {
        hash = (hash * 31) + c; // Einfachere Hash-Berechnung mit range-based for loop
    }
    return hash % 1000000; // Auf 6-stellige Zahl begrenzen
}

String generateDeviceCode(String macAddress) {
    uint32_t code = customHash(macAddress); // Verwende die eigene Hash-Funktion
    return String(code);
}

// Variablen für die Display-Status-Steuerung
enum DisplayState {
  FACE,
  WIFI_INFO,
  WEATHER_DATA,
  DONE
};

DisplayState currentState = FACE;
unsigned long stateStartTime = 0;
boolean displaySequenceCompleted = false;

void setup() {
    Serial.begin(115200);
    
    // OLED-Display initialisieren
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    
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

    // LED-Pins initialisieren
    pinMode(LED_LOW, OUTPUT);
    pinMode(LED_MEDIUM, OUTPUT);
    pinMode(LED_HIGH, OUTPUT);
    
    // Alle LEDs zunächst ausschalten
    digitalWrite(LED_LOW, LOW);
    digitalWrite(LED_MEDIUM, LOW);
    digitalWrite(LED_HIGH, LOW);

    pinMode(DHT_POWER, OUTPUT); // Spannungsversorgung für DHT-Baustein
    digitalWrite(DHT_POWER, HIGH); // GPIO16 dauerhaft auf HIGH setzen

    dht.begin(); // Initialisierung des DHT-Sensors

    // Webserver starten und Route für Redirect einrichten
    server.on("/", handleRedirect);
    server.begin();
    Serial.println("Webserver gestartet - Redirect zu http://3r6nuss.de:7456/index.php aktiv");

    // Erstes Display-State setzen und Timer starten
    stateStartTime = millis();
    showFace(); // Zuerst das Gesicht anzeigen
}

void loop() {
    unsigned long currentTime = millis();
    
    // Einfachere Statusüberprüfung mit Timeout
    if (!displaySequenceCompleted && currentTime - stateStartTime >= display_timeout) {
        // Fortschreiten in der Anzeigesequenz
        switch (currentState) {
            case FACE:
                currentState = WIFI_INFO;
                showWifiInfo();
                break;
            case WIFI_INFO:
                currentState = WEATHER_DATA;
                showWeatherData();
                break;
            case WEATHER_DATA:
                currentState = DONE;
                displaySequenceCompleted = true;
                Serial.println("Anzeigesequenz abgeschlossen.");
                break;
        }
        stateStartTime = currentTime;
    }
    
    // Regelmäßige Datenaktualisierung im WEATHER_DATA oder DONE Status
    if ((currentState == WEATHER_DATA || currentState == DONE) && 
        (currentTime % interval < 100)) { // Überprüft ob wir im interval-Zeitpunkt sind
        updateAndSendData();
        if (currentState == DONE) {
            showWeatherData();
        }
    }
    
    // Webserver-Client bearbeiten (wichtig für Redirect-Funktionalität)
    server.handleClient();
    
    delay(100);
}

// Zeigt das Gesicht auf dem OLED-Display an
void showFace() {
    display.clearDisplay();
    display.drawBitmap(0, 0, epd_bitmap_bild, 116, 64, SSD1306_WHITE);
    display.display();
    Serial.println("Zeige Gesicht für 20 Sekunden");
}

// Zeigt WLAN-Informationen auf dem Display an
void showWifiInfo() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    
    // Kompaktere Darstellung mit weniger Leerzeilen
    display.println("WLAN-Info:");
    display.print("SSID: ");
    display.println(ssid);
    display.print("IP: ");
    display.println(WiFi.localIP());
    
    // Gerätecode direkt ohne MAC-Adresse zwischenzuspeichern
    display.print("Code: ");
    display.println(generateDeviceCode(WiFi.macAddress()));
    
    display.display();
    Serial.println("Zeige WLAN-Info für 20 Sekunden");
}

// Zeigt Wetterdaten auf dem Display an (mit persönlicher Note)
void showWeatherData() {
    display.clearDisplay();
    
    // Sensordaten in einer Operation lesen
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int lightLevel = analogRead(lightSensorPin);
    
    // Display-Layout effizienter gestalten mit weniger Zeilen
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Titel
    display.setCursor(25, 2);
    display.println("Wetter-Aktuell");
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SSD1306_WHITE);
    
    // Daten in optimiertem Layout
    display.setCursor(5, 16);
    display.print("Temp: ");
    display.print(t);
    display.println(" C");
    
    display.setCursor(5, 28);
    display.print("Luft: ");
    display.print(h);
    display.println(" %");
    
    display.setCursor(5, 40);
    display.print("Licht: ");
    display.println(lightLevel);
    
    // Device-Code ohne temporäre Variable
    display.setCursor(5, 52);
    display.print("ID: #");
    display.println(generateDeviceCode(WiFi.macAddress()));
    
    display.display();
    Serial.println("Zeige Wetterdaten");
}

// Aktualisiert und sendet Daten an den Server
void updateAndSendData() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WLAN-Verbindung verloren");
        return; // Frühes Zurückkehren, wenn keine Verbindung besteht
    }
    
    // Sensordaten lesen
    int lightLevel = analogRead(lightSensorPin);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    Serial.print("Helligkeit: "); 
    Serial.println(lightLevel);

    if (isnan(h) || isnan(t)) {
        Serial.println("Fehler beim Lesen des DHT-Sensors");
        return; // Frühes Zurückkehren bei Sensorfehlern
    }
    
    Serial.println("Luftfeuchtigkeit: " + String(h) + " %");
    Serial.println("Temperatur: " + String(t) + " °C");
    Serial.println();
    
    // Aktualisiere die LED-Anzeige basierend auf der Temperatur
    updateTemperatureLEDs(t);
    
    // HTTP-Client und WiFiClient erstellen
    WiFiClient client;
    HTTPClient http;
    
    // Daten für die Anfrage vorbereiten
    String deviceCode = generateDeviceCode(WiFi.macAddress());
    String postData = "light_level=" + String(lightLevel) + 
                     "&temperature=" + String(t) + 
                     "&humidity=" + String(h) + 
                     "&device_code=" + deviceCode +
                     "&esp_id=" + WiFi.macAddress();  // Direkte MAC-Adresse als esp_id
    
    // Verbindung zum Server herstellen und Daten senden
    Serial.println("Sende Daten an Server...");
    http.begin(client, "http://3r6nuss.de:7456/esp_server/sensor_data.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(postData);
    
    // Antwort verarbeiten
    if (httpResponseCode > 0) {
        Serial.print("HTTP Response Code: ");
        Serial.println(httpResponseCode);
        Serial.println("Antwort vom Server: " + http.getString());
    } else {
        Serial.print("Fehler bei der HTTP-Anfrage: ");
        Serial.println(httpResponseCode);
    }
    
    http.end(); // Verbindung schließen
}

// Aktualisiert die LED-Anzeige basierend auf der Temperatur
void updateTemperatureLEDs(float temperature) {
    // Alle LEDs zuerst ausschalten
    digitalWrite(LED_LOW, LOW);
    digitalWrite(LED_MEDIUM, LOW);
    digitalWrite(LED_HIGH, LOW);
    
    // Je nach Temperatur die entsprechende LED einschalten
    if (temperature < TEMP_LOW) {
        // Unter 15°C - nur LOW-LED leuchtet
        digitalWrite(LED_LOW, HIGH);
        Serial.println("LED: Niedrige Temperatur");
    } else if (temperature >= TEMP_LOW && temperature < TEMP_MEDIUM) {
        // Zwischen 15°C und 25°C - nur MEDIUM-LED leuchtet
        digitalWrite(LED_MEDIUM, HIGH);
        Serial.println("LED: Mittlere Temperatur");
    } else {
        // Über 25°C - nur HIGH-LED leuchtet
        digitalWrite(LED_HIGH, HIGH);
        Serial.println("LED: Hohe Temperatur");
    }
}

// Redirect-Funktion, um Benutzer zur Hauptwebseite umzuleiten
void handleRedirect() {
  server.sendHeader("Location", "http://3r6nuss.de:7456/index.php", true);
  server.send(302, "text/plain", "Redirecting to http://3r6nuss.de:7456/index.php");
}