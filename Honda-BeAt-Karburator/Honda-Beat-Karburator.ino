#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>    // Library for Mifare RC522 Devices


/*
  For visualizing whats going on hardware
  we need some leds and
  to control door lock a relay and a wipe button
  (or some other hardware)
  Used common anode led,digitalWriting HIGH turns OFF led
  Mind that if you are going to use common cathode led or
  just seperate leds, simply comment out #define COMMON_ANODE,
 */

//define redLed 7    // Set Led Pins
//define greenLed 6
//#define blueLed 5
#define blueLed 8

#define relay1 A1     // Set relay1 Pin
#define relay2 A2     // Set relay2 Pin
#define relay3 A3     // Set relay3 Pin

boolean match = false;          // initialize card match to false
boolean programMode = false;    // initialize programming mode to false

int successRead;    // Variable integer to keep if we have Successful Read from Reader

byte storedCard[4];   // Stores an ID read from EEPROM
byte readCard[4];     // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM

/*
  We need to define MFRC522's pins and create instance
  Pin layout should be as follows (on Arduino Uno):
  MOSI: Pin 11 / ICSP-4
  MISO: Pin 12 / ICSP-1
  SCK : Pin 13 / ICSP-3
  SS : Pin 10 (Configurable)
  RST : Pin 9 (Configurable)
  look MFRC522 Library for
  other Arduinos' pin configuration 
 */

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  //Arduino Pin Configuration
  pinMode(blueLed, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  //Be careful how relay1 circuit behave on while resetting or power-cycling your Arduino
  digitalWrite(relay1, HIGH);    // Make Motor is mati
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(blueLed, LOW); // Make sure led is off
  //Protocol Configuration
  Serial.begin(9600);  // Initialize serial communications with PC
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  
  //If you set Antenna Gain to Max it will increase reading distance
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  
  Serial.println(F("Access Control v3.3"));   // For debugging purposes
  ShowReaderDetails();  // Show details of PCD - MFRC522 Card Reader details

  // Check if master card defined, if not let user choose a master card
  // This also useful to just redefine Master Card
  // You can keep other EEPROM records just write other than 143 to EEPROM address 1
  // EEPROM address 1 should hold magical number which is '143'
  if (EEPROM.read(1) != 143) {      
    Serial.println(F("No Master Card Defined"));
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
      successRead = getID();            // sets successRead to 1 when we get read from reader otherwise 0
      digitalWrite(blueLed, HIGH);      // Visualize Master Card need to be defined
      delay(200);
      digitalWrite(blueLed, LOW);
      delay(200);
    }
    while (!successRead);                  // Program will not go further while you not get a successful read
    for ( int j = 0; j < 4; j++ ) {        // Loop 4 times
      EEPROM.write( 2 + j, readCard[j] );  // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1, 143);                  // Write to EEPROM we defined Master Card.
    Serial.println(F("Master Card Defined"));
  }
  Serial.println(F("-------------------"));
  Serial.println(F("Master Card's UID"));
  for ( int i = 0; i < 4; i++ ) {          // Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i);    // Write it to masterCard
    Serial.print(masterCard[i], HEX);
  }
  Serial.println("");
  Serial.println(F("-------------------"));
  Serial.println(F("Everything Ready"));
  Serial.println(F("Waiting PICCs to be scanned"));
  cycleLeds();    // Everything ready lets give user some feedback by cycling leds
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {
  do {
    successRead = getID();  // sets successRead to 1 when we get read from reader otherwise 0
    if (programMode) {
      cycleLeds();              // Program Mode cycles through RGB waiting to read a new card
    }
    else {
      normalModeOn();     // Normal mode, blue Power LED is on, all others are off
    }
  }
  while (!successRead);   //the program will not go further while you not get a successful read
  if (programMode) {
    if ( isMaster(readCard) ) { //If master card scanned again exit program mode
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else {
      if ( findID(readCard) ) { // If scanned card is known delete it
        Serial.println(F("I know this PICC, removing..."));
        deleteID(readCard);
        Serial.println("-----------------------------");
      }
      else {                    // If scanned card is not known add it
        Serial.println(F("I do not know this PICC, adding..."));
        writeID(readCard);
        Serial.println(F("-----------------------------"));
      }
    }
  }
  else {
    if ( isMaster(readCard) ) {   // If scanned card's ID matches Master Card's ID enter program mode
      programMode = true;
      Serial.println(F("Hello Master - Entered Program Mode"));
      int count = EEPROM.read(0);   // Read the first Byte of EEPROM that
      Serial.print(F("I have "));     // stores the number of ID's in EEPROM
      Serial.print(count);
      Serial.print(F(" record(s) on EEPROM"));
      Serial.println("");
      Serial.println(F("Scan a PICC to ADD or REMOVE"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if ( findID(readCard) ) { // If not, see if the card is in the EEPROM
        Serial.println(F("Bismillaahi tawakkaltu 'alallaah"));
        granted();         // Open the door lock for 300 ms
      }
      else {      // If not, show that the ID was not valid
        Serial.println(F("Kamu Ngapain Coba2 ? Mau Curi Motor Orang"));
        denied();
      }
    }
  }
}

/////////////////////////////////////////  Access Granted    ///////////////////////////////////
void granted() {
  digitalWrite(blueLed, HIGH);      // Turn ON blue LED
  digitalWrite(relay1, LOW);       // relay1 Motor Hidup
  delay(100);
  digitalWrite(relay2, LOW);       // relay1 Motor Hidup
  delay(2000);
  digitalWrite(relay2, HIGH);
  delay(43200000);
}

///////////////////////////////////////// Access Denied  ///////////////////////////////////
void denied() {
  digitalWrite(blueLed, LOW);   // Make sure blue LED is off
  delay(1000);
}


///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
int getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned PICC's UID:"));
  for (int i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    while(true);  // do not go further
  }
}

///////////////////////////////////////// Cycle Leds (Program Mode) ///////////////////////////////////
void cycleLeds() {
  digitalWrite(blueLed, LOW);   // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, HIGH);  // Make sure blue LED is on
  delay(200);
  digitalWrite(blueLed, LOW);   // Make sure blue LED is off
  delay(200);
}

//////////////////////////////////////// Normal Mode Led  ///////////////////////////////////
void normalModeOn() {
  digitalWrite(blueLed, LOW);  // Blue LED off dan siap membaca kartu
  digitalWrite(relay1, HIGH);    // Pastikan relay1 mati
  digitalWrite(relay2, HIGH);    // Pastikan relay2 mati
}

//////////////////////////////////////// Read an ID from EEPROM //////////////////////////////
void readID( int number ) {
  int start = (number * 4 ) + 2;    // Figure out starting position
  for ( int i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}

///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
void writeID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    int start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write( 0, num );     // Write the new count to the counter
    for ( int j = 0; j < 4; j++ ) {   // Loop 4 times
      EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }
    successWrite();
  Serial.println(F("Succesfully added ID record to EEPROM"));
  }
  else {
    failedWrite();
  Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}

///////////////////////////////////////// Remove ID from EEPROM   ///////////////////////////////////
void deleteID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite();      // If not
  Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else {
    int num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards
    int slot;       // Figure out the slot number of the card
    int start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    int looping;    // The number of times the loop repeats
    int j;
    int count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;      // Decrement the counter by one
    EEPROM.write( 0, num );   // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( int k = 0; k < 4; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);
    }
    successDelete();
  Serial.println(F("Succesfully removed ID record from EEPROM"));
  }
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL )       // Make sure there is something in the array first
    match = true;       // Assume they match at first
  for ( int k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] )     // IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if ( match ) {      // Check to see if if match is still true
    return true;      // Return true
  }
  else  {
    return false;       // Return false
  }
}

