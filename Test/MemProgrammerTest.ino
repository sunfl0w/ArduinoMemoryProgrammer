const int dataPin = 2;
const int clockPin = 3;
const int latchPin = 4;
const int memPin0 = 5;
const int memPin7 = 12;
const int writeEnablePin = 13;

const int maxAddress = 2 << 8;

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
	setIOPinsMode(OUTPUT);

  	for (int pin = memPin0; pin <= memPin7; pin += 1) {
    	digitalWrite(pin, data & 1);
    	data = data >> 1;
  	}

	digitalWrite(writeEnablePin, LOW);
	delayMicroseconds(1);
	digitalWrite(writeEnablePin, HIGH);
	delay(10);
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
		writeToMemory(address, 0);
	}
}

void printMemoryData() {
	for (int base = 0; base <= 255; base += 16) {
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
	//Write to memory here!
	byte data[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8 };

	for (int address = 0; address < sizeof(data); address++) {
		writeToMemory(address, data[address]);
	}
	Serial.println("Memory programmed");

	printMemoryData();
}

void loop() {}