const int dataPin = 2;
const int clockPin = 3;
const int latchPin = 4;
const int memPin0 = 5;
const int memPin7 = 12;
const int writeEnablePin = 13;

const int timeoutTime = 3000;  //Timeout time in ms

int currentAddress = 0;
int timeSinceLastData = 0;

enum StorageDevice {
	Invalid,
	AT28CSeries,
	SST39SFSeries
};

void setIOPinsMode(uint8_t mode) {
	for (int pin = memPin0; pin <= memPin7; pin += 1) {
		pinMode(pin, mode);
	}
}

void setMemoryAddress(int address, bool enableChipOutput) {
	shiftOut(dataPin, clockPin, MSBFIRST, (address >> 8) | (!enableChipOutput << 7));
	shiftOut(dataPin, clockPin, MSBFIRST, address);

	digitalWrite(latchPin, LOW);
	digitalWrite(latchPin, HIGH);
	digitalWrite(latchPin, LOW);
}

void setOutputData(byte data) {
	for (int pin = memPin0; pin <= memPin7; pin += 1) {
		digitalWrite(pin, data & 1);
		data = data >> 1;
	}
}

void writeToMemory(StorageDevice storageDevice, int address, byte data) {
	if (storageDevice == StorageDevice::AT28CSeries) {
		setMemoryAddress(address, false);
		setIOPinsMode(OUTPUT);
		setOutputData(data);
		digitalWrite(writeEnablePin, LOW);
		delayMicroseconds(1);
		digitalWrite(writeEnablePin, HIGH);
		delayMicroseconds(10);
	} else if (storageDevice == StorageDevice::SST39SFSeries) {
		digitalWrite(writeEnablePin, HIGH);
		setIOPinsMode(OUTPUT);
		delay(10);

		setMemoryAddress(0x5555, false);
		setOutputData(0xaa);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(0x2aaa, false);
		setOutputData(0x55);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(0x5555, false);
		setOutputData(0xa0);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(address, false);
		setOutputData(data);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);
	} else {
		Serial.println("Unable to write to memory. Storage device not recognized");
	}
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

void clearMemory(StorageDevice storageDevice, int maxAddress) {
	if (storageDevice == StorageDevice::AT28CSeries) {
		for (int address = 0; address < maxAddress; address++) {
			writeToMemory(storageDevice, address, 0x00);
		}
	} else if (storageDevice == StorageDevice::SST39SFSeries) {
		digitalWrite(writeEnablePin, HIGH);
		setIOPinsMode(OUTPUT);
		delay(10);

		setMemoryAddress(0x5555, false);
		setOutputData(0xaa);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(0x2aaa, false);
		setOutputData(0x55);
		writeToMemory(storageDevice, 0x2aaa, 0x55);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(0x5555, false);
		setOutputData(0x80);
		writeToMemory(storageDevice, 0x5555, 0x80);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(0x5555, false);
		setOutputData(0xaa);
		writeToMemory(storageDevice, 0x5555, 0xaa);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(0x2aaa, false);
		setOutputData(0x55);
		writeToMemory(storageDevice, 0x2aaa, 0x55);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);

		setMemoryAddress(0x5555, false);
		setOutputData(0x10);
		writeToMemory(storageDevice, 0x5555, 0x10);
		digitalWrite(writeEnablePin, HIGH);
		digitalWrite(writeEnablePin, LOW);
		digitalWrite(writeEnablePin, HIGH);
		delay(10);
	} else {
		Serial.println("Unable to clear memory. Storage device not recognized");
	}
}

void printMemoryData(int maxAddress) {
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

StorageDevice getStorageDeviceToProgram() {
	Serial.println("Send storage device type");

	delay(1000);

	String deviceType = Serial.readString();
	StorageDevice deviceTypeEnum = StorageDevice::Invalid;

	if (deviceType == "AT28CSeries") {
		Serial.println("Device type " + deviceType + " set");;
		return StorageDevice::AT28CSeries;
	} else if (deviceType == "SST39SFSeries") {
		Serial.println("Device type " + deviceType + " set");
		return StorageDevice::SST39SFSeries;
	} else {
		Serial.println("Device type " + deviceType + " not recognized");
		return StorageDevice::Invalid;
	}
}

int getDataSizeToProgram() {
	Serial.println("Send data size in bytes");

	delay(1000);

	int numberOfBytes = Serial.readString().toInt();

	if (numberOfBytes <= 0) {
		Serial.println("Data size " + String(numberOfBytes) + " invalid");
		return 0;
	} else {
		Serial.println("Data size is " + String(numberOfBytes) + " bytes");
		return numberOfBytes;
	}
}

void setup() {
	Serial.begin(57600);

	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	digitalWrite(writeEnablePin, HIGH);
	pinMode(writeEnablePin, OUTPUT);

	StorageDevice storageDevice = getStorageDeviceToProgram();
	delay(1000);
	int bytesToProgram = getDataSizeToProgram();
	delay(1000);

	Serial.println("Clearing memory");
	clearMemory(storageDevice, bytesToProgram);
	Serial.println("Memory cleared");
	delay(1000);

	Serial.println("Commencing programming");
	Serial.println("Send next byte");
	delay(1000);

	while (timeSinceLastData < timeoutTime) {
		int now = millis();
		if (Serial.available() > 0 && currentAddress < bytesToProgram) {
			byte data = Serial.read();

			//Use for debugging
			Serial.print("Received:");
			Serial.println(data, DEC);

			writeToMemory(storageDevice, currentAddress, data);
			currentAddress++;
			timeSinceLastData = 0;
			Serial.println("Send next byte");
			delay(1000);
		} else {
			timeSinceLastData += millis() - now;
		}
	}

	Serial.println("No more data detected");
	Serial.println("Memory programmed");
	Serial.println("Memory dump");
	printMemoryData(bytesToProgram);
	Serial.println("Goodbye");
	Serial.println("Have a nice day :D");
}

void loop() {}