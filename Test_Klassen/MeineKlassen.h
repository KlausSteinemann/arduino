// --- Klassendefinition von Klaus Steinemann ---
class Blinker {
  private:
    int pin;
    int interval;

  public:
    // Konstruktor mit Parametern
    Blinker(int pinNumber, int blinkInterval) {
      pin = pinNumber;
      interval = blinkInterval;
      pinMode(pin, OUTPUT);
    }

    // Eine einzige Funktion
    void blink() {
      digitalWrite(pin, HIGH);
      delay(interval);
      digitalWrite(pin, LOW);
      delay(interval);
    }
};