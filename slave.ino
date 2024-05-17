#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>

#define btn 4
int ledK = 5, ledM = 14;

bool button;

bool ileti = false, sinyal = false;

bool signal = false, ledState = true, buttonState = true;

typedef struct struct_message
{
  bool ileti;
  bool sinyal;
} struct_message;

uint8_t alici[] = {0x08, 0xF9, 0xE0, 0x6D, 0xBD, 0x20};

struct_message iletiGonder;
struct_message gelenVeri;

void ReceiveData(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&gelenVeri, incomingData, sizeof(gelenVeri));

  ileti = gelenVeri.ileti;
  signal = gelenVeri.sinyal;
}

void transmitData(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Son Verinin GÖnderilme Durumu: ");
  sendStatus ? Serial.println("İstek İletilemedi !!!") : Serial.println("İstek mesajı iletildi");
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  pinMode(btn, INPUT_PULLUP);

  pinMode(ledK, OUTPUT);
  pinMode(ledM, OUTPUT);

  if (esp_now_init() != 0)
  {
    Serial.println("ESP NOW Başaltılamadı!!!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(transmitData);
  esp_now_register_recv_cb(ReceiveData);

  esp_now_add_peer(alici, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop()
{
  unsigned long currentMillis = millis();

  button = digitalRead(btn);

  if (button == false && buttonState == true)
  {
    iletiGonder.ileti = true;
    iletiGonder.sinyal = false;

    esp_now_send(alici, (uint8_t *)&iletiGonder, sizeof(iletiGonder));
    buttonState = false;
  }

  if (ileti == true && signal == false)
  {
    ileti = false;
    buttonState = true;
    digitalWrite(ledM, LOW);
    digitalWrite(ledK, LOW);

    Serial.println("İleti Geldi\n\n");
  }

  if (signal == true)
  {
    if (ledState == true)
    {
      digitalWrite(ledM, HIGH);
      delay(100);
      digitalWrite(ledK, LOW);
      delay(100);
      ledState = false;
    }
    else
    {
      digitalWrite(ledM, LOW);
      delay(100);
      digitalWrite(ledK, HIGH);
      delay(100);
      ledState = true;
    }
  }
}
