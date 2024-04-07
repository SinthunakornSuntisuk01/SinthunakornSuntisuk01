#include <TridentTD_LineNotify.h>

#define SSID        "--------------------"
#define PASSWORD    "--------------------"
#define LINE_TOKEN  "--------------------"

int measurePin = A0;
int ledPower = D2;    //Pin LED

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup() {
  Serial.begin(115200); 
  pinMode(ledPower, OUTPUT);
  
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  

  LINE.setToken(LINE_TOKEN);
}

void loop() {
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 3.3V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (3.3 / 1024);

  // linear eqaution taken from https://www.ab.in.th/b/59
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;

  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);

  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);

  dustDensity = dustDensity * 1000;


  if (dustDensity <= 0.00) {
    dustDensity = 0.00;
  }
  else if (dustDensity >= 201){
    LINE.notify(dustDensity);
    LINE.notify("The dust particles in the air are at an extremely high level."); 
  }
  else if (dustDensity >= 101){
    LINE.notify(dustDensity);
    LINE.notify("The dust particles in the air are at a high level.");
  }
  else if (dustDensity >= 51){
    LINE.notify(dustDensity);
    LINE.notify("The dust particles in the air are at a medium level.");
  } 
  
  Serial.print(" - Dust Density: ");
  Serial.print(dustDensity);
  Serial.println(" µg/m³");
  delay(1000);
}

