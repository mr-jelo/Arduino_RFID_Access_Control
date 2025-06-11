#include <virtuabotixRTC.h> // Required for RTC module
#include <SPI.h>            // Required for MFRC522 (RFID reader)
#include <MFRC522.h>        // RFID reader library
#include <Wire.h>           // Required for I2C communication (for LCD)
#include <LiquidCrystal_I2C.h> // I2C LCD library

// --- RTC Pin Definitions ---
// Initialize the RTC module with data pin, clock pin, and chip select pin
// IMPORTANT: VERIFY THESE DIGITAL PIN NUMBERS AGAINST YOUR PHYSICAL WIRING!
virtuabotixRTC myRTC(6, 7, 8); // Example: Data Pin 6, Clock Pin 7, CS Pin 8

// --- RFID Reader Pin Definitions ---
// These pins are for the MFRC522 RFID reader.
// RST_PIN: Reset pin for the MFRC522
// SDA_PIN: SPI Slave Select (SS) pin for the MFRC522
#define RST_PIN 9
#define SDA_PIN 10

// --- Component Pin Definitions ---
// VERIFY THESE DIGITAL PIN NUMBERS AGAINST YOUR PHYSICAL WIRING!
// Connect each LED's anode (long leg) to its respective digital pin
// via a current-limiting resistor (e.g., 220-330 Ohm).
// Connect the LED's cathode (short leg) to Arduino GND.
const int BLUE_LED = A0;    // Blue LED for "waiting for tag" state
const int GREEN_LED = A1;   // Green LED for "Access Granted"
const int RED_LED = A2;     // Red LED for "Access Denied"
const int BUZZER_PIN = A3;  // Buzzer connected to Digital Pin A3

// --- MFRC522 Object Initialization ---
MFRC522 mfrc522(SDA_PIN, RST_PIN);

// --- LCD I2C Object Initialization ---
// Parameters: I2C address, columns, rows
// Common I2C addresses are 0x27 or 0x3F. If your LCD doesn't work, try 0x3F,
// or run an I2C scanner sketch to find the correct address.
LiquidCrystal_I2C lcd(0x27, 16, 2); // Assuming a 16x2 LCD with address 0x27

// --- RFID User Data Structure ---
// Defines a structure to hold an RFID UID and the corresponding user's name parts, including Student ID.
struct RFIDUser {
  String uid;             // Stores the Unique Identifier of the RFID tag
  String studentID;       // New: Stores the Student ID
  String lastName;        // Stores the last name
  String firstName;       // Stores the first name
  String middleInitial;   // Stores the middle initial (can be empty string if none)
  bool isInside;          // State: true if currently "inside", false if "outside"
};

// --- Authorized Users Array ---
// IMPORTANT: Replace these example UIDs, Student IDs, and names with your actual data.
RFIDUser authorizedUsers[] = {
  {"AC 80 A2 D3", "2023-121343", "Dela Cruz", "Juan", "J.", false}, 
  {"A6 59 18 9E", "2023-987654", "TEJADA", "Paul Andrei", "B.", false}, 
  {"22 3D 05 4B", "2023-112233", "ABUED", "Jose Anjelo", "R.", false}
};

// --- Custom Tone Function ---
// Generates a square wave on the specified pin to make a sound.
void tone(int pin, int frequency, int duration) {
  long toggleTime = 1000000L / (frequency * 2);
  long numToggles = (long)duration * 1000L / toggleTime;
  for (long i = 0; i < numToggles; i++) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(toggleTime);
    digitalWrite(pin, LOW);
    delayMicroseconds(toggleTime);
  }
}

