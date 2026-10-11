#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

String never_gonna[] = {
  "Turktelekom_5G_A342",
  "TURKTELEKOM_KURSAD",
  "Superonline_WiFi_9821",
  "Kablonet_Guest_44",
  "VodafoneNet_Bedava",
  "Turktelekom_Tivibu",
  "Superonline_Fiber_77",
  "Free_Internet_WiFi",
  "Turktelekom_Komsu",
  "Modem_Karti_Yanik"
};

const int update_interval = 30000; // SSID update interval in ms (every 30s default)

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer webServer(80);

// File and log details
String logFilename = "/connection_log.txt";
String counterFilename = "/counter.txt"; // File to store the connection counter
unsigned long connectionAttempts = 0; // Counter for connection attempts

String responseHTML = "<!DOCTYPE html><html lang='en'><head>"
                      "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>Rickrolling the WiFi</title>"
                      "<style>"
                      "body { background-color: #111; color: #fff; font-family: Arial, sans-serif; text-align: center; padding-top: 50px; }"
                      "h1 { color: #ff0055; }"
                      ".btn { display: inline-block; background-color: #ff0000; color: white; padding: 15px 30px; font-size: 20px; font-weight: bold; text-decoration: none; border-radius: 8px; margin-top: 20px; box-shadow: 0px 4px 10px rgba(255,0,0,0.3); }"
                      ".btn:hover { background-color: #cc0000; }"
                      "</style>"
                      "</head><body>"
                      "<h1>bedava internet için bas 🤑🤑🤑🤑</h1>"
                      "<p>İnternete bağlanmak için aşağıdaki butona tıkla:</p>"
                      "<a class='btn' href='https://youtu.be/52IoCmGPnx8?si=nB9-rl3HhRYnoF8G' target='_blank'>Devam Et / Müziği Çal 🎵</a>"
                      "<p style='margin-top: 40px; color: #777;'>You can blame this on \@h0bbel</p>"
                      "</body></html>";

String adminHTML = "<!DOCTYPE html><html lang='en'><head>"
                   "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"
                   "<meta name='viewport' content='width=device-width'>"
                   "<title>Administration</title></head><body>"
                   "<h1>Never Gonna Give You Up WiFi Beacon Administration</h1>"
                      "⡀⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠉⠁⠀⣀⣀⠀⣀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⡀⠀⠀⠀<br>"
                      "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣛⣿⣿⣷⣿⣿⣯⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣏⠉⣿⠉⢹⠟⢉⠉⢻⣿⠉⢻⠋⠙⡏⣿⠋⢻⡏⠉⣿⠉⣉⣻⠀<br>"
                      "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣻⡀⠈⢀⣿⠀⢸⠀⠀⣿⠀⢸⠀⠰⣿⣿⠀⢸⠁⢀⡟⠀⢹⣿⠀<br>"
                      "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⡿⠿⠿⢿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣧⠀⣼⣿⠀⢸⡀⠀⣏⠀⢸⠀⠀⣿⣿⡄⠘⠀⢸⡇⠀⢰⣾⠀<br>"
                      "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⡿⠋⠀⠀⠀⠀⠀⠈⠉⠉⠁⠀⠀⠈⢻⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣀⣿⣿⣆⡈⢁⣰⣿⣄⠘⢀⣼⣿⣿⣇⣀⣀⣼⣧⣀⣈⣹⡇<br>"
                      "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⣿⣿⣿⣿⣿⣿⠟⠿⢿⣿⠿⠛⠛⠻⠿⠿⠻⠛⠉⠉⠉⠀<br>"
                      "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠀⠀⢀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣧⣄⠀⠀⠀⠀⠀⠀⣴⠶⡶⠿⣿⣿⠿⠿⢿⡿⠿⠿⣿⠿⢿⡿⢿⡿⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⣿⣿⠀⠀⢨⣭⣽⣿⡇⠀⢠⣾⣿⣿⣷⣆⠀⠀⢸⣿⠀⠀⠀⠀⠀⠀⣿⠀⢱⡆⠈⣿⠀⢴⣾⡇⠀⣶⣿⠀⠘⡇⠀⡇⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⠉⠛⠀⠀⠀⠀⠉⠁⠀⠀⠘⡏⠉⠉⠛⠋⠀⣠⣼⣿⠀⠀⠀⠀⠀⠀⣿⠀⢨⡁⠺⣿⠀⣈⣹⡇⠀⣉⣿⠀⡀⠁⠀⡇⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⢹⡄⠀⠀⠀⠀⣿⣿⡿⠀⠀⠀⠀⠀⠀⣿⠀⠸⠇⠀⣿⠀⠹⢿⡇⠀⠿⢿⠀⢸⡀⠀⡇⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⢷⣄⡀⠀⢠⡾⠋⠀⠛⢶⣶⣾⡇⠀⣠⠄⢰⣿⠟⠀⠀⠀⠀⠀⠀⠀⠻⢶⣶⡶⠚⠓⠶⠶⠾⠷⠶⠶⠾⠶⠾⠳⠾⠟⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣹⡷⣠⠏⠙⢷⣶⠲⠶⠶⣷⣶⡿⠋⢀⣾⠃⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣹⣧⡀⢀⠀⠀⣀⣀⣀⡀⢀⣀⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠟⣫⣽⠃⠀⠀⠀⠉⠉⠙⠛⠋⠀⠀⢀⣾⡃⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠉⢉⡉⠻⡏⠉⣿⠟⢉⡉⠙⣿⠉⢹⡏⢉⡿⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠛⠁⠀⣼⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⡏⢳⡄⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠸⠇⣰⡇⠀⣿⠀⢸⣧⣀⣿⠀⠈⠀⣼⠁⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⠀⠀⠀⠀⠀⠀⢀⣴⠏⠀⠀⠀⢸⣿⣿⡀⠀⠀⠰⣦⣄⡀⣀⣤⡾⣿⣿⣧⠀⠻⢦⣄⡀⠀⠀⠀⠀⠀⣿⠀⢸⠀⠈⡇⠀⣿⠀⢸⡟⠛⣿⠀⢠⠀⢹⣆⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⠀⠀⠀⠀⠀⠀⠘⠁⠀⠀⠀⠀⣾⣿⣿⣷⣄⡀⠀⠙⠿⣿⣏⣽⣿⣿⣿⣿⠄⢸⣧⠈⠙⠶⣤⣀⠀⠀⣿⣀⣸⣄⣠⣷⣀⣿⣦⣀⣁⣠⣿⣀⣸⣧⣀⣿⠀⠀⠀⠀⠀⠀⠀<br>"
                      "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣿⣿⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⠀⠀⠹⣆⠀⠀⠀⠉⠳⣦⡀⠉⠉⠙⠻⣿⠉⠁⠀⠉⠉⠀⠀⠈⠉⠀⠉⠹⠇⠀⠀⠀⠀⠀⠀<br>"
                      "⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⢿⡆⠀⠀⠀⠀⠻⣿⠓⠒⠲⢦⣹⠷⠒⠲⣶⡖⠒⣶⣶⠒⢶⣾⠗⠒⠲⡶⠒⡖⠶⣄<br>"
                      "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⡞⣿⠀⠀⠀⠀⠀⢹⠀⢹⡀⢈⡏⠀⣿⠀⢸⡇⠀⣿⡟⠀⢸⣿⠀⢸⣶⡇⠀⢳⠀⢸<br>"
                      "⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀⢀⣰⠃⢹⡆⠀⠀⠀⠀⢸⠀⢠⠀⠛⡇⠀⣿⠀⢸⡇⠀⣿⡇⠀⢸⣿⠀⢠⣬⡇⠀⢸⠀⢸<br>"
                      "⠀⠀⠀⠀⠀⠀⠀⠙⠛⠛⠋⠉⠀⠺⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠶⠞⠋⠀⠀⠀⢿⠀⠀⠀⠀⣸⠀⢸⠀⢰⣧⠀⠛⠀⣸⡇⠀⠛⣧⠀⠘⢻⠀⠘⠛⡇⠀⠚⠀⢸<br>"
                      "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠆⠀⠀⠀⠿⠓⠛⠓⠛⠉⠙⠒⠚⠉⠛⣛⡚⠛⠛⠛⠛⠛⠓⡚⠛⠛⠓⠛⠉<br>"
                   "<p><a href=\"/log\">Show Log</a></p>"
                   "<p><a href=\"/reset\">Reset Log</a></p>"
                   "</body></html>";

