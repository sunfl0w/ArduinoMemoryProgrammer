import argparse
import serial
import time

def main():
    programmingEnded = False
    operationEnded = False
    dataRequested = False
    currentIndex = 0

    # Parsing command line arguments
    argumentParser = argparse.ArgumentParser()
    argumentParser.add_argument("-r", "--baudRate", required=True)
    argumentParser.add_argument("-d", "--dataFile")
    argumentParser.add_argument("-p", "--serialPort", required=True)
    argumentParser.add_argument("-s", "--storageDevice", required=True)
    argumentParser.add_argument("-m", "--operatingMode", required=True)
    args = argumentParser.parse_args()

    baudRate = args.baudRate
    dataFile = args.dataFile
    serialPort = args.serialPort
    deviceType = args.storageDevice
    operatingMode = args.operatingMode

    print("Baudrate: " + baudRate)
    if dataFile is not None:
        print("Data file: " +  dataFile)
    print("Serial port: " + serialPort)
    print("Storage device: " + deviceType)
    print("Operating mode: " + operatingMode)

    if operatingMode == "Program":
        if dataFile == None:
            print("Data file not set")
            return

        print("Reading binary data file")

        data = []
        with open(dataFile, "rb") as file:
            data = file.read()

        print("Opening serial port")

        ser = serial.Serial(serialPort, baudRate, serial.EIGHTBITS);
        ser.flush()
        time.sleep(3)

        while not programmingEnded:
            line = ser.readline().decode('utf-8')
            line = line.replace('\r', '')
            line = line.replace('\n', '')
            print("Arduino: " + line)

            if line == "Send storage device type":
                print("Sending storage device type")
                ser.write(deviceType.encode())

            if line == "Send data size in bytes":
                print("Sending data size in bytes")
                print("Size is " + str(len(data)) + " bytes")
                ser.write(str(len(data)).encode())

            if line == "Send operating mode":
                print("Sending operating mode")
                ser.write(operatingMode.encode())

            if line == "Send next byte" and currentIndex < len(data):
                # Use for debugging
                # print("Sending: " + str(data[currentIndex]))

                ser.write(bytes([data[currentIndex]]))
                currentIndex = currentIndex + 1
                time.sleep(0.001) # sleep 1 ms to move data more slowly

            if line == "Goodbye":
                print("Closing connection")
                programmingEnded = True

        ser.close();
    elif operatingMode == "MemoryDump" or operatingMode == "Erase":
        print("Opening serial port")

        ser = serial.Serial(serialPort, baudRate, serial.EIGHTBITS);
        ser.flush()
        time.sleep(3)

        while not operationEnded:
            line = ser.readline().decode('utf-8')
            line = line.replace('\r', '')
            line = line.replace('\n', '')
            print("Arduino: " + line)

            if line == "Send storage device type":
                print("Sending storage device type")
                ser.write(deviceType.encode())

            if line == "Send data size in bytes":
                print("Sending data size in bytes")
                print("Size is " + str(8192) + " bytes")
                ser.write(str(8192).encode())
            
            if line == "Send operating mode":
                print("Sending operating mode")
                ser.write(operatingMode.encode())

            if line == "Goodbye":
                print("Closing connection")
                operationEnded = True

        ser.close();

if __name__ == "__main__":
    main()