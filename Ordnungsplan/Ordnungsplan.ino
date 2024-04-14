#include <Wire.h>
#include <U8g2lib.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//---------Setup-Options---------
// Set up the Display pins
#define Display_SDA 26
#define Display_SCL 27

// Set up the GPS pins
#define GPS_RXPin 16
#define GPS_TXPin 17
#define GPS_Baud 9600

// Define  the time shift in your region
#define time_shift 2 //(GM+2)

// Define the DEBUG macro to enable or disable the debug output
#define DEBUG 0
// Define the GPS_DEBUG macro to enable or disable the GPS debug output
#define GPS_DEBUG 1






// Create the U8G2 display object
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, Display_SDA, Display_SCL);

// Create the TinyGPS++ object
TinyGPSPlus gps;

// Set up the serial port for GPS data
SoftwareSerial ss(GPS_RXPin, GPS_TXPin);

// Add the dayOfYear and isoWeekNumber functions here
int dayOfYear(int year, int month, int day) {
  // Calculate the day of the year
  int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  return days[month - 1] + day + (month > 2 ? 1 : -1) * (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int isoWeekNumber(int year, int month, int day) {
  // Calculate the ISO week number
  int doy = dayOfYear(year, month, day);
  int doyOfFirstThursday = (4 - (year + (year/4) - (year/100) + (year/400)) % 7 + doy) % 7;
  return (doy - doyOfFirstThursday) / 7 + 1;
}

void drawMenu(String Z1, String Z2, String Z3, int year, int month, int day, int hour, int minute, int second) {
  // Draw the cleaningplan on the display with time and date
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(5, 12);
  String S1 = "K";
  String S2 = "\xfc";
  String S3 = "che";
  String Kueche = S1 + S2 + S3;
  u8g2.print(Kueche);
  u8g2.setCursor(45, 12);
  u8g2.print("|Bad");
  u8g2.setCursor(85, 12);
  u8g2.print("|Flur");

  u8g2.setCursor(45, 17);
  u8g2.print("|");
  u8g2.setCursor(85, 17);
  u8g2.print("|");

  u8g2.setCursor(45, 22);
  u8g2.print("|");
  u8g2.setCursor(85, 22);
  u8g2.print("|");

  u8g2.setFont(u8g2_font_5x8_mn);
  u8g2.setCursor(5, 22);
  u8g2.print(Z1);
  u8g2.setCursor(50, 22);
  u8g2.print(Z2);
  u8g2.setCursor(90, 22);
  u8g2.print(Z3);

  u8g2.setFont(u8g2_font_6x10_tf);

  // Display date and time
  u8g2.setCursor(35, 57);
  u8g2.print(day);
  if (day < 10) {
  u8g2.print("0");
  }
  u8g2.print(".");
  if (month < 10) {
  u8g2.print("0");
  }
  u8g2.print(month);
  u8g2.print(".");
  u8g2.print(year);
  u8g2.setCursor(38, 42);
  if (hour < 10) {
  u8g2.print("0");
  }
  u8g2.print(hour);
  u8g2.print(":");
  if (minute < 10) {
  u8g2.print("0");
  }
  u8g2.print(minute);
  u8g2.print(":");
  if (second < 10) {
  u8g2.print("0");
  }
  u8g2.print(second);

  u8g2.sendBuffer();
}

void setup() {
  // Initialize the serial port for debugging
  Serial.begin(115200);

  // Initialize the U8G2 display
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.sendBuffer();

  // Initialize the GPS pins
  pinMode(GPS_RXPin, INPUT);
  pinMode(GPS_TXPin, OUTPUT);

  // Initialize the GPS serial port
  ss.begin(GPS_Baud);

#if DEBUG
  // Print a message to the serial monitor to indicate that the setup is complete
  Serial.println("Setup complete.");
#endif
}

void loop() {
  // Read data from the GPS module
  while (ss.available()) {
    gps.encode(ss.read());
    if (gps.date.isUpdated() && gps.time.isUpdated()) {
      // Get the current date and time
      int year = gps.date.year();
      int month = gps.date.month();
      int day = gps.date.day();
      int hour = gps.time.hour();
      int minute = gps.time.minute();
      int second = gps.time.second();

      // Calculate the ISO week
      int isoYear = year;
      int isoWeek = isoWeekNumber(year, month, day);

      // Convert the GPS time to local time (GMT+2)
      hour = hour + time_shift;
      if (hour >= 24) {
        hour = hour - 24;
        day++;
        if (day > 31) {
          day = 1;
          month++;
          if (month > 12) {
            month =1;
            year++;
          }
        }
      }

#if GPS_DEBUG
      // Print the GPS data to the serial monitor
      Serial.print("Date: ");
      Serial.print(gps.date.day());
      Serial.print("/");
      Serial.print(gps.date.month());
      Serial.print("/");
      Serial.print(gps.date.year());
      Serial.print(" Time: ");
      Serial.print(gps.time.hour());
      Serial.print(":");
      Serial.print(gps.time.minute());
      Serial.print(":");
      Serial.print(gps.time.second());
      Serial.print(" Quality: ");
      Serial.println(gps.satellites.value());
      Serial.print(F("DIAGS      Chars="));
      Serial.print(gps.charsProcessed());
      Serial.print(F(" Sentences-with-Fix="));
      Serial.print(gps.sentencesWithFix());
      Serial.print(F(" Failed-checksum="));
      Serial.print(gps.failedChecksum());
      Serial.print(F(" Passed-checksum="));
      Serial.println(gps.passedChecksum());

#endif

#if DEBUG
      // Display the time and ISO week on the serial monitor
      Serial.print(hour);
      Serial.print(":");
      Serial.print(minute);
      Serial.print(":");
      Serial.println(second);
      Serial.print("ISO Week: ");
      Serial.println(isoWeek);
#endif

      // Assign the rooms to different strings based on the week number
      String Zimmer1, Zimmer2, Zimmer3;
      switch (isoWeek % 3) {
        case 1:
          Zimmer1 = "3.02.1";
          Zimmer2 = "3.02.2";
          Zimmer3 = "3.02.3";
          break;
        case 2:
          Zimmer1 = "3.02.2";
          Zimmer2 = "3.02.3";
          Zimmer3 = "3.02.1";
          break;
        case 0:
          Zimmer1 = "3.02.3";
          Zimmer2 = "3.02.1";
          Zimmer3 = "3.02.2";
          break;
      }

      // Display the time and ISO week on the screen
      drawMenu(Zimmer1, Zimmer2, Zimmer3, year, month, day, hour, minute, second);
    }
  }
}