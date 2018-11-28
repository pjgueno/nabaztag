#include <WiFi.h>;
#include <FS.h>;
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "XT_DAC_Audio.h"
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

#include "Pictures.h"
#include "SoundData.h"

#define BUFF_SIZE 64

const char* ssid = "XXXXXX";
const char* password = "XXXXXXX";

AsyncWebServer server(80);

const char HTML[] PROGMEM = "<h2>Vibreur</h2><form method=\"get\"> <input type=\"range\" name=\"vibreur_on\" min=\"0\" max=\"1000\" value=\"500\"> <br><input type=\"range\" name=\"vibreur_off\" min=\"0\" max=\"1000\" value=\"500\"> <br><input type=\"range\" name=\"time_vibreur\" min=\"1\" max=\"10\" value=\"5\"> <br><input type=\"submit\" value=\"Submit\"></form> <h2>Son</h2><form method=\"get\"> <input type=\"radio\" name=\"son\" value=\"force\"> Force<br><input type=\"radio\" name=\"son\" value=\"barbara\"> Barbara<br><input type=\"radio\" name=\"son\" value=\"kissed\"> Kissed<br><input type=\"radio\" name=\"son\" value=\"petitcul\"> Petit cul<br><input type=\"submit\" value=\"Submit\"></form> <h2>Yeux</h2> <form method=\"get\"> <input type=\"range\" name=\"eyes_on\" min=\"0\" max=\"1000\" value=\"200\"> <br><input type=\"range\" name=\"eyes_off\" min=\"0\" max=\"1000\" value=\"200\"> <br><input type=\"range\" name=\"time_eyes\" min=\"1\" max=\"10\" value=\"5\"> <br><input type=\"submit\" value=\"Submit\"></form> <h2>Antennes</h2><form method=\"get\"> <input type=\"range\" name=\"antenna_on\" min=\"0\" max=\"1000\" value=\"200\"> <br><input type=\"range\" name=\"antenna_off\" min=\"0\" max=\"1000\" value=\"200\"> <br><input type=\"range\" name=\"time_antenna\" min=\"1\" max=\"10\" value=\"5\"> <br><input type=\"submit\" value=\"Submit\"></form> <h2>Text</h2><form method=\"get\"> <textarea name=\"message\" rows=\"3\" cols=\"30\">Type text...</textarea> <br><input type=\"submit\" value=\"Submit\"></form> <h2>Image</h2><form method=\"get\"><input type=\"radio\" name=\"image\" value=\"coeur\"> Coeur<br><input type=\"radio\" name=\"image\" value=\"bisous\"> Bisous<br><input type=\"radio\" name=\"image\" value=\"hitch\"> Hitch<br><input type=\"radio\" name=\"image\" value=\"ilu\"> I love you<br><input type=\"submit\" value=\"Submit\"></form>";

XT_DAC_Audio_Class DacAudio(26, 0);
XT_Wav_Class Son1(force);
XT_Wav_Class Son2(barbara);
XT_Wav_Class Son3(kissed);
XT_Wav_Class Son4(petitcul);

bool triger1 = false;
bool triger2 = false;
bool triger3 = false;
bool triger4 = false;

unsigned long currentMillis;
unsigned long stopMillis1;
unsigned long stopMillis2;
unsigned long stopMillis3;
unsigned long stopMillis4;

uint16_t OnTime1;
uint16_t OffTime1;

uint16_t OnTime2;
uint16_t OffTime2;

uint16_t OnTime3;
uint16_t OffTime3;

