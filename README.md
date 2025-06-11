# Arduino RFID Access Control System

This project implements an RFID-based access control system using an Arduino board, an MFRC522 RFID reader, an I2C LCD display, an RTC (Real-Time Clock) module, and indicator LEDs with a buzzer. It logs access attempts (both granted and denied) to the Serial Monitor in a CSV (Comma Separated Values) format, which can be easily imported into a spreadsheet for record-keeping and analysis.

## Features

* **RFID Tag Reading:** Reads UID of RFID tags/cards using the MFRC522 reader.
* **User Authorization:** Checks scanned UIDs against a pre-defined list of authorized users.
* **Detailed User Information:** Stores and displays Last Name, First Name, Middle Initial, and Student ID for authorized users.
* **Entry/Exit Tracking:** Toggles the "inside/outside" status of authorized users with each successful scan, logging it as "Entry" or "Exit".
* **Real-Time Clock (RTC):** Uses a DS1302 RTC module to timestamp all log entries with accurate Date and Time.
* **Visual Feedback:**
    * **Blue LED:** Indicates the system is ready and waiting for an RFID tag.
    * **Green LED:** Lights up for "Access Granted."
    * **Red LED:** Lights up for "Access Denied."
* **Auditory Feedback:**
    * **Buzzer:** Plays distinct tones for "Access Granted" and "Access Denied."
* **LCD Display:** Provides user-friendly messages for system status, access outcome, and user details.
* **CSV Logging:** Outputs comprehensive log data to the Serial Monitor, formatted for direct import into spreadsheet applications (Excel, Google Sheets, LibreOffice Calc).

## Hardware Requirements

* Arduino Uno (or compatible board)
* MFRC522 RFID Reader Module
* DS1302 RTC Module
* I2C LCD Display (e.g., 16x2 with PCF8574 adapter)
* Blue LED
* Green LED
* Red LED
* Buzzer (passive or active)
* 3x 220 Ohm resistors (for LEDs)
* Breadboard
* Jumper Wires
* USB-B Cable (for Arduino Uno)
* CR2032 Coin Cell Battery (for RTC module)

## Software Requirements

* Arduino IDE (version 1.8.x or newer recommended)
* **Libraries (Install via Arduino IDE Library Manager):**
    * `virtuabotixRTC` by virtuabotix (or similar for DS1302, ensure compatibility)
    * `MFRC522` by UIPEthernet or miguelbalboa (e.g., "MFRC522 by `miguelbalboa`")
    * `LiquidCrystal_I2C` by Frank de Brabander (or similar for I2C LCD)
    * `SPI` (usually pre-installed with Arduino IDE)
    * `Wire` (usually pre-installed with Arduino IDE)

## Wiring Diagram

**NOTE:** The provided code uses specific digital pins. **ALWAYS verify your actual wiring against these pin assignments.**

**MFRC522 RFID Reader to Arduino Uno:**

| MFRC522 Pin | Arduino Uno Pin |
| :---------- | :-------------- |
| SDA (SS)    | D10             |
| SCK         | D13             |
| MOSI        | D11             |
| MISO        | D12             |
| IRQ         | N/A (not used)  |
| GND         | GND             |
| RST         | D9              |
| VCC         | 3.3V            |

**DS1302 RTC Module to Arduino Uno:**

| DS1302 Pin | Arduino Uno Pin |
| :--------- | :-------------- |
| VCC        | 5V              |
| GND        | GND             |
| CLK        | D7              |
| DAT        | D6              |
| RST        | D8              |

**I2C LCD Display to Arduino Uno:**

| LCD Pin | Arduino Uno Pin |
| :------ | :-------------- |
| VCC     | 5V              |
| GND     | GND             |
| SDA     | A4 (SDA)        |
| SCL     | A5 (SCL)        |

**LEDs and Buzzer to Arduino Uno:**

| Component       | Arduino Uno Pin | Resistor |
| :-------------- | :-------------- | :------- |
| Blue LED (Anode)| A0              | 220 Ohm  |
| Green LED (Anode)| A1              | 220 Ohm  |
| Red LED (Anode) | A2              | 220 Ohm  |
| Buzzer (+)      | A3              | N/A      |
| All Cathodes    | GND             | N/A      |

## Installation and Setup

