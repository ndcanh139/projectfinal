#define BLYNK_TEMPLATE_ID "TMPL6Kzcf8raF"
#define BLYNK_DEVICE_NAME "DATN"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);  
WiFiUDP udp;

char packetBuffer[255];
unsigned int localPort = 9999;

unsigned long blinkMillis = 0;
bool blinkState = false;
const unsigned long blinkInterval = 500; // 500ms

const char *ssid = "Wifichua";
const char *password = "12345678";

WidgetLED ledConnect(V0);
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
BlynkEdgent.begin();
  udp.begin(localPort);

  Serial.print(F("UDP Client : ")); Serial.println(WiFi.localIP());
  Serial.println();
  lcd.begin();
  // turn on LCD backlight                      
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("DO AN TOT NGHIEP");
  lcd.setCursor(1, 1);
  lcd.print("NGUYEN DINH CANH");
  lcd.setCursor(1, 2);
  lcd.print("NGO NGOC ANH");
  lcd.setCursor(1, 3);
  lcd.print("TRUONG DUY HOANG");
  delay(4000);
  lcd.clear();
}
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
 
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
String len="";
int sT;
int sH;
int sL;
int sA;
int sB;
int sC;

void updateDisplay() {
 // Kiểm tra các điều kiện
  bool isWarning = false;
  if (sT < 15 || sT > 37) {
    isWarning = true;
  }
  if (sH < 40 || sH > 70) {
    isWarning = true;
  }
  if (sB < 0 || sB > 5) {
    isWarning = true;
  }
  if (sA < 0 || sA > 10) {
    isWarning = true;
  }
  if (sC < 101100 || sC > 101500) {
    isWarning = true;
  }
  if (sL == 1) {
    isWarning = true;
  }

  // In các thông số lên màn hình LCD
  // hien thi nhiet do
  lcd.setCursor(0, 0);
  lcd.print("ND:");
  if (sT < 15 || sT > 37) {
    // Nhấp nháy nhiệt độ
    if (millis() - blinkMillis >= blinkInterval) {
      blinkMillis = millis();
      blinkState = !blinkState;
    }
    if (blinkState) {
      lcd.print(sT);
      lcd.write(0xDF); // Mã ASCII của ký tự độ C
      lcd.print("C");
    } else {
      lcd.print("       ");
    }
  } else {
      lcd.print(sT);
      lcd.write(0xDF); // Mã ASCII của ký tự độ C
      lcd.print("C");
  }

  //hien thi do am
  lcd.setCursor(11, 0);
  lcd.print("DA:");
  if (sH < 40 || sH > 70) {
    // Nhấp nháy độ ẩm
    if (millis() - blinkMillis >= blinkInterval) {
      blinkMillis = millis();
      blinkState = !blinkState;
    }
    if (blinkState) {
      lcd.print(sH);
      lcd.print("%");
    } else {
      lcd.print("      ");
    }
  } else {
    lcd.print(sH);
    lcd.print("%");
  }

  //hien thi tia UV
  lcd.setCursor(0, 1);
  lcd.print("UV:MUC ");
  if (sB < 0 || sB > 5) {
    // Nhấp nháy chỉ số tia UV
    if (millis() - blinkMillis >= blinkInterval) {
      blinkMillis = millis();
      blinkState = !blinkState;
    }
    if (blinkState) {
      lcd.print(sB);
    } else {
      lcd.print("       ");
    }
  } else {
    lcd.print(sB);
  }
 
// hien thi toc do gio
  lcd.setCursor(11, 1);
  lcd.print("Gio:");
  if (sA < 0 || sA > 10) {
    // Nhấp nháy chỉ số toc do gio
    if (millis() - blinkMillis >= blinkInterval) {
      blinkMillis = millis();
      blinkState = !blinkState;
    }
    if (blinkState) {
      lcd.print(sA);
      lcd.print("m/s");
    } else {
      lcd.print("     ");
    }
  } else {
      lcd.print(sA);
      lcd.print("m/s");
  }

// hien thi ap suat
  lcd.setCursor(0, 2);
  lcd.print("P:");
  if (sC < 101100 || sC > 101500) {
    // Nhấp nháy chỉ số ap xuat
    if (millis() - blinkMillis >= blinkInterval) {
      blinkMillis = millis();
      blinkState = !blinkState;
    }
    if (blinkState) {
      lcd.print(sC);
      lcd.print("Pa");
    } else {
      lcd.print("        ");
    }
  } else {
      lcd.print(sC);
      lcd.print("Pa");
  }

// hien thi mua
  lcd.setCursor(11, 2);
  if (sL == 1) {
    // Nhấp nháy mua
    if (millis() - blinkMillis >= blinkInterval) {
      blinkMillis = millis();
      blinkState = !blinkState;
    }
    if (blinkState) {
      lcd.print("DANG MUA ");
    } else {
      lcd.print("        ");
    }
  } else {
      lcd.print("KHONG MUA");
  }
  
  // Hiển thị cảnh báo
  lcd.setCursor(0, 3);
  if (isWarning) {
    lcd.print("CANH BAO            ");
  } else {
    lcd.print("OK                  ");
  }
}

void loop() {
  BlynkEdgent.run();
  int packetSize = udp.parsePacket();
  Serial.print(" Received packet from : "); Serial.println(udp.remoteIP());
  Serial.print(" Size : "); Serial.println(packetSize);
  Serial.print(" Data : ");
  
  if (packetSize) {
    len = udp.read(packetBuffer, 255);
   
  String t = getValue(packetBuffer, ',', 0); 
  String h = getValue(packetBuffer, ',', 1); 
  String v = getValue(packetBuffer, ',', 2);
  String a = getValue(packetBuffer, ',', 3); 
  String b = getValue(packetBuffer, ',', 4); 
  String c = getValue(packetBuffer, ',', 5);
  sT = t.toInt(); 
  sH = h.toInt();
  sL = v.toInt();
  sA = a.toInt(); 
  sB = b.toInt();
  sC = c.toInt();
  Serial.println(packetBuffer);
  Serial.println(sT);
  Serial.println(sH);
  Serial.println(sL);
  Serial.println(sA);
  Serial.println(sB);
  Serial.println(sC);
  }
  Serial.println("\n");

    if(ledConnect.getValue()){
      ledConnect.off();
    }else{
      ledConnect.on();
    }
  Blynk.virtualWrite(V1,sT);
  Blynk.virtualWrite(V2,sH);
  Blynk.virtualWrite(V3,sL);
  Blynk.virtualWrite(V4,sA);
  Blynk.virtualWrite(V5,sB);
  Blynk.virtualWrite(V6,sC);
  updateDisplay();
  delay(1000); // Cập nhật hiển thị mỗi 1 giây
}
