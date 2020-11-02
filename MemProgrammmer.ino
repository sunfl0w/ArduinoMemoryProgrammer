const int dataPin = 2;
const int clockPin = 3;
const int latchPin = 4;

const int greenLED = 5;
const int yellowLED = 6;
const int redLED = 7;
const int writeEnablePin = 8;
const int button = 13;

const int maxAddress = 2 << 12;

uint16_t addressesToWrite[] = {0x0, 0x1};
uint8_t dataToWrite[] = {0xAA, 0xAA};

bool writingData = false;

void writeDataAtAddress(uint16_t address, uint8_t data) {
	shiftOut(dataPin, clockPin, MSBFIRST, (address >> 8));
	shiftOut(dataPin, clockPin, MSBFIRST, address);
	shiftOut(dataPin, clockPin, MSBFIRST, data);

    digitalWrite(latchPin, LOW);
	digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);

    digitalWrite(writeEnablePin, LOW);
    delayMicroseconds(1);
    digitalWrite(writeEnablePin, HIGH);
    delay(10);
}

void clearMemory() {
	digitalWrite(redLED, HIGH);
	for (int i = 0; i < maxAddress; i++) {
		digitalWrite(yellowLED, HIGH);
		writeDataAtAddress(i, 0);
		digitalWrite(yellowLED, LOW);
	}
	digitalWrite(redLED, LOW);
}

void writeDataToMemory(uint16_t addresses[], uint8_t data[], uint16_t dataLenght) {
	int maxDataLength = min(maxAddress, dataLenght);
	for (int i = 0; i < maxDataLength; i++) {
		digitalWrite(yellowLED, HIGH);
		writeDataAtAddress(addresses[i], data[i]);
		digitalWrite(yellowLED, LOW);
	}
}

void setup() {
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
    pinMode(writeEnablePin, OUTPUT);
    digitalWrite(writeEnablePin, HIGH);

	pinMode(greenLED, OUTPUT);
	pinMode(yellowLED, OUTPUT);
	pinMode(redLED, OUTPUT);
	pinMode(button, INPUT);
}

void loop() {
	if (digitalRead(button)) {
        digitalWrite(greenLED, LOW);
		clearMemory();
		writeDataToMemory(addressesToWrite, dataToWrite, 2);
		digitalWrite(greenLED, HIGH);
	}
}