// --- Setup Function ---
// Runs once when the Arduino starts or resets.
void setup() {
  Serial.begin(9600);       // Initialize serial communication for debugging
  SPI.begin();              // Initialize SPI bus for RFID reader
  mfrc522.PCD_Init();       // Initialize MFRC522 RFID reader

  // Initialize the LCD
  lcd.init();               // Initializes the LCD module
  lcd.backlight();          // Turns on the LCD backlight
  lcd.clear();              // Clears any text on the LCD screen

  // Set all LED and Buzzer pins as OUTPUTs
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Ensure all LEDs and the buzzer are initially OFF (LOW state)
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Set the current date and time for the RTC.
  // Format: seconds, minutes, hours, day of the week, day of the month, month, year
  // day of the week: 1 = Sunday, 2 = Monday, ..., 7 = Saturday
  // myRTC.setDS1302Time(ss, mm, hh, WW, DD, MM,YYYY);
  // Current time: Thursday, June 13, 2025 at 12:45:00 AM
  myRTC.setDS1302Time(00, 45, 00, 5, 13, 6, 2025); 
  
  // Print CSV Header to Serial Monitor
  // Updated to include "StudentID" column
  Serial.println("Logs,RFID Tag,StudentID,Last Name,First Name,Middle Initial,Authorized (Yes/No),Entry/Exit,Date,Time");

  // Set the initial state: Blue LED ON and prompt message on LCD
  digitalWrite(BLUE_LED, HIGH); // Turn on the blue LED (default waiting state)
  lcd.setCursor(0, 0);          // Set LCD cursor to the first column, first row
  lcd.print("Please tap an");
  lcd.setCursor(0, 1);          // Set LCD cursor to the first column, second row
  lcd.print("RFID Tag...");
  Serial.println("Please tap an RFID Tag..."); // Also print to Serial Monitor for debugging
}

