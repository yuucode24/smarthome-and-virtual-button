#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_TEMPLATE_ID           "TMPL6qzeUpNRk"
#define BLYNK_TEMPLATE_NAME         "Smarthome"
#define BLYNK_AUTH_TOKEN            "Qzbz4RSBQGQfnPcgC9-qtYHwNqU7DLGl"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial




// Konfigurasi WiFi
char* apSSID = "Smarthome";
char* apPassword = "12344321";
char* clientSSID = "TKJ";
char* clientPassword = "12344321";

// Konfigurasi Blynk
char auth[] = "Qzbz4RSBQGQfnPcgC9-qtYHwNqU7DLGl";

// Pin yang digunakan
const int lampu1Pin = D1;
const int lampu2Pin = D2;

// Variabel status lampu
bool lampu1Status = false;
bool lampu2Status = false;

// Web server lokal
ESP8266WebServer server(80);

void setup() {
  // Inisialisasi pin
  pinMode(lampu1Pin, OUTPUT);
  pinMode(lampu2Pin, OUTPUT);

  // Mulai serial monitor
  Serial.begin(115200);

  // Mengatur NodeMCU sebagai Access Point (AP)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);

  Serial.println("");
  Serial.print("NodeMCU AP terhubung dengan SSID: ");
  Serial.println(apSSID);

  // Mulai server web
  server.on("/", handleRoot);
  server.on("/lampu", handleLampu);
  server.on("/lampu/on", handleLampuOn);
  server.on("/lampu/off", handleLampuOff);
  server.on("/connect", handleConnect);
  server.on("/button1", handleButton1);
  server.on("/button2", handleButton2);
  server.on("/refresh", handleRefresh);
  server.begin();
  // Hubungkan ke WiFi eksternal sebagai client

  // Mulai koneksi Blynk
  Blynk.config(auth);
  Blynk.connect();
}

void loop() {
  server.handleClient();
  Blynk.run();
}

void handleRoot() {
  String html = "<html>"
                "<head>"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                "<style>"
                "body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 20px; }"
                "h1 { color: #333333; }"
                "p { margin: 10px 0; }"
                ".container { max-width: 400px; margin: 0 auto; }"
                ".button-container { display: flex; flex-direction: column; align-items: center; }"
                ".button { display: inline-block; background-color: #4CAF50; color: #ffffff; text-decoration: none; padding: 10px 20px; border-radius: 4px; margin: 10px; }"
                "</style>"
                "</head>"
                "<body>"
                "<div class=\"container\">"
                "<h1 style=\"text-align:center\">Smarthome</h1>"
                "<div class=\"button-container\">"
                "<a class=\"button\" href=\"/lampu/on\" ontouchstart=\"event.preventDefault();lampuOn();\">Hidupkan Semua Lampu</a>"
                "<a class=\"button\" href=\"/lampu/off\" ontouchstart=\"event.preventDefault();lampuOff();\">Matikan Semua Lampu</a>"
                "<a class=\"button\" href=\"/connect\">Status Jaringan</a>"
                "<a class=\"button\" href=\"/lampu\">Status Lampu</a>"
                "<a class=\"button\" href=\"/button1\" ontouchstart=\"event.preventDefault();button1();\">Lampu 1</a>"
                "<a class=\"button\" href=\"/button2\" ontouchstart=\"event.preventDefault();button2();\">Lampu 2</a>"
                "</div>"
                "</div>"
                "<script>"
                "function lampuOn() {"
                "  fetch('/lampu/off');"
                "}"
                "function lampuOff() {"
                "  fetch('/lampu/on');"
                "}"
                "function button1() {"
                "  fetch('/button1');"
                "}"
                "function button2() {"
                "  fetch('/button2');"
                "}"
                "</script>"
                "</body>"
                "</html>";

  server.send(200, "text/html", html);
}

void handleLampu() {
    String html = "<html><body>";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    html += "<h1>Status Lampu</h1>";
    html += "<p>Lampu 1: " + String(lampu1Status ? "Mati" : "Hidup") + "</p>";
    html += "<p>Lampu 2: " + String(lampu2Status ? "Mati" : "Hidup") + "</p>";
    html += "</body></html>";
  
    server.send(200, "text/html", html);
}

