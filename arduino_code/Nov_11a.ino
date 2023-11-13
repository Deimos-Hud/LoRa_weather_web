#include <SPI.h>
#include <RH_RF95.h>

// LoRa Pins
#define RFM95_CS 10
#define RFM95_RST 5
#define RFM95_INT 2

// LoRa Frequency
#define RF95_FREQ 915.0

// Size of the userMessage buffer
#define USER_MESSAGE_SIZE 242

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Buffer for storing user input
char userMessage[USER_MESSAGE_SIZE];

void setup() {
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    while (!Serial);
    Serial.begin(9600);
    delay(100);

    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!rf95.init()) {
        Serial.println("LoRa radio init failed");
        while (1);
    }
    Serial.println("LoRa radio init OK!");

    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed");
        while (1);
    }

    rf95.setTxPower(23, false);
}

void loop() {
    readUserInput(); // Read user input into userMessage

    if (strlen(userMessage) > 0) {

        rf95.send((uint8_t *)userMessage, strlen(userMessage) + 1);
        rf95.waitPacketSent();

        memset(userMessage, 0, USER_MESSAGE_SIZE); // Clear the userMessage for next use

        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.waitAvailableTimeout(3000)) { // Wait for a reply
            if (rf95.recv(buf, &len)) {
                Serial.print("Got reply: ");
                Serial.println((char*)buf);
                Serial.print("RSSI: ");
                Serial.println(rf95.lastRssi(), DEC);
            } else {
                Serial.println("Receive failed");
            }
        } else {
            Serial.println("No reply, is there a listener around?");
        }
    }
}

void readUserInput() {
    int index = 0;
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || index >= (USER_MESSAGE_SIZE - 1)) {
                break;
            }
            userMessage[index] = c;
            index++;
        }
    }
    userMessage[index] = '\0'; // Null-terminate the string
}
