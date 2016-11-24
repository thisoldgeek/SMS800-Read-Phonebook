//*****************************************************
// Works to extract phonebook data from the SIM800H
// such as on the Feather Fona
//
// Need to first load phone book by Bluetooth pairing with
// your phone and downloading to the SIM800H with
// a serial monitor (such as the "S" tunnel in the
// Adafruit Fona Test) to run raw AT+ commands
// 
// Process for the iPhone 6 is described at:
//  thisoldgeek.blogspot.com
//
// 11-14-2016 bob murphy aka thisoldgeek
//
//*****************************************************


#include "Adafruit_FONA.h"

#define FONA_RX  9
#define FONA_TX  8
#define FONA_RST 4
#define FONA_RI  7

// this is a large buffer for replies
char replybuffer[255];

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
//  HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {
  while (!Serial);

  Serial.begin(115200);
  Serial.println(F("FONA SMS caller ID test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  // make it slow so its easy to read!
  fonaSerial->begin(9600);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }
  Serial.println(F("FONA is OK"));

  // Print SIM card IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }
  
  Serial.println("FONA Ready");
  delay(3000);
}
void getPhoneBook()
{ char *toSend = "AT+CPBR=1";
  
  int numRecs = 6;
  //Serial.print("Sending: ");
  Serial.println(toSend);
  
 // Serial.print("Received: ");
 for (int i=1;i<numRecs;i++)
 {String result = "";
 toSend[8] = i;
 fonaSS.println(toSend);
  if (fona.available()) 
      {  
      char c = fonaSS.read();
      //Serial.write(c);
      result += c;  // append to the result string
      }
      Serial.println(result);
 }   
}
  


String getFavorites( int i) {
  unsigned long milliseconds = 500;
  char toSend[] = "AT+CPBR=";   
  // see the SIM800 AT Command Reference for more details: 
  /* https://cdn-shop.adafruit.com/datasheets/sim800_series_at_command_manual_v1.01.pdf */
  String result;
  int numRecs = 5;  // Number of records to return 
  
  unsigned long startTime = millis();
  fonaSS.print(toSend);
  fonaSS.println(i);
    char quote = '"';

  int ind1;
  int ind2;
  int ind3;
  int ind4;

  // char arrays for passing data
  char numPhone[50];
  char namePhone[50];
  // Strings for local processing only
  String Str_numPhone;
  String Str_namePhone;

  while (millis() - startTime < milliseconds) 
  {  
    if (fona.available()) {
      char c = fonaSS.read();
      result += c;  // append to the result string
    }
  }      

   // get the phone number and name from the phonebook data
  ind1 = result.indexOf('"');  //finds location of first quote,
  ind2 = result.indexOf(quote, ind1+1 );   //finds end of first data string - phone number
  Str_numPhone = result.substring(ind1+1, ind2);   //captures first data String
  ind3 = result.indexOf(quote, ind2+1 );   //finds location of third quote
  ind4 = result.indexOf(quote, ind3+1 );   //finds location of last quote
  Str_namePhone = result.substring(ind3+1, ind4);   //captures second data string - phone name

  Str_numPhone.toCharArray(numPhone,50);
  Str_namePhone.toCharArray(namePhone,50);
      
  Serial.print(numPhone);
  Serial.print(" ");
  Serial.println(namePhone);
  
 
  return result;
  
  
}

void loop()
{ int numRecs = 5;
  
    for (int i=1;i<=+numRecs;i++)
    {
      getFavorites(i);
    } 
  //delay(2000);
}

