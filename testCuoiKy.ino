#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLWSOBlRGk"
#define BLYNK_DEVICE_NAME "Test3Cambien"
#define BLYNK_AUTH_TOKEN "s4BKEPUP1cKjzyJkCoeE_3_MHjvOGBeA"

#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

byte icon[] = {
  B01110, B10001, B01110, B00100, B11111, B00100, B01010, B01010
};

char auth[] = "s4BKEPUP1cKjzyJkCoeE_3_MHjvOGBeA";
char ssid[] = "TuanKiet";
char pass[] = "82822222";

BlynkTimer timer;

int doAmBomTuoi;
int doAmTatBomTuoi;

int chedohoatdong;
int giamsat;

int relay = D3; //dieu khien relay
int cambien = A0; //doc cam bien do am dat tu A0

#define DHTPIN D4 //nhan gia tri cam bien nhiet do, do am
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  delay(1000);
  Blynk.begin(auth,ssid,pass);
  
  dht.begin();
  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  lcd.createChar(1, icon);

  pinMode(relay,OUTPUT);
  digitalWrite(relay,LOW);
  
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8);
}

BLYNK_WRITE(V5){
  doAmBomTuoi = param.asInt();
  Serial.print("Mức thấp");
  Serial.println(doAmBomTuoi);
}

BLYNK_WRITE(V6){
  doAmTatBomTuoi = param.asInt();
  Serial.print("Mức cao");
  Serial.println(doAmTatBomTuoi);
}

BLYNK_WRITE(V7){
  chedohoatdong = param.asInt();
  Serial.println("Chế độ hoạt động");
  Serial.print(chedohoatdong);
}

BLYNK_WRITE(V8){
  giamsat = param.asInt();
  Serial.println("Chế độ giam sat");
  Serial.print(giamsat);
}

void loop() {
  Blynk.run();
  timer.run();

  //do am va nhiet do
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  //do am dat
  int ana = analogRead(cambien);
  int phamtramao = map(ana,0,1023,0,100);
  int doAmDat = 100 - phamtramao;

  //Dua gia tri len blynk
  Blynk.virtualWrite(V1,t);
  Serial.println(t);
  Blynk.virtualWrite(V2,h);
  Serial.println(h);
  Blynk.virtualWrite(V3,doAmDat);
  Serial.println(doAmDat);

  lcd.setCursor(0,0);
  lcd.print("Nhiệt độ: ");
  lcd.print(t);
  lcd.print("C");

  lcd.setCursor(0,0);
  lcd.print("Độ ẩm đất: ");
  lcd.print(doAmDat);
  lcd.print("%");
  
  if (chedohoatdong == 0 && giamsat == 0){
    digitalWrite(relay,LOW);
    Blynk.virtualWrite(V4,0);
  }
  
  if (chedohoatdong == 1 && giamsat == 1){
    chedohoatdong = 0;
    Blynk.virtualWrite(V7,0);
    digitalWrite(relay,HIGH); 
  }

  if (giamsat == 0){
  if (chedohoatdong == 1){
    if (doAmDat < doAmBomTuoi){
      Blynk.virtualWrite(V4,1);
      Blynk.logEvent("Alert","Độ ẩm thấp! Đang tưới nước");
      digitalWrite(relay,HIGH);
    }

    else if (doAmDat >= doAmBomTuoi && doAmDat <= doAmTatBomTuoi){
      Blynk.virtualWrite(V4,0);
      digitalWrite(relay,LOW);
    }

    else if (doAmDat > doAmTatBomTuoi){
      Blynk.virtualWrite(V4,1);
      Blynk.logEvent("Alert2","Độ ẩm cao! Đang ngưng tưới nước");
      digitalWrite(relay,LOW);
    }
  }
  }else if (giamsat ==  1){
    digitalWrite(relay,HIGH);
  }
}
