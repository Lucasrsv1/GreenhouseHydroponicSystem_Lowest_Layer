/*
 * Created by Lucas Rassilan Vilanova;
 * This software works only with the C# application of Greenhouse Hydroponic System,
 * it receives and sends data to the application using a SerialPort interface.
 * The comunication protocol can be found in the C# application documentation.
 * Before building this software in an Arduino board, check the baud rate specified below,
 * because this value must be used in the C# application connection configuration.
 * 
 */

// Baud Rate used in the comunication
const unsigned int baud = 9600;

// Protocol bytes
byte inputByte[6];

// Company's plan
int estatisticas = 0;
int controles = 0;

// Debug LED pin, set negative to disable debug
const unsigned int debugPin = 13;

/* Debug information code
 *  -2: don't using debug
 *  -1: system suspended
 *   0: system started
 *   1: command received
 *   2: answer sent
 *   3: data sent
 */
int debugCode = -2;

// Interval to toggle debug LED
int debugInterval = -1;

// Millis for the last debug toggle
unsigned long lastDebug = 0;

// Is system suspended?
bool suspended;

int Checksum (byte input[]) {
	int checksum = 0;
	for (int i = 0; i < 3; i++)
		checksum += input[i];

	checksum &= 0xFF;
	return checksum;
}

void setup() {
	Serial.begin(baud);
	suspended = true;

	// Start debug
	if (debugPin >= 0) {
		pinMode(debugPin, OUTPUT);
		SetDebugCode(-1);
	}
}

void loop() {
	if (suspended)
		return;

	// Toggle debug LED
	if (debugInterval > 0) {
		unsigned long time = millis();
		if (time - lastDebug >= debugInterval) {
			digitalWrite(debugPin, !digitalRead(debugPin));
			lastDebug = time;
		}
	}
}

void serialEvent () {
	// New command found
	if (Serial.available() == 6) {
		// Read Buffer
		for (int b = 0; b < 6; b++)
			inputByte[b] = Serial.read();

		// Check command start
		if (inputByte[0] == 16) {
			SetDebugCode(1);

			// Detect which command was received
			switch (inputByte[1]) {
				case 126:
					SetDebugCode(-1);
					suspended = true;
					break;
				case 127:
					byte bytesArray[3];
					bytesArray[0] = inputByte[2];
					bytesArray[1] = inputByte[3];
					bytesArray[2] = inputByte[4];

					int checksum;
					checksum = Checksum(bytesArray);
					Serial.println("Hello from Arduino running Greenhouse_Hydroponic_System_Lowest_Level [" + String(checksum) + "]");
					suspended = false;
					SetDebugCode(0);
					break;
				case 128:
					break;
				case 129:
					break;
				case 130:
					break;
				case 131:
					break;
				case 132:
					break;
				case 133:
					break;
				case 134:
					break;
			}

			// Clear message bytes
			for (int b2 = 0; b2 < 6; b2++)
				inputByte[b2] = 0;
		}
	}
}

void SetDebugCode (int code) {
	// Use LED to debug
	if (debugPin >= 0) {
		debugCode = code;
		switch (debugCode) {
			case -1:
				digitalWrite(debugPin, LOW);
				debugInterval = -1;
				break;
			case 0:
				digitalWrite(debugPin, HIGH);
				debugInterval = -1;
				break;
			case 1:
				digitalWrite(debugPin, LOW);
				debugInterval = 500;
				break;
			case 2:
				digitalWrite(debugPin, HIGH);
				debugInterval = 1000;
				break;
			case 3:
				break;
			default:
				break;
		}
	}
}