///////////////////////////////////////// Find Slot   ///////////////////////////////////
int findIDSLOT( byte find[] ) {
  int count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
      break;          // Stop looking we found it
    }
  }
}

///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
boolean findID( byte find[] ) {
  int count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;  // Stop looking we found it
    }
    else {    // If not, return false
    }
  }
  return false;
}

///////////////////////////////////////// Write Success to EEPROM   ///////////////////////////////////
// Flashes the blue LED 3 times to indicate a successful write to EEPROM
void successWrite() {
  digitalWrite(blueLed, LOW);  // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, HIGH);   // Make sure green LED is on
  delay(200);
  digitalWrite(blueLed, LOW);  // Make sure green LED is off
  delay(200);
  digitalWrite(blueLed, HIGH);   // Make sure green LED is on
  delay(200);
  digitalWrite(blueLed, LOW);  // Make sure green LED is off
  delay(200);
  digitalWrite(blueLed, HIGH);   // Make sure green LED is on
  delay(200);
}

///////////////////////////////////////// Write Failed to EEPROM   ///////////////////////////////////
// Flashes the blue LED 2 times to indicate a failed write to EEPROM
void failedWrite() {
  digitalWrite(blueLed, LOW);   // Make sure blue LED is off
  delay(1000);
  digitalWrite(blueLed, HIGH);   // Make sure red LED is on
  delay(1000);
  digitalWrite(blueLed, LOW);  // Make sure red LED is off
  delay(1000);
  digitalWrite(blueLed, HIGH);   // Make sure red LED is on
  delay(1000);
}

///////////////////////////////////////// Success Remove UID From EEPROM  ///////////////////////////////////
// Flashes the blue LED 3 times to indicate a success delete to EEPROM
void successDelete() {
  digitalWrite(blueLed, LOW);   // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, HIGH);  // Make sure blue LED is on
  delay(200);
  digitalWrite(blueLed, LOW);   // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, HIGH);  // Make sure blue LED is on
  delay(200);
  digitalWrite(blueLed, LOW);   // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, HIGH);  // Make sure blue LED is on
  delay(200);
}

////////////////////// Check readCard IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
boolean isMaster( byte test[] ) {
  if ( checkTwo( test, masterCard ) )
    return true;
  else
    return false;
}