void loadConnectionAttempts() {
  File counterFile = SPIFFS.open(counterFilename, FILE_READ);
  if (counterFile) {
    String countString = counterFile.readStringUntil('\n');
    connectionAttempts = countString.toInt();
    counterFile.close();
  } else {
    connectionAttempts = 0;
  }
}

void saveConnectionAttempts() {
  File counterFile = SPIFFS.open(counterFilename, FILE_WRITE);
  if (counterFile) {
    counterFile.println(connectionAttempts);
    counterFile.close();
  }
}

void logClientConnection() {
  connectionAttempts++;
  unsigned long timestamp = millis();
  String logEntry = "Attempt " + String(connectionAttempts) + " at: " + String(timestamp) + " ms\n";
  File logFile = SPIFFS.open(logFilename, FILE_APPEND);
  if (logFile) {
    logFile.println(logEntry);
    logFile.close();
  }
  Serial.print(logEntry);
  saveConnectionAttempts();
}

void resetLogs() {
  connectionAttempts = 0;
  saveConnectionAttempts();
  
  File logFile = SPIFFS.open(logFilename, FILE_WRITE);
  if (logFile) {
    logFile.close();
  }
  
  Serial.println("Logs and counter have been reset.");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("RickRoller started...");

  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  loadConnectionAttempts();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  
  Serial.println("Setting SSID to '" + never_gonna[0] + "'.");
  WiFi.softAP(never_gonna[0].c_str());

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
      logClientConnection();
    }
  });

  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.onNotFound([]() {
    webServer.send(200, "text/html; charset=utf-8", responseHTML);
  });

  webServer.on("/log", []() {
    File logFile = SPIFFS.open(logFilename, FILE_READ);
    if (logFile) {
      webServer.streamFile(logFile, "text/plain");
      logFile.close();
    } else {
      webServer.send(404, "text/plain", "Log file not found.");
    }
  });

  webServer.on("/reset", []() {
    resetLogs();
    webServer.send(200, "text/plain", "Logs and counter have been reset.");
  });

  webServer.on("/admin", []() {
    webServer.send(200, "text/html; charset=utf-8", adminHTML);
  });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  
  static unsigned long last_update_time = 0;
  if (millis() - last_update_time > update_interval) {
    static int current_line = 0;
    current_line = (current_line + 1) % (sizeof(never_gonna) / sizeof(String));
    Serial.println("Setting SSID to '" + never_gonna[current_line] + "'.");
    WiFi.softAP(never_gonna[current_line].c_str());
    last_update_time = millis();
  }
  delay(1000);
}