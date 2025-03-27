#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Konfiguration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset Pin wenn -1, dann wird der Arduino reset pin verwendet
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "bbs1-raspberry";       // WLAN-Name geändert
const char* password = ""; // Passwort geändert

ESP8266WebServer server(80);

// LED pins
#define LED_PIN1 12
#define LED_PIN2 13
#define LED_PIN3 15

// HTML page
String getHTML() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>LED Control</title>";
  html += "<style>button{margin:10px;padding:10px;}</style></head>";
  html += "<body><h1>LED Control</h1>";
  
  // LED 1 (GPIO 12) - Blau
  html += "<div><h3>Blaue LED (GPIO 12)</h3>";
  html += "<a href='/led1/on'><button>ON</button></a>";
  html += "<a href='/led1/off'><button>OFF</button></a></div>";
  
  // LED 2 (GPIO 13) - Grün
  html += "<div><h3>Grüne LED (GPIO 13)</h3>";
  html += "<a href='/led2/on'><button>ON</button></a>";
  html += "<a href='/led2/off'><button>OFF</button></a></div>";
  
  // LED 3 (GPIO 15) - Rot
  html += "<div><h3>Rote LED (GPIO 15)</h3>";
  html += "<a href='/led3/on'><button>ON</button></a>";
  html += "<a href='/led3/off'><button>OFF</button></a></div>";
  
  html += "</body></html>";
  return html;
}

// Redirect-Funktion
void handleRedirect() {
  server.sendHeader("Location", "http://3r6nuss.de:7456/index.php", true);
  server.send(302, "text/plain", "Redirecting to http://3r6nuss.de:7456/index.php");
}

void setup() {
  // Initialize pins
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  
  // Start with LEDs off
  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN2, LOW);
  digitalWrite(LED_PIN3, LOW);
  
  // Initialisiere das OLED-Display
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adresse 0x3C für 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Endlosschleife im Fehlerfall
  }
  
  // Display vorbereiten
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Verbinde mit WLAN...");
  display.display();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());
  
  // Zeige IP-Adresse auf dem Display an
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Verbunden mit:");
  display.println(ssid);
  display.println("");
  display.println("IP-Adresse:");
  display.println(WiFi.localIP().toString());
  display.println("");
  display.println("Redirect aktiv zu:");
  display.println("3r6nuss.de:7456");
  display.display();
  
  // Set up web server routes
  server.on("/", handleRedirect); // Hauptseite auf Redirect umgestellt
  
  // LED Control-Seite unter separater URL
  server.on("/control", []() {
    server.send(200, "text/html", getHTML());
  });
  
  // LED 1 routes
  server.on("/led1/on", []() {
    digitalWrite(LED_PIN1, HIGH);
    server.send(200, "text/html", getHTML());
  });
  
  server.on("/led1/off", []() {
    digitalWrite(LED_PIN1, LOW);
    server.send(200, "text/html", getHTML());
  });
  
  // LED 2 routes
  server.on("/led2/on", []() {
    digitalWrite(LED_PIN2, HIGH);
    server.send(200, "text/html", getHTML());
  });
  
  server.on("/led2/off", []() {
    digitalWrite(LED_PIN2, LOW);
    server.send(200, "text/html", getHTML());
  });
  
  // LED 3 routes
  server.on("/led3/on", []() {
    digitalWrite(LED_PIN3, HIGH);
    server.send(200, "text/html", getHTML());
  });
  
  server.on("/led3/off", []() {
    digitalWrite(LED_PIN3, LOW);
    server.send(200, "text/html", getHTML());
  });
  
  server.begin();
}

void loop() {
  server.handleClient();
  
  // Aktualisiere IP-Adresse auf dem Display alle 10 Sekunden
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > 10000) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Verbunden mit:");
    display.println(ssid);
    display.println("");
    display.println("IP-Adresse:");
    display.println(WiFi.localIP().toString());
    
    // Zeige aktuellen LED-Status an
    display.print("LED1: ");
    display.println(digitalRead(LED_PIN1) ? "AN" : "AUS");
    display.print("LED2: ");
    display.println(digitalRead(LED_PIN2) ? "AN" : "AUS");
    display.print("LED3: ");
    display.println(digitalRead(LED_PIN3) ? "AN" : "AUS");
    
    display.display();
    lastDisplayUpdate = millis();
  }
}