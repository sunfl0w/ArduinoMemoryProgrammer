import argparse
import serial
import time

def main():
    programmingEnded = False
    dataRequested = False
    currentIndex = 0

    # Parsing command line arguments
    argumentParser = argparse.ArgumentParser()
    argumentParser.add_argument("-r", "--baudRate", required=True)
    argumentParser.add_argument("-d", "--dataFile", required=True)
    argumentParser.add_argument("-p", "--serialPort", required=True)
    args = argumentParser.parse_args()

    baudRate = args.baudRate
    dataFile = args.dataFile
    serialPort = args.serialPort

    print("Baudrate: " + baudRate)
    print("Data file: " +  dataFile)
    print("Serial port: " + serialPort)

    print("Reading binary data file")

    data = []
    with open(dataFile, "rb") as file:
        data = file.read()

    print("Opening serial port")

    ser = serial.Serial(serialPort, baudRate, serial.EIGHTBITS);
    ser.flush()

    while(not programmingEnded):
        line = ser.readline().decode('utf-8')
        line = line.replace('\r', '')
        line = line.replace('\n', '')
        print("Arduino: " + line);

        if(line == "Send data"):
            print("Sending data over serial")
            dataRequested = True

        if(line == "Goodbye"):
            print("Closing connection")
            programmingEnded = True

        if(dataRequested and currentIndex < len(data)):
            # Use for debugging
            # print("Sending: " + str(data[currentIndex]))

            ser.write(bytes([data[currentIndex]]))
            currentIndex = currentIndex + 1
            time.sleep(0.01) # sleep 10 ms to move data more slowly

    ser.close();

if __name__ == "__main__":
    main()