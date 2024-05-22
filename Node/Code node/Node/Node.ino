#include <WiFi.h>
#include <WiFiUdp.h>
// Include required libraries
#include <DHT.h>
 DHT dht(13, DHT11);// Cấu hình chân DHT11
unsigned long timesBlinkLed=millis();
int UVOUT = 34; //Output from the sensor
int REF_3V3 = 35; //Điện áp tham chiếu 3V3
int pulse = 14; // brown 5v, yeloww G, blue 13
unsigned long t_up, t_dow, T, f;
WiFiUDP udp;

char packetBuffer[255];
unsigned int localPort = 9999;

const char *ssid = "Wifichua";
const char *password = "12345678";
  int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;
  // Đọc cảm biến mưa
  for (int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;
  return (runningValue);
}
// hàm lấy tỉ lệ
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int mua;
void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("LoRa Sender");// In ra đã nhận
  // Cấu hình chân vào ra
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  pinMode(pulse, INPUT);
   WiFi.begin(ssid, password);
  udp.begin(localPort);
  Serial.print(F("UDP Client : ")); Serial.println(WiFi.localIP());
}

void loop() {
    udp.beginPacket("192.168.137.210", 9999);
  String buf="";
  int Humidity = dht.readHumidity();// đọc độ ẩm
  int Temperature = dht.readTemperature();// đọc nhiệt độ
   int uvLevel = averageAnalogRead(UVOUT);// Hàm tính toán UV
  int refLevel = averageAnalogRead(REF_3V3);// ĐIện áp tham chiêus
  float outputVoltage = 3.3 / refLevel * uvLevel;
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0);// Lấy tỉ lệ //Convert the voltage to a UV intensity level
  int UV;
  int p= 101290;
  //read sensor
  int f = 0, v = 0; // Biến
  t_up = pulseIn(pulse, HIGH, 1000000); //Hàm trả về mức cao
  t_dow = pulseIn(pulse, LOW, 1000000); //Hàm trả về mức thấp
  T = t_up + t_dow ;// Chu kỳ
  if (T == 0) {
    v = 0;
    f = 0;}
  else if(T>0) {
   f = 1000000/T;
   v = f*0.83 ;
  }
  if(uvIntensity<0)
  {
    UV=0;
  }
  else if(uvIntensity>0)
  {
    UV = uvIntensity;
  }
  // Hàm gửi dữ liệu
  Serial.print("Temperature: ");
  Serial.print(Temperature);
  Serial.write("\xC2\xB0"); //The Degree symbol
  Serial.println("C");
  Serial.print("Humidity: ");
  Serial.print(Humidity);
  Serial.println("%");
  Serial.print("T: ");
  Serial.print(T);
  Serial.print("RainD: ");
  Serial.println(digitalRead(23));
  Serial.print("mua ");
  Serial.println(mua);
  Serial.print("van toc (m/s) = ");
  Serial.println(v);
  Serial.print("UV Intensity (mW/cm^2): ");
  Serial.println(UV);
  Serial.print("Ap suat: ");
  Serial.println(p);
  Serial.println();
  // Add to structured data object
  buf = buf + Temperature + "," + Humidity+"," +mua + "," +v+"," + UV+"," + p;
  if(digitalRead(23)==0)
  {
    mua=1;
  }
  if(digitalRead(23)==1)
  {
    mua=0;
  }
  Serial.print("[Client Connected] ");
  Serial.println(WiFi.localIP());
  udp.println(buf);
  //udp.write("\r\n");
  udp.endPacket();
  buf = "";
}