unsigned long previousMillis1;
unsigned long previousMillis2;
unsigned long previousMillis3;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  Son1.RepeatForever = false;
  Son2.RepeatForever = false;
  Son3.RepeatForever = false;
  Son4.RepeatForever = false;
  
  pinMode(25, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(21, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  triger1 = true;
  currentMillis = millis();
  stopMillis1 = millis() + 5000;

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {

    if (request->hasParam("vibreur_on")) {
      Serial.println("vibreur");
      AsyncWebParameter* p9 = request->getParam("vibreur_on");
      AsyncWebParameter* p10 = request->getParam("vibreur_off");
      AsyncWebParameter* p2 = request->getParam("time_vibreur");

      OnTime3 = p9->value().toInt();
      OffTime3 = p10->value().toInt();
      triger2 = true;
      currentMillis = millis();
      previousMillis3 = 0;
      stopMillis2 = millis() + (p2->value().toInt() * 1000);
    }

    if (request->hasParam("son")) {
        Serial.println("Son");
        AsyncWebParameter* p12 = request->getParam("son");
      if (p12->value() == "force") {DacAudio.Play(&Son1);}
      if (p12->value() == "barbara") {DacAudio.Play(&Son2);}
      if (p12->value() == "kissed") {DacAudio.Play(&Son3);} 
      if (p12->value() == "petitcul") {DacAudio.Play(&Son4);}    
    }

    if (request->hasParam("image")) {
      Serial.println("Image");
      digitalWrite(21, HIGH);
      AsyncWebParameter* p11 = request->getParam("image");

      if (p11->value() == "coeur") {show(coeur);}
      if (p11->value() == "hitch") {show(hitch);}
      if (p11->value() == "ilu") {show(ilu);}
      if (p11->value() == "bisous") {show(bisous);}
     
      triger1 = true;
      currentMillis = millis();
      stopMillis1 = millis() + 10000;
    }

    if (request->hasParam("message")) {
      AsyncWebParameter* p1 = request->getParam("message");
      Serial.println("Message");
      Serial.printf(p1->name().c_str());
      Serial.printf(p1->value().c_str());
      digitalWrite(21, HIGH);
      printer(p1->value().c_str());
      triger1 = true;
      currentMillis = millis();
      stopMillis1 = millis() + 20000;
    }

    if (request->hasParam("eyes_on")) {
      Serial.println("Eyes");
      AsyncWebParameter* p3 = request->getParam("eyes_on");
      AsyncWebParameter* p4 = request->getParam("eyes_off");
      AsyncWebParameter* p5 = request->getParam("time_eyes");

      OnTime1 = p3->value().toInt();
      OffTime1 = p4->value().toInt();
      triger3 = true;
      currentMillis = millis();
      previousMillis1 = 0;
      stopMillis3 = millis() + (p5->value().toInt() * 1000);
    }

    if (request->hasParam("antenna_on")) {
      Serial.println("antenna");
      AsyncWebParameter* p6 = request->getParam("antenna_on");
      AsyncWebParameter* p7 = request->getParam("antenna_off");
      AsyncWebParameter* p8 = request->getParam("time_antenna");

      OnTime2 = p6->value().toInt();
      OffTime2 = p7->value().toInt();
      triger4 = true;
      currentMillis = millis();
      previousMillis2 = 0;
      stopMillis4 = millis() + (p8->value().toInt() * 1000);
    }

    request->send(200, "text/html", HTML);
  });

  server.begin();
}

void loop() {
  DacAudio.FillBuffer();

  if (triger1 == true) {
    currentMillis = millis();
    if (currentMillis > stopMillis1) {
      triger1 = false;
      tft.fillScreen(TFT_BLACK);
      digitalWrite(21, LOW);
    }
  }

  if (triger2 == true) {
    currentMillis = millis();
    if (currentMillis > stopMillis2) {
      triger2 = false;
      digitalWrite(25, LOW);
    }else {

      if ((digitalRead(25) == LOW) && (currentMillis - previousMillis3 >= OffTime3)) {
        digitalWrite(25, HIGH);
        previousMillis3 = currentMillis;
      }

      if ((digitalRead(25) == HIGH) && (currentMillis - previousMillis3 >= OnTime3)) {
        digitalWrite(25, LOW);
        previousMillis3 = currentMillis;
      }
    }
  }


  if (triger3 == true) {
    currentMillis = millis();
    if (currentMillis > stopMillis3) {
      triger3 = false;
      digitalWrite(16, LOW);
      digitalWrite(17, LOW);      
    } 
    
    else {

      if ((digitalRead(16) == LOW && (digitalRead(17) == HIGH || digitalRead(17) == LOW)) && (currentMillis - previousMillis1 >= OffTime1)) {
        digitalWrite(16, HIGH);
        digitalWrite(17, LOW);
        previousMillis1 = currentMillis;
      }

      if ((digitalRead(16) == HIGH && digitalRead(17) == LOW) && (currentMillis - previousMillis1 >= OnTime1)) {
        digitalWrite(16, LOW);
        digitalWrite(17, HIGH);
        previousMillis1 = currentMillis;
      }
    }
  }


  if (triger4 == true) {
    currentMillis = millis();
    if (currentMillis > stopMillis4) {
      triger4 = false;
      digitalWrite(4, LOW);
      digitalWrite(15, LOW);
    } else {
      
      if ((digitalRead(4) == LOW && (digitalRead(15) == HIGH || digitalRead(15) == LOW)) && (currentMillis - previousMillis2 >= OffTime2)) {
        digitalWrite(4, HIGH);
        digitalWrite(15, LOW);
        previousMillis2 = currentMillis;
      }

      if ((digitalRead(4) == HIGH && digitalRead(15) == LOW) && (currentMillis - previousMillis2 >= OnTime2)) {
        digitalWrite(4, LOW);
        digitalWrite(15, HIGH);
        previousMillis2 = currentMillis;
      }
    }
  }
}

void show(const unsigned short* icon) {

  uint16_t  pix_buffer[BUFF_SIZE];   // Pixel buffer (16 bits per pixel)

  // Set up a window the right size to stream pixels into
  tft.setAddrWindow(0, 0, 159, 127);

  // Work out the number whole buffers to send
  uint16_t nb = ((uint16_t)128 * 160) / BUFF_SIZE;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < BUFF_SIZE; j++) {
      pix_buffer[j] = pgm_read_word(&icon[i * BUFF_SIZE + j]);
    }
    tft.pushColors(pix_buffer, BUFF_SIZE);
  }

  // Work out number of pixels not yet sent
  uint16_t np = ((uint16_t)128 * 160) % BUFF_SIZE;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * BUFF_SIZE + i]);
    tft.pushColors(pix_buffer, np);
  }
}

void printer(const char *string ) {
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextWrap(true);
  tft.print(string);
}

