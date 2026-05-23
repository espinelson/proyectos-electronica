// Pin donde conectaste la salida del divisor de voltaje
const int sensorPin = 34; 

void setup() {
  // Iniciar la comunicación serial a 115200 baudios (estándar en ESP32)
  Serial.begin(115200);
  delay(1000);
  Serial.println("Iniciando prueba del sensor MPX5010DP...");
}

void loop() {
  // 1. Leer el valor bruto o crudo del ADC (va de 0 a 4095)
  int rawValue = analogRead(sensorPin);

  // 2. Calcular el voltaje que está llegando físicamente al pin 34 (Máximo 3.3V)
  float voltageAtPin = (rawValue / 4095.0) * 3.3;

  // 3. Calcular el voltaje real que está sacando el sensor ANTES del divisor.
  // Como usamos R1=10k y R2=20k, el factor de multiplicación es 1.5
  float sensorVoltage = voltageAtPin * 1.5;

  // 4. Mostrar los datos en el Monitor Serie
  Serial.print("Valor ADC: ");
  Serial.print(rawValue);
  Serial.print(" | Voltaje en ESP32: ");
  Serial.print(voltageAtPin, 2);
  Serial.print("V | Voltaje real del sensor: ");
  Serial.print(sensorVoltage, 2);
  Serial.println("V");

  // Esperar medio segundo antes de la siguiente lectura
  delay(500); 
}