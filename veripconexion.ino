#include <WiFi.h>

const char* ssid = "FLDSMDFR";
const char* password = "Lucas281+";

void setup() {
  Serial.begin(115200);
  delay(1000); // Esperar a que el monitor serie abra bien
  Serial.println("\n--- Iniciando prueba de WiFi ---");

  // 1. Borrar cualquier configuración vieja atorada en la memoria
  WiFi.disconnect(true);
  delay(1000);

  // 2. Intentar conectar (SIN WiFi.config, usando DHCP automático)
  Serial.print("Conectando a la red: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    intentos++;
    
    // Si falla después de 20 intentos (10 segundos)
    if (intentos >= 20) {
      Serial.println("\n[ERROR] El ESP32 no pudo conectarse.");
      Serial.println("Revisa la contraseña o asegúrate de que ASUS_D4 sea de 2.4GHz.");
      while(1); // Detener el programa
    }
  }

  // 3. ¡Éxito!
  Serial.println("\n¡Conectado exitosamente!");
  Serial.print("El modem ASUS nos asignó esta IP real: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // No necesitamos nada aquí para la prueba
}