void handleLampuOn() {
  if (server.method() == HTTP_GET || server.method() == HTTP_POST) {
    lampu1Status = true;
    lampu2Status = true;
    digitalWrite(lampu1Pin, HIGH);
    digitalWrite(lampu2Pin, HIGH);
    server.send(200, "text/plain", "Lampu dinyalakan");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleLampuOff() {
  if (server.method() == HTTP_GET || server.method() == HTTP_POST) {
    lampu1Status = false;
    lampu2Status = false;
    digitalWrite(lampu1Pin, LOW);
    digitalWrite(lampu2Pin, LOW);
    server.send(200, "text/plain", "Lampu dimatikan");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}
bool isWiFiConnected = false;

void connectToWiFi() {
    WiFi.begin(clientSSID, clientPassword);

  Serial.print("Menghubungkan ke ");
  Serial.println(clientSSID);

  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("Alamat IP: " + WiFi.localIP().toString());

  unsigned long startTime = millis();  // Waktu awal

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > 10000) {  // Jika waktu mencapai 10 detik (10000 milidetik)
      Serial.println("Gagal terhubung ke WiFi");
      break;  // Keluar dari loop jika gagal terhubung
    }

    delay(1000);
    Serial.println("Mencoba terhubung ke WiFi...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi terhubung");
    Serial.print("Alamat IP: ");
    Serial.println(WiFi.localIP());

    isWiFiConnected = true;
  }
}
void handleConnect() {
  String html = "<html>"
                  "<head>"
                  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                  "<style>"
                  "body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 20px; }"
                  "h1 { color: #333333; }"
                  "p { margin: 10px 0; }"
                "</style>"
                  "</head>"
                  "<body>"
                  "<h1>Status Koneksi</h1>";

  if (WiFi.status() == WL_CONNECTED) {
    html += "<p>Status: Terhubung ke WiFi " + WiFi.SSID() + "</p>";
    html += "<p>Alamat IP: " + WiFi.localIP().toString() + "</p>";
  } else {
    html += "<p>Status: Tidak Terhubung ke WiFi</p>";
    html += "<p>Alamat IP: -</p>";
    html += "<form action=\"/refresh\" ontouchstart=\"event.preventDefault();refreshh();\">";
    html += "<input type=\"submit\" value=\"Connect\">";
    html += "</form>";
    html += "<script>";
    html += "function refreshh() {";
    html += "fetch('/refresh?');";
    html += "}";
    html += "</script>";
    html += "<p>Setelah klik connect lalu refresh ulang halaman</p>";
  }
  server.send(200, "text/html", html);
}

void handleRefresh() {
  connectToWiFi();
  handleConnect();  // Menampilkan ulang status koneksi setelah refresh
}

void handleButton1() {
  if (server.method() == HTTP_GET || server.method() == HTTP_POST) {
    if (lampu1Status) {
      lampu1Status = false;
      digitalWrite(lampu1Pin, LOW);
      server.send(200, "text/plain", "Lampu 1 dimatikan");
    } else {
      lampu1Status = true;
      digitalWrite(lampu1Pin, HIGH);
      server.send(200, "text/plain", "Lampu 1 dinyalakan");
    }
  } else {
    server.send(400, "text/plain", "Metode HTTP tidak didukung");
  }
}

void handleButton2() {
  if (server.method() == HTTP_GET || server.method() == HTTP_POST) {
    if (lampu2Status) {
      lampu2Status = false;
      digitalWrite(lampu2Pin, LOW);
      server.send(200, "text/plain", "Lampu 2 dimatikan");
    } else {
      lampu2Status = true;
      digitalWrite(lampu2Pin, HIGH);
      server.send(200, "text/plain", "Lampu 2 dinyalakan");
    }
  } else {
    server.send(400, "text/plain", "Metode HTTP tidak didukung");
  }
}

BLYNK_WRITE(V1)
{
  int value = !param.asInt();

  // Ubah status pin D1 sesuai dengan nilai yang diterima dari Blynk
  digitalWrite(D1, value);
}

// Fungsi untuk mengendalikan pin D2 melalui Blynk
BLYNK_WRITE(V2)
{
  int value = !param.asInt();

  // Ubah status pin D2 sesuai dengan nilai yang diterima dari Blynk
  digitalWrite(D2, value);
}
