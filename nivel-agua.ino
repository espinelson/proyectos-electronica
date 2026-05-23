/*
 Protocolo Modbus TCP/IP con Node-RED y ESP32 + Monitor I2C 16x4
*/

#include <WiFi.h>              
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ModbusIP_ESP8266.h>  
#include <Modbus.h>

// --- Configuración de la Pantalla LCD 16x4 ---
LiquidCrystal_I2C lcd(0x27, 16, 4); 

// --- Configuración Pines GPIO ---
const byte PinNivel = 34; // Sensor de Agua (Calle)
const byte PinFlujo = 35; // Sensor de Flujo
const byte SalidaLed = 2; // Pin Led
#define PIN_RELE 26       // Pin del relevador de alarma/bomba
#define UMBRAL_AGUA 50    // % para activar relevador

// --- Modbus Registers Offsets (0-9999) ---
const int RegNivel = 0; 
const int RegFlujo = 1; 
const int RegLed = 1;

// Variables globales
bool StateLed;
ModbusIP mb;
unsigned long ts; 

// --- DIRECCIONES IP CORREGIDAS PARA TU MÓDEM ASUS ---
IPAddress ip(192, 168, 1, 9);     // La IP que te asignó el módem
IPAddress gateway(192, 168, 1, 1);  // La puerta de enlace de tu red
IPAddress subnet(255, 255, 255, 0);

// --- Caracteres Personalizados ---
byte bloque[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
byte palomita[8] = { B00000, B00001, B00011, B10110, B11100, B01000, B00000, B00000 };

void setup() {
  Serial.begin(115200);

  pinMode(PinNivel, INPUT);
  pinMode(PinFlujo, INPUT);
  pinMode(SalidaLed, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);

  digitalWrite(SalidaLed, HIGH); 
  digitalWrite(PIN_RELE, LOW);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, bloque);   
  lcd.createChar(2, palomita); 
  
  // PANTALLA 1: CONECTANDO
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Buscando WiFi...");
  lcd.setCursor(0, 1);
  lcd.print("SSID: modem totalplay");
  lcd.setCursor(0, 2);

  // Conexión WiFi
  WiFi.config(ip, gateway, subnet);
  WiFi.begin("FLDSMDFR", "Lucas281+");
  
  int puntos = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print("."); 
    puntos++;
    if(puntos > 15) { 
      lcd.setCursor(0, 2);
      lcd.print("                ");
      lcd.setCursor(0, 2);
      puntos = 0;
    }
  }

  // PANTALLA 2: ÉXITO
  Serial.println("\nWiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("CONEXION EXITOSA");
  lcd.setCursor(7, 1);
  lcd.write(2); // Palomita
  lcd.setCursor(3, 2);
  lcd.print("IP Asignada:");
  lcd.setCursor(2, 3);
  lcd.print(WiFi.localIP()); 

  // Modbus
  mb.server(); 
  mb.addHreg(RegNivel); 
  mb.addHreg(RegFlujo); 
  mb.addCoil(RegLed);   
  
  delay(3500); 
  lcd.clear(); 
}

void loop() {
   mb.task();  
  
   if (millis() > ts + 200) {
       
       unsigned int VarNivel = analogRead(PinNivel);
       unsigned int VarFlujo = analogRead(PinFlujo);

       unsigned int EnviarNivel = map(VarNivel, 0, 4095, 0, 200);
       unsigned int EnviarFlujo = map(VarFlujo, 0, 4095, 0, 30);

       int porcentajeAgua = map(VarNivel, 0, 4095, 0, 100);
       if (porcentajeAgua > 100) porcentajeAgua = 100;
       if (porcentajeAgua < 0) porcentajeAgua = 0;

       if (porcentajeAgua >= UMBRAL_AGUA) {
         digitalWrite(PIN_RELE, HIGH);
       } else {
         digitalWrite(PIN_RELE, LOW);
       }

       mb.Hreg(RegNivel, EnviarNivel);
       mb.Hreg(RegFlujo, EnviarFlujo);
       
       StateLed = mb.Coil(RegLed);
       digitalWrite(SalidaLed, StateLed); 

       // Actualizar LCD Principal
       lcd.setCursor(0, 0);
       lcd.print("Estado de Calle:");
       
       lcd.setCursor(0, 1);
       lcd.print("Agua: ");
       lcd.print(porcentajeAgua);
       lcd.print("%   "); 

       lcd.setCursor(0, 2); 
       int numBloques = map(porcentajeAgua, 0, 100, 0, 16); 
       for (int i = 0; i < 16; i++) {
         if (i < numBloques) lcd.write(1); 
         else lcd.print(" ");     
       }

       lcd.setCursor(0, 3);
       lcd.print("Raw:");
       lcd.print(VarNivel);
       lcd.print(" ");
       
       lcd.print("Rel:");
       if (porcentajeAgua >= UMBRAL_AGUA) lcd.print("ON ");
       else lcd.print("OFF");
       lcd.print("  ");

       Serial.print(EnviarNivel);
       Serial.print(",");
       Serial.print(EnviarFlujo);
       Serial.print(",");      
       Serial.println(StateLed);
       
       ts = millis();    
   }
}
