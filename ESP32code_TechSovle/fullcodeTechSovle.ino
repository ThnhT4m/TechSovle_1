#include <Arduino.h>
#include <WiFi.h>               //we are using the ESP32
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert your network credentials
#define WIFI_SSID "VTV38 LAU 2"
#define WIFI_PASSWORD "vtv382024"
// Insert Firebase project API Key
#define API_KEY "AIzaSyDM9Nmnq_w3HHWA2SHI2muGO7HkyW33udA"
// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://techsovlev1-default-rtdb.firebaseio.com/" 
#include <LiquidCrystal_I2C.h>
#define NTC_PIN 33  
#define SERIES_RESISTOR 10000  // Điện trở nối tiếp 10kΩ
#define NOMINAL_RESISTANCE 10000 // Điện trở danh định của NTC (thường là 10kΩ)
#define NOMINAL_TEMPERATURE 25   // Nhiệt độ danh định (°C)
#define B_COEFFICIENT 3950  // Hệ số B của cảm biến NTC
#define SDA_PIN 27   // 
#define SCL_PIN 14   // 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C của LCD (0x27 hoặc 0x3F)

int analogPin = 34;  // GPIO34 (ADC1) để đọc EC
int dtime = 500;
int raw = 0;
float Vin = 3.3;  // ESP32 chạy ở mức 3.3V
float Vout = 0;
float R1 = 1000;
float R2 = 0;
float buff = 0;
float avg = 0;
int samples = 5000;
const int potPin=35;
float ph;
float Value=0;
//Define Firebase Data object
FirebaseData fbdo;
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
String tempPath = "/Temp";          
String phPath = "/pH";
String ecPath = "/EC";
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;     
int data5base;                //since we are doing an anonymous sign in 
void setup() {
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
Wire.begin(SDA_PIN, SCL_PIN);
   // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
    
  pinMode(25, OUTPUT); // Cấp xung AC cho EC probe (GPIO25 )
  pinMode(26, OUTPUT); // Cấp xung AC cho EC probe (GPIO26 )
   pinMode(potPin,INPUT);
  Serial.begin(115200);
   delay(1000);
}
 float getPH(){
     Value= analogRead(potPin);
    float voltage=Value*(3.3/4095.0);
    ph=(3.3*voltage);
    return ph;
  }
  float getEC(){
       
   float tot = 0;
  for (int i =0; i<samples; i++) {
    digitalWrite(26,HIGH);
    digitalWrite(25,LOW);
    delayMicroseconds(dtime);
    digitalWrite(26,LOW);
    digitalWrite(25,HIGH);
    delayMicroseconds(dtime);
    raw = analogRead(analogPin);
    if(raw){
      buff = raw * Vin;
      Vout = (buff)/4095.0;
      buff = (Vin/Vout) - 1;
      R2= R1 * buff;
      //Serial.print("Vout: ");
      //Serial.println(Vout);
      //Serial.print("R2: ");
      //Serial.println(R2);
      tot = tot + R2;
    }
  }
  avg = tot/samples;
  return avg;
  }
  float getTemp(){
     int adc_value = analogRead(NTC_PIN); // Đọc ADC (0 - 4095)
    float voltage2 = adc_value * (3.3 / 4095.0); // Chuyển ADC thành điện áp
    float resistance = SERIES_RESISTOR * ((3.3 / voltage2) - 1); // Tính điện trở NTC

    // Tính nhiệt độ theo công thức Steinhart-Hart
    float steinhart;
    steinhart = resistance / NOMINAL_RESISTANCE;  // (R/Ro)
    steinhart = log(steinhart); // ln(R/Ro)
    steinhart /= B_COEFFICIENT; // 1/B * ln(R/Ro)
    steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart; // Nghịch đảo
    steinhart -= 273.15; // Chuyển sang °C
    return steinhart;
  }
void loop() {
     float var_ph = getPH();
    float var_EC = getEC();
    float var_temp = getTemp();
  

  Serial.print("The average resistance is: ");
  Serial.print(var_EC);
  Serial.println(" Ohm"); 
  Serial.print("DO PH: ");
  Serial.println(var_ph);
  
    Serial.print("Temperature: ");
    Serial.print(var_temp);
    Serial.println(" °C");
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(var_temp);
    lcd.print(" C  ");
     delay(2000);
    lcd.clear();
   
     lcd.setCursor(0, 0);
    lcd.print("Do PH: ");
    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(var_ph);
     delay(2000);
    lcd.clear();
   
     lcd.setCursor(0, 0);
    lcd.print("Do Dan Dien: ");
    lcd.setCursor(0, 1);
    lcd.print("EC: ");
    lcd.print(var_EC);
    lcd.print(" Omh ");
    delay(2000);
  Firebase.RTDB.setFloat (&firebaseData,tempPath,var_temp);
  Firebase.RTDB.setFloat (&firebaseData,phPath,var_ph);
  Firebase.RTDB.setFloat (&firebaseData,ecPath,var_EC);

}
