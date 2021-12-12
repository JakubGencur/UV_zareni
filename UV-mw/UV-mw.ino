//Tento program vam bude vracet hodnoty v mw/cm^2

// Arduino senzor UVA a UVB záření

// nastavení čísel propojovacích pinů senzoru
#define pinOut A0
#define pinRef3V3 A1

void setup() {
  // zahájení komunikace po sériové lince rychlostí 9600 baud
  Serial.begin(9600);
  // nastavení propojovacích pinů jako vstupních
  pinMode(pinOut, INPUT);
  pinMode(pinRef3V3, INPUT);
}

void loop() {
  // načtení analogové hodnoty z obou vstupů,
  // pro načtení volaný podprogram, který vrátí
  // průměr za 8 měření
  int hodnotaUV = prumerAnalogRead(pinOut);
  int hodnotaRef3V3 = prumerAnalogRead(pinRef3V3);
  // přepočet načteného napětí z UV senzoru a napětí
  // z 3,3 V Arduina pro přesnou hodnotu z UV senzoru
  float napetiOutUV = 3.3 / hodnotaRef3V3 * hodnotaUV;
  // přepočet napětí z UV senzoru na intenzitu UV záření
  float intenzitaUV = prevodNapetiIntenzita(napetiOutUV, 0.96, 2.8, 0.0, 15.0);
  // vytištění informací po sériové lince
  Serial.print("ML8511 napeti: ");
  Serial.print(napetiOutUV);
  Serial.print("V, UV Intenzita (mW/cm^2): ");
  Serial.println(intenzitaUV);
  // volitelná pauza 1 sekunda před novým měřením
  delay(1000);
}
// podprogram pro provedení 8 měření a vrácení průměrné hodnoty
int prumerAnalogRead(int pinToRead) {
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

// podprogram pro převod naměřené hodnoty
// z desetinného čísla na UV intenzitu
float prevodNapetiIntenzita(float x, float in_min, float in_max,
                float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
