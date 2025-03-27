#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "WLAN-782577";       // Hier deinen WLAN-Namen eintragen
const char* password = "60025416322051135176"; // Hier dein WLAN-Passwort eintragen

ESP8266WebServer server(80);

// LED Pins
#define LED_PIN1 12
#define LED_PIN2 13
#define LED_PIN3 15

// HTML Page
String getHTML() {
    String html = "<!DOCTYPE html><html><head><title>LED Control</title>";
    html += "<style>button{margin:10px;padding:10px;}</style></head><body><h1>LED Control</h1>";
    html += "<div><h3>Blaue LED (GPIO 12)</h3><a href='/led1/on'><button>ON</button></a><a href='/led1/off'><button>OFF</button></a></div>";
    html += "<div><h3>Grüne LED (GPIO 13)</h3><a href='/led2/on'><button>ON</button></a><a href='/led2/off'><button>OFF</button></a></div>";
    html += "<div><h3>Rote LED (GPIO 15)</h3><a href='/led3/on'><button>ON</button></a><a href='/led3/off'><button>OFF</button></a></div>";
    html += "</body></html>";
    return html;
}

void handleRedirect() {
    server.sendHeader("Location", "http://3r6nuss.de:7456/index.php", true);
    server.send(302, "text/plain", "Redirecting to http://3r6nuss.de:7456/index.php");
}

void setup() {
    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    pinMode(LED_PIN3, OUTPUT);

    digitalWrite(LED_PIN1, LOW);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);

    Serial.begin(115200);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Verbinde mit WLAN...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        display.print(".");
        display.display();
    }

    Serial.println("\nConnected to WiFi. IP: " + WiFi.localIP().toString());

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Verbunden mit:");
    display.println(ssid);
    display.println("\nIP-Adresse:");
    display.println(WiFi.localIP().toString());
    display.println("\nRedirect aktiv zu:");
    display.println("3r6nuss.de:7456");
    display.display();

    server.on("/", []() { server.send(200, "text/html", getHTML()); });
    server.on("/control", []() { server.send(200, "text/html", getHTML()); });

    server.on("/led1/on", []() { digitalWrite(LED_PIN1, HIGH); server.send(200, "text/html", getHTML()); });
    server.on("/led1/off", []() { digitalWrite(LED_PIN1, LOW); server.send(200, "text/html", getHTML()); });

    server.on("/led2/on", []() { digitalWrite(LED_PIN2, HIGH); server.send(200, "text/html", getHTML()); });
    server.on("/led2/off", []() { digitalWrite(LED_PIN2, LOW); server.send(200, "text/html", getHTML()); });

    server.on("/led3/on", []() { digitalWrite(LED_PIN3, HIGH); server.send(200, "text/html", getHTML()); });
    server.on("/led3/off", []() { digitalWrite(LED_PIN3, LOW); server.send(200, "text/html", getHTML()); });

    server.begin();
}

void loop() {
    server.handleClient();

    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > 1000) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Verbunden mit:");
        display.println(ssid);
        display.println("\nIP-Adresse:");
        display.println(WiFi.localIP().toString());
        display.print("LED1: "); display.println(digitalRead(LED_PIN1) ? "AN" : "AUS");
        display.print("LED2: "); display.println(digitalRead(LED_PIN2) ? "AN" : "AUS");
        display.print("LED3: "); display.println(digitalRead(LED_PIN3) ? "AN" : "AUS");
        display.display();
        lastDisplayUpdate = millis();
    }
}