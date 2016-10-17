
/**
 * ALDL160 - program to read 160 baud ALDL signal from GM ECM (e.g., 1227747)
 * Michael Shimniok - www.bot-thoughts.com
 */

const int AldlPin=28;
const int HiPin=33;
const int LoPin=34;

void setup() {
  // put your setup code here, to run once:
  pinMode(AldlPin, INPUT_PULLUP);
  pinMode(HiPin, OUTPUT);
  pinMode(LoPin, OUTPUT);
  pinMode(13, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(AldlPin), handlePinChange, FALLING);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
}

elapsedMillis loTime=0;
IntervalTimer timer;
uint8_t bitCount = 0;
uint16_t myByte = 0;

// 1227747 ALDL timing (approximate based on test):
//   total bit time: ~6250usec
//   0: Low for  ~370usec, high for ~5880usec
//   1: Low for ~4400usec, high for ~1850usec
void handlePinChange() {
  loTime = 0;
  timer.begin(doSample, 2000);
}

// Sample ALDL data portion of the bit
// and do stuff with it
//
void doSample() {
  timer.end();
  myByte <<= 1;
  if (digitalRead(AldlPin) == LOW) {
    // Bit is a 1
    digitalWrite(LoPin, HIGH);
    myByte |= 1;
  } else {
    // Bit is a 0
    digitalWrite(LoPin, LOW);
  }
  myByte &= 0x1ff;

  // Sync to the sync character
  // GM 160 ALDL sends a sync character starting with
  // a "1" start bit (all other bytes have "0" start bit)
  // followed by 0xff.
  if (myByte == 0x1ff) {
    digitalWrite(HiPin, HIGH);
    Serial.println("***** SYNC *****");
    bitCount = 0;
    myByte = 0;
  } else if (++bitCount >= 9) { // Received next 9 bits?
    // Mask off the low byte and print
    digitalWrite(HiPin, LOW);
    Serial.println(int(myByte & 0x0ff));
    bitCount = 0;
  }
}

