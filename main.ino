#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>


const char* ssid     = "Wokwi-GUEST"; 
const char* password = "";             


#define BOTtoken  ""  
#define CHAT_ID   ""                                    


WiFiClientSecure  client;
UniversalTelegramBot bot(BOTtoken, client);

const int botRequestDelay    = 1000;
unsigned long lastTimeBotRan = 0;

const int ledPin = 2;
bool      ledState = LOW;


void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println("Received: " + text);

    if (text == "/start") {
      String welcome = "Bem-vindo ao Bot ESP32 LED!\n";
      welcome += "/led_on  - Acende o LED\n";
      welcome += "/led_off - Apaga o LED\n";
      welcome += "/state   - Informa estado atual\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (text == "/led_on") {
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      bot.sendMessage(chat_id, "LED is ON", "");
    }
    else if (text == "/led_off") {
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      bot.sendMessage(chat_id, "LED is OFF", "");
    }
    else if (text == "/state") {
      String msg = (digitalRead(ledPin) == HIGH)
                   ? "LED is currently ON"
                   : "LED is currently OFF";
      bot.sendMessage(chat_id, msg, "");
    }
    else {
      bot.sendMessage(chat_id, "Unknown command", "");
    }
  }
}


void setup() {
  Serial.begin(115200);

 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, 6);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi Connected! IP: ");
  Serial.println(WiFi.localIP());

  
  client.setInsecure();
}


void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages) {
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }
}
