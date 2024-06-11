#include <SoftwareSerial.h>

const uint32_t kBaudRates[] = {9600, 19200, 38400, 57600, 115200};
const int kNumBaudRates = sizeof(kBaudRates) / sizeof(kBaudRates[0]);

const int kBuzzerPin = 9;
const int kRxPin = 2;
const int kStabilizationTime = 2000;
const int kMinValidChars = 3; // Minimum number of valid ASCII characters to confirm detection

SoftwareSerial uart(kRxPin, -1); // RX, TX (TX is not used)

void setup()
{
    pinMode(kBuzzerPin, OUTPUT);
    Serial.begin(115200);
    Serial.println("UART Discovery Module Initialized");
}

void loop()
{
    size_t i = 0;
    for (; i < kNumBaudRates; i++) {
        uart.end();
        delay(100);

        uart.begin(kBaudRates[i]);
        Serial.print("Testing baud rate: ");
        Serial.println(kBaudRates[i]);

        delay(kStabilizationTime); // Wait for stabilization

        int validCharCount = 0;
        unsigned long startMillis = millis();
        while (millis() - startMillis < kStabilizationTime) {
            if (uart.available()) {
                char receivedChar = uart.read();

                // Check for ASCII chars..
                if (receivedChar >= 32 && receivedChar <= 126) {
                    validCharCount++;
                    Serial.print("Received: ");
                    Serial.println(receivedChar);
                    if (validCharCount >= kMinValidChars) {
                        beep();
                        Serial.print("Valid baud rate detected: ");
                        Serial.println(kBaudRates[i]);
                        delay(3000); // Wait for a few seconds before continuing
                        break;
                    }
                }
            }
        }
        if (validCharCount >= kMinValidChars) {
            break;
        }
    }
    // Reset to the initial baud rate for stability
    uart.end();
    delay(100);
    Serial.println("Baud Rate: " + String(kBaudRates[i]));

    for (;;) ;
}

static void beep()
{
    delay(100); // Short delay before the beep
    tone(kBuzzerPin, 1000, 100); // Generate a beep sound
    delay(100); // Short delay after the beep
}