1.  **Install Arduino IDE:** Download and install the Arduino IDE from the official website: [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
2.  **Install Libraries:**
    * Open Arduino IDE.
    * Go to `Sketch > Include Library > Manage Libraries...`
    * Search for and install:
        * `virtuabotixRTC`
        * `MFRC522` (by `miguelbalboa`)
        * `LiquidCrystal_I2C` (by `Frank de Brabander`)
3.  **Wire up the Components:** Follow the wiring diagram above carefully. Double-check all connections, especially VCC/GND and data lines.
4.  **Open the Sketch:** Copy the provided Arduino code into a new Arduino IDE sketch.
5.  **Configure `authorizedUsers`:**
    * In the code, locate the `RFIDUser authorizedUsers[]` array.
    * **Crucially, replace the example UIDs and user details with your actual RFID tag UIDs and the corresponding student IDs and names.**
        * To get a tag's UID: Upload the code, open the Serial Monitor, and scan an unknown tag. The UID will be printed there. Copy it.
    * Set the initial `isInside` status (`true` or `false`) for each user.
6.  **Set RTC Time:**
    * In the `setup()` function, locate `myRTC.setDS1302Time(ss, mm, hh, WW, DD, MM,YYYY);`
    * **Update the parameters to your current date and time.**
        * `ss`: seconds (0-59)
        * `mm`: minutes (0-59)
        * `hh`: hours (0-23, 24-hour format)
        * `WW`: day of the week (1=Sunday, 2=Monday, ..., 7=Saturday)
        * `DD`: day of the month (1-31)
        * `MM`: month (1-12)
        * `YYYY`: year (e.g., 2025)
    * **After the first upload with the correct time, you can comment out or remove this line** to prevent the RTC from resetting its time on every subsequent Arduino power-up/reset (as the RTC module has its own battery).
7.  **Select Board and Port:**
    * In the Arduino IDE, go to `Tools > Board` and select your Arduino model (e.g., "Arduino Uno").
    * Go to `Tools > Port` and select the correct COM port that your Arduino is connected to.
8.  **Upload the Sketch:** Click the "Upload" button (right arrow icon) in the Arduino IDE.

    * **Troubleshooting Upload Errors (`Access is denied`):**
        * Ensure the Arduino IDE's Serial Monitor is CLOSED.
        * Ensure no other program (like PuTTY) is connected to the same COM port.
        * Try unplugging and replugging your Arduino.
        * Try a different USB cable or USB port.
        * Restart your computer.

## Usage

1.  **Initial State:** The Blue LED will be ON, and the LCD will display "Please tap an RFID Tag...".
2.  **Scan an RFID Tag:** Place an RFID tag or card near the MFRC522 reader.

### Access Granted:

* **Green LED** will light up.
* **Buzzer** will play a short, high-pitched beep.
* **LCD Display (Example):**
    ```
    ABUED, Jose Anjelo R.
    Entry - 08:30
    ```
    (Or "Exit - HH:MM" if the user was previously registered as "inside")
* **Serial Monitor Output (CSV):**
    ```csv
    Access Granted,AC 80 A2 D3,2023-121343,RAMIREZ,Maria,C.,Yes,Entry,06/13/2025,08:30:15
    ```

### Access Denied (Unknown Tag):

* **Red LED** will light up.
* **Buzzer** will play two short, lower-pitched beeps.
* **LCD Display:**
    ```
    Unknown User!
    Access Denied!
    ```
* **Serial Monitor Output (CSV):**
    ```csv
    Access Denied,D1 0E 01 02,N/A,N/A,N/A,N/A,No,N/A,06/13/2025,08:30:25
    ```
    (Note: "N/A" for user-specific fields, but the RFID Tag and timestamp are still logged.)

### Logging to a Spreadsheet:

1.  **Open PuTTY (or other terminal emulator):**
    * Select "Serial" connection type.
    * Enter your Arduino's COM port (e.g., `COM9`).
    * Set Speed to `9600`.
    * Go to `Session > Logging`.
    * Select "All session output".
    * Browse and specify a log file name (e.g., `rfid_logs.txt`).
    * Click "Open" to start the session and logging.
2.  **Let the system run:** Scan tags and generate log data.
3.  **Close PuTTY:** When you are done collecting data, close the PuTTY window. The log file will be saved.
4.  **Import into Spreadsheet Software (e.g., Microsoft Excel):**
    * Open Excel.
    * Go to `Data > From Text/CSV`.
    * Select your saved `.txt` or `.log` file.
    * In the import wizard/preview, ensure the **Delimiter** is set to **Comma**.
    * Click "Load".

Your data will now be organized into columns: `Logs`, `RFID Tag`, `StudentID`, `Last Name`, `First Name`, `Middle Initial`, `Authorized (Yes/No)`, `Entry/Exit`, `Date`, `Time`.

## Future Enhancements

* **Persistent User State (EEPROM/SD Card):** The `isInside` status for users currently resets if the Arduino loses power. For a robust system, store this state in EEPROM or on an SD card.
* **User Registration Interface:** Implement a way to add new users without re-uploading code (e.g., via Serial Monitor prompts, a keypad, or an SD card management file).
* **Time Synchronization:** Add a button or Serial command to synchronize the RTC with the computer's time.
* **Door Lock/Gate Control:** Integrate a relay module to control a solenoid door lock or gate motor based on access status.
* **Web Interface/Cloud Logging:** For advanced applications, send log data to a local server or cloud database using an Ethernet or Wi-Fi module (e.g., ESP8266/ESP32).