// --- Loop Function ---
// Runs repeatedly after setup() is complete.
void loop() {
  // Check if a new RFID card is present AND successfully read.
  // If not, ensure the blue LED is on (waiting state) and exit the loop iteration.
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    digitalWrite(BLUE_LED, HIGH); // Keep blue LED on while waiting
    return; // Exit current loop iteration, go back to the start of loop()
  }

  // A card has been detected and read:
  digitalWrite(BLUE_LED, LOW); // Turn off the blue LED as a tag is being processed

  // Display "Processing Tag..." on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Processing Tag...");
  Serial.println("Processing tag..."); // Also print to Serial Monitor

  // --- Generate the RFID Tag's UID String ---
  String scannedContent = ""; // Initialize an empty string to store the UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      scannedContent.concat("0");
    }
    scannedContent.concat(String(mfrc522.uid.uidByte[i], HEX));
    if (i < mfrc522.uid.size - 1) {
      scannedContent.concat(" ");
    }
  }
  scannedContent.toUpperCase(); // Convert the entire UID string to uppercase

  Serial.print("RFID TAG Scanned: ");
  Serial.println(scannedContent); // Print the scanned UID to Serial Monitor

  // --- Check if the Scanned RFID Tag is Authorized ---
  bool authorized = false;      // Flag to track authorization status
  RFIDUser* currentUser = nullptr; // Pointer to the matched user

  // Iterate through the list of authorized users to find a match
  for (int i = 0; i < sizeof(authorizedUsers) / sizeof(authorizedUsers[0]); i++) {
    if (scannedContent.equals(authorizedUsers[i].uid)) {
      authorized = true;
      currentUser = &authorizedUsers[i]; // Store pointer to the found user
      break;
    }
  }

  // --- Handle Authorization Outcome ---
  if (authorized && currentUser != nullptr) {
    // --- Access Granted ---
    digitalWrite(GREEN_LED, HIGH); // Turn on the green LED

    // Update RTC time
    myRTC.updateTime(); // Get the most current time for logging

    // Determine Entry/Exit
    String entryExitStatus;
    if (currentUser->isInside) {
      entryExitStatus = "Exit";
      currentUser->isInside = false; // User is now outside
    } else {
      entryExitStatus = "Entry";
      currentUser->isInside = true;  // User is now inside
    }

    // --- LCD Display ---
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(currentUser->lastName);
    lcd.print(", ");
    lcd.print(currentUser->firstName);
    if (currentUser->middleInitial.length() > 0) {
      lcd.print(" ");
      lcd.print(currentUser->middleInitial);
      lcd.print(".");
    }
    
    lcd.setCursor(0, 1);
    lcd.print(entryExitStatus);
    lcd.print(" - ");
    if (myRTC.hours < 10) lcd.print("0");
    lcd.print(myRTC.hours);
    lcd.print(":");
    if (myRTC.minutes < 10) lcd.print("0");
    lcd.print(myRTC.minutes);
    // Seconds are not typically shown on LCD for entry/exit time

    // --- CSV Output for Access Granted ---
    // Columns: Logs, RFID Tag, StudentID, Last Name, First Name, Middle Initial, Authorized (Yes/No), Entry/Exit, Date, Time
    Serial.print("Access Granted,");   // Logs
    Serial.print(scannedContent);      // RFID Tag (the scanned UID)
    Serial.print(",");
    Serial.print(currentUser->studentID); // StudentID
    Serial.print(",");
    Serial.print(currentUser->lastName); // Last Name
    Serial.print(",");
    Serial.print(currentUser->firstName); // First Name
    Serial.print(",");
    Serial.print(currentUser->middleInitial); // Middle Initial
    Serial.print(",");
    Serial.print("Yes,");              // Authorized (Yes/No)
    Serial.print(entryExitStatus);     // Entry/Exit
    Serial.print(",");

    // Date (MM/DD/YYYY) - Ensure this format matches your desired spreadsheet
    if (myRTC.month < 10) Serial.print("0");
    Serial.print(myRTC.month);
    Serial.print("/");
    if (myRTC.dayofmonth < 10) Serial.print("0");
    Serial.print(myRTC.dayofmonth);
    Serial.print("/");
    Serial.print(myRTC.year);
    Serial.print(",");

    // Time (HH:MM:SS)
    if (myRTC.hours < 10) Serial.print("0");
    Serial.print(myRTC.hours);
    Serial.print(":");
    if (myRTC.minutes < 10) Serial.print("0");
    Serial.print(myRTC.minutes);
    Serial.print(":");
    if (myRTC.seconds < 10) Serial.print("0");
    Serial.print(myRTC.seconds);
    Serial.println(); // New line for the next log entry

    tone(BUZZER_PIN, 1000, 150); // Play a single, high-pitched beep
    delay(3000);                 // Keep green LED and LCD message on for 3 seconds
    digitalWrite(GREEN_LED, LOW);  // Turn off the green LED
  } else {
    // --- Access Denied ---
    digitalWrite(RED_LED, HIGH); // Turn on the red LED

    // Update RTC time for denied access too, for accurate timestamps
    myRTC.updateTime();

    // Display "Unknown User!" and "Access Denied!" messages on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unknown User!"); // Changed from "Unauthorized User!"
    lcd.setCursor(0, 1);
    lcd.print("Access Denied!");

    // --- CSV Output for Access Denied ---
    // Columns: Logs, RFID Tag, StudentID, Last Name, First Name, Middle Initial, Authorized (Yes/No), Entry/Exit, Date, Time
    Serial.print("Access Denied,");   // Logs
    Serial.print(scannedContent);     // RFID Tag (the scanned UID, even if unauthorized)
    Serial.print(",");
    Serial.print("N/A,");             // StudentID
    Serial.print("N/A,");             // Last Name
    Serial.print("N/A,");             // First Name
    Serial.print("N/A,");             // Middle Initial
    Serial.print("No,");              // Authorized (Yes/No)
    Serial.print("N/A,");             // Entry/Exit (or "Denied Attempt")

    // Date (MM/DD/YYYY)
    if (myRTC.month < 10) Serial.print("0");
    Serial.print(myRTC.month);
    Serial.print("/");
    if (myRTC.dayofmonth < 10) Serial.print("0");
    Serial.print(myRTC.dayofmonth);
    Serial.print("/");
    Serial.print(myRTC.year);
    Serial.print(",");

    // Time (HH:MM:SS)
    if (myRTC.hours < 10) Serial.print("0");
    Serial.print(myRTC.hours);
    Serial.print(":");
    if (myRTC.minutes < 10) Serial.print("0");
    Serial.print(myRTC.minutes);
    Serial.print(":");
    if (myRTC.seconds < 10) Serial.print("0");
    Serial.print(myRTC.seconds);
    Serial.println(); // New line for the next log entry

    tone(BUZZER_PIN, 500, 150); // Play first, lower-pitched beep
    delay(200);                 // Short pause between beeps
    tone(BUZZER_PIN, 500, 150); // Play second, lower-pitched beep
    delay(3000);                // Keep red LED and LCD message on for 3 seconds
    digitalWrite(RED_LED, LOW);   // Turn off the red LED
  }

  // --- Prepare for Next Scan ---
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1000); // Small delay to allow the system to settle before detecting the next tag

  // Return to the initial "waiting for tag" state
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please tap an");
  lcd.setCursor(0, 1);
  lcd.print("RFID Tag...");
  digitalWrite(BLUE_LED, HIGH); // Turn the blue LED back ON
  Serial.println("Please tap an RFID Tag..."); // Re-prompt on Serial Monitor
}