// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 5 // Use the pin 5 you specified for reset
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

#define USER_MESSAGE_SIZE 242

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Buffer for storing user input
char userMessage[USER_MESSAGE_SIZE];

// Blinky on receipt
#define LED 13

void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(115200);
  delay(100);
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("#FAIL");
    while (1);
  }
  Serial.println("#BEGIN");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    Serial.println((char*)buf);
    
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      //Serial.println((char*)buf);
      
      // Send a reply
      readUserInput(); // Read user input into userMessage
      if (strlen(userMessage) > 0) {
        rf95.send((uint8_t *)userMessage, strlen(userMessage) + 1);
        rf95.waitPacketSent();
        digitalWrite(LED, LOW);
      }
      else {
        //Serial.println("Fail");
      }
    }
    else
    {
      //Serial.println("Receive failed");
    }
  }
}


void readUserInput() {
    unsigned long startTime = millis();
    int index = 0;

    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || index >= (USER_MESSAGE_SIZE - 1)) {
                break;
            }
            userMessage[index] = c;
            index++;
            startTime = millis();  // Reset the timeout timer when new input is received
        }

        // Check for timeout (5 seconds in this example)
        if (millis() - startTime > 9000) {
            // Set userMessage to the default JSON data if no input within 5 seconds
            const char defaultJson[] = "[{\"h\":12.66, \"l\":9.29, \"r\":\"Yes\"},{\"h\":18.79, \"l\":8.28, \"r\":\"No\"},{\"h\":17.49, \"l\":12.46, \"r\":\"No\"},{\"h\":15.62, \"l\":10.92, \"r\":\"No\"},{\"h\":19.85, \"l\":11.27, \"r\":\"Yes\"}]";
            strncpy(userMessage, defaultJson, sizeof(userMessage) - 1);
            userMessage[sizeof(userMessage) - 1] = '\0';  // Null-terminate the string
            return;
        }
    }

    userMessage[index] = '\0'; // Null-terminate the string
}
