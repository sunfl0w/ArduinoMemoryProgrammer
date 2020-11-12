const int dataPin = 2;
const int clockPin = 3;
const int latchPin = 4;
const int memPin0 = 5;
const int memPin7 = 12;
const int writeEnablePin = 13;

const int maxAddress = 2048;   //Set this accordingly
const int timeoutTime = 3000;  //Timeout time in ms

int currentAddress = 0;
int timeSinceLastData = 0;

void setIOPinsMode(uint8_t mode) {
	for (int pin = memPin0; pin <= memPin7; pin += 1) {
		pinMode(pin, mode);
	}
}

void setMemoryAddress(int address, bool enableOutput) {
	shiftOut(dataPin, clockPin, MSBFIRST, (address >> 8) | (!enableOutput << 7));
	shiftOut(dataPin, clockPin, MSBFIRST, address);

	digitalWrite(latchPin, LOW);
	digitalWrite(latchPin, HIGH);
	digitalWrite(latchPin, LOW);
}

void writeToMemory(int address, byte data) {
	setMemoryAddress(address, false);
	setIOPinsMode(OUTPUT);

	for (int pin = memPin0; pin <= memPin7; pin += 1) {
		digitalWrite(pin, data & 1);
		data = data >> 1;
	}

	digitalWrite(writeEnablePin, LOW);
	delayMicroseconds(1);
	digitalWrite(writeEnablePin, HIGH);
	delayMicroseconds(10);
}

byte readFromMemory(int address) {
	setIOPinsMode(INPUT);
	setMemoryAddress(address, true);

	byte data = 0;
	for (int pin = memPin7; pin >= memPin0; pin--) {
		data = (data << 1) + digitalRead(pin);
	}

	return data;
}

void clearMemory() {
	for (int address = 0; address < maxAddress; address++) {
		writeToMemory(address, 0x00);
	}
}

void printMemoryData() {
	for (int base = 0; base < maxAddress; base += 16) {
		byte data[16];
		for (int offset = 0; offset <= 15; offset += 1) {
			data[offset] = readFromMemory(base + offset);
		}

		char buf[100];
		sprintf(buf, "%08d:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
				base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
				data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

		Serial.println(buf);
	}
}

void setup() {
	Serial.begin(57600);

	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	digitalWrite(writeEnablePin, HIGH);
	pinMode(writeEnablePin, OUTPUT);

	Serial.println("Clearing memory");
	clearMemory();
	Serial.println("Memory cleared");

	Serial.println("Programming memory");
	Serial.println("Send data");

	while (timeSinceLastData < timeoutTime) {
		int now = millis();
		if (Serial.available() > 0 && currentAddress < maxAddress) {
			byte data = Serial.read();

			//Use for debugging
			//Serial.print("Received:");
			//Serial.println(data, DEC);

			writeToMemory(currentAddress, data);
			currentAddress++;
			timeSinceLastData = 0;
		} else {
			timeSinceLastData += millis() - now;
		}
	}

	Serial.println("No more data detected");
	Serial.println("Memory programmed");
	Serial.println("Memory dump");
	printMemoryData();
	Serial.println("Goodbye");
}

void loop() {}