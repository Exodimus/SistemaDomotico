#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Estructura para los pines
struct Pins {
  const int lightSensor = A1;
  const int relayLight = 3;
  const int tempSensor = A0;
  const int relayFan = 4;
  const int motionSensor = 8;
  const int buzzer = 5;
  const int soilMoistureSensor = A3;
  const int relayPump = 2;
  const int buttonUp = 6;
  const int buttonDown = 7;
  const int buttonEnter = 9;
} pins;

// Estructura para los umbrales
struct Thresholds {
  const int light = 500;
  const float temp = 25.0;
  const int moisture = 700;
} thresholds;

// Estructura para los estados
struct States {
  int currentOption = 0;
  int light = 2;
  int alarm = 2;
  int irrigation = 2;
  int ventilation = 2;
} states;

String menuOptions[] = {
  "Luces",
  "Alarma",
  "Riego",
  "Ventilacion"
};

// Estados de los botones
bool buttonUpPressed = true;
bool buttonDownPressed = true;
bool buttonEnterPressed = true;

// Indicadores de actividad del sistema
bool lightsOn = false;
bool alarmActive = false;
bool irrigationActive = false;
bool ventilationActive = false;

void setup() {
  // Configuración de pines usando struct
  pinMode(pins.lightSensor, INPUT);
  pinMode(pins.tempSensor, INPUT);
  pinMode(pins.relayLight, OUTPUT);
  pinMode(pins.relayFan, OUTPUT);
  pinMode(pins.motionSensor, INPUT);
  pinMode(pins.buzzer, OUTPUT);
  pinMode(pins.soilMoistureSensor, INPUT);
  pinMode(pins.relayPump, OUTPUT);
  pinMode(pins.buttonUp, INPUT_PULLUP);
  pinMode(pins.buttonDown, INPUT_PULLUP);
  pinMode(pins.buttonEnter, INPUT_PULLUP);

  // Inicialización de la pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("¡Bienvenido!");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);

  displayMenu();
}

void loop() {
  // Manejo del botón "Arriba"
  if (digitalRead(pins.buttonUp) == LOW && !buttonUpPressed) {
    buttonUpPressed = true;
    states.currentOption = (states.currentOption - 1 + 4) % 4;
    displayMenu();
    delay(50);
  } else if (digitalRead(pins.buttonUp) == HIGH) {
    buttonUpPressed = false;
  }

  // Manejo del botón "Abajo"
  if (digitalRead(pins.buttonDown) == LOW && !buttonDownPressed) {
    buttonDownPressed = true;
    states.currentOption = (states.currentOption + 1) % 4;
    displayMenu();
    delay(50);
  } else if (digitalRead(pins.buttonDown) == HIGH) {
    buttonDownPressed = false;
  }

  // Manejo del botón "Enter"
  if (digitalRead(pins.buttonEnter) == LOW && !buttonEnterPressed) {
    buttonEnterPressed = true;
    changeState(states.currentOption);
    displayMenu();
    delay(50);
  } else if (digitalRead(pins.buttonEnter) == HIGH) {
    buttonEnterPressed = false;
  }

  // Control de las luces
  if (states.light == 1) {
    digitalWrite(pins.relayLight, HIGH);
    lightsOn = true;
  } else if (states.light == 0) {
    digitalWrite(pins.relayLight, LOW);
    lightsOn = false;
  } else if (states.light == 2) {
    int lightLevel = analogRead(pins.lightSensor);
    if (lightLevel > thresholds.light) {
      digitalWrite(pins.relayLight, HIGH);
      if (!lightsOn) {
        lightsOn = true;
        displayMessage("Sistema Luces", "Activado");
      }
    } else {
      digitalWrite(pins.relayLight, LOW);
      if (lightsOn) {
        lightsOn = false;
        displayMessage("Sistema Luces", "Desactivado");
      }
    }
  }

  // Control de la alarma
  if (states.alarm == 1) {
    tone(pins.buzzer, 1000);
    alarmActive = true;
  } else if (states.alarm == 0) {
    noTone(pins.buzzer);
    alarmActive = false;
  } else if (states.alarm == 2) {
    int motionDetected = digitalRead(pins.motionSensor);
    if (motionDetected == HIGH) {
      tone(pins.buzzer, 1000);
      if (!alarmActive) {
        alarmActive = true;
        displayMessage("Sistema Alarma", "Activado");
      }
    } else {
      noTone(pins.buzzer);
      if (alarmActive) {
        alarmActive = false;
        displayMessage("Sistema Alarma", "Desactivado");
      }
    }
  }

  // Control del riego
  if (states.irrigation == 1) {
    digitalWrite(pins.relayPump, HIGH);
    irrigationActive = true;
  } else if (states.irrigation == 0) {
    digitalWrite(pins.relayPump, LOW);
    irrigationActive = false;
  } else if (states.irrigation == 2) {
    int moistureLevel = analogRead(pins.soilMoistureSensor);
    if (moistureLevel < thresholds.moisture) {
      digitalWrite(pins.relayPump, HIGH);
      if (!irrigationActive) {
        irrigationActive = true;
        displayMessage("Sistema Riego", "Activado");
      }
    } else {
      digitalWrite(pins.relayPump, LOW);
      if (irrigationActive) {
        irrigationActive = false;
        displayMessage("Sistema Riego", "Desactivado");
      }
    }
  }

  // Control de la ventilación
  if (states.ventilation == 1) {
    digitalWrite(pins.relayFan, HIGH);
    ventilationActive = true;
  } else if (states.ventilation == 0) {
    digitalWrite(pins.relayFan, LOW);
    ventilationActive = false;
  } else if (states.ventilation == 2) {
    int tempReading = analogRead(pins.tempSensor);
    float tempCelsius = (tempReading * 5.0 / 1023.0) * 100;
    if (tempCelsius > thresholds.temp) {
      digitalWrite(pins.relayFan, HIGH);
      if (!ventilationActive) {
        ventilationActive = true;
        displayMessage("Sistema Ventilacion", "Activado");
      }
    } else {
      digitalWrite(pins.relayFan, LOW);
      if (ventilationActive) {
        ventilationActive = false;
        displayMessage("Sistema Ventilacion", "Desactivado");
      }
    }
  }

  delay(50); // Retardo global para estabilizar
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuOptions[states.currentOption]);
  lcd.setCursor(0, 1);
  switch (states.currentOption) {
    case 0:
      lcd.print(stateText(states.light));
      break;
    case 1:
      lcd.print(stateText(states.alarm));
      break;
    case 2:
      lcd.print(stateText(states.irrigation));
      break;
    case 3:
      lcd.print(stateText(states.ventilation));
      break;
  }
}

void changeState(int option) {
  switch (option) {
    case 0:
      states.light = (states.light + 1) % 3;
      break;
    case 1:
      states.alarm = (states.alarm + 1) % 3;
      break;
    case 2:
      states.irrigation = (states.irrigation + 1) % 3;
      break;
    case 3:
      states.ventilation = (states.ventilation + 1) % 3;
      break;
  }
}

String stateText(int state) {
  switch (state) {
    case 0: return "Apagado";
    case 1: return "Encendido";
    case 2: return "Auto";
  }
  return "";
}

void displayMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  delay(2000);
  displayMenu();
}