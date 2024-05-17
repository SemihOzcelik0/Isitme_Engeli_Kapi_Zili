#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>

#define ses 5
bool voice = false;
byte komut = 0;

unsigned long previousMillis = 0;
const long interval = 5000;

bool ileti = false, sinyal = false;

bool signal = false;

typedef struct struct_message
{
  bool ileti;
  bool sinyal;
} struct_message;

uint8_t nokta[] = {0x08, 0xF9, 0xE0, 0x6D, 0x9C, 0x64};
uint8_t arti[] = {0x08, 0xF9, 0xE0, 0x6D, 0xA1, 0x54};
uint8_t eksi[] = {0x08, 0xF9, 0xE0, 0x6D, 0xA0, 0x03};

struct_message iletiGonder;
struct_message gelenVeri;

void ReceiveData(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&gelenVeri, incomingData, sizeof(gelenVeri));

  ileti = gelenVeri.ileti;
  sinyal = gelenVeri.sinyal;
}

void transmitData(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Son Verinin GÖnderilme Durumu: ");
  sendStatus ? Serial.println("İstek İletilemedi !!!") : Serial.println("İstek mesajı iletildi");
}

void setup()
{

  Serial.begin(9600);
  Serial.write(0xAA);
  Serial.write(0x37);

  delay(1000);

  Serial.write(0xAA);
  Serial.write(0x21);

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  pinMode(ses, INPUT);

  if (esp_now_init() != 0)
  {
    Serial.println("ESP NOW Başaltılamadı!!!");
    return;
  }
  else
  {
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(70);
    }
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(transmitData);
  esp_now_register_recv_cb(ReceiveData);

  esp_now_add_peer(nokta, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(arti, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(eksi, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (Serial.available())
  {
    komut = Serial.read();
    delay(50);
  }

  if (komut == 0x11 || komut == 0x12 || komut == 0x13 || komut == 0x14)
  {
    voice = true;
  }

  if (voice == true && signal == false)
  {
    iletiGonder.ileti = true;
    iletiGonder.sinyal = true;

    esp_now_send(nokta, (uint8_t *)&iletiGonder, sizeof(iletiGonder));
    delay(50);
    esp_now_send(arti, (uint8_t *)&iletiGonder, sizeof(iletiGonder));
    delay(50);
    esp_now_send(eksi, (uint8_t *)&iletiGonder, sizeof(iletiGonder));
    delay(50);

    signal = true;
    voice = false;
    komut = 0;

    Serial.println("İleti Gönderildi\n\n");
  }

  if (ileti == true && signal == true)
  {
    iletiGonder.ileti = true;
    iletiGonder.sinyal = false;

    esp_now_send(nokta, (uint8_t *)&iletiGonder, sizeof(iletiGonder));
    delay(50);
    esp_now_send(arti, (uint8_t *)&iletiGonder, sizeof(iletiGonder));
    delay(50);
    esp_now_send(eksi, (uint8_t *)&iletiGonder, sizeof(iletiGonder));
    delay(50);

    signal = false;
    ileti = false;
    voice = false;
    komut = 0;
    Serial.println("İleti Geldi\n\n");
  }
  voice = false;
}
