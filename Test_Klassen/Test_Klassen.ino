#include <MeineKlassen.h>

// --- Zwei Instanzen mit unterschiedlichen Parametern ---
Blinker fastBlink(2, 150);   // schnelle LED
Blinker slowBlink(2, 2600);   // langsame LED an Pin 5

void setup() {
  // nichts weiter nötig – Konstruktor übernimmt pinMode()
}

void loop() {
  fastBlink.blink();
  //delay(200);
  slowBlink.blink();
  //delay(200);
}

