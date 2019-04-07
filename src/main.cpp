/*****************************************************************************************
Title           : Mayele-Labs Embedded Firmware 
Version         : 0.01
Author          : Fabrice Beya

Summary:
This is an application that collects motion data from the MPU90050, heart rate data from the
MAX30100, logs the data in an SD card, display summary of the data to user through an TFT 
display and finally sends the data to a mobile application over Bluetooth Low Energy.

*****************************************************************************************/

/*******************************************************************************
  Libraries
*******************************************************************************/

#include <Arduino.h>
#include "SPI.h"
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"
#include <MPU6050_tockn.h>
#include "FS.h"
#include "SD.h"
#include <SD_MMC.h> 

/*******************************************************************************
  System States
*******************************************************************************/

#define STATE_STORAGE_READY 0x1
#define STATE_MEMS_READY 0x2
#define STATE_OLED_READY 0x3
#define STATE_SD_READY 0x4
#define STATE_WORKING 0x5

// A generic class to manage system states. 
class State 
{
  public:
    bool check(uint16_t flags) { return (m_state & flags) == flags; }
    void set(uint16_t flags) { m_state |= flags; }
    void clear(uint16_t flags) { m_state &= ~flags; }
  private:
    uint16_t m_state = 0;
};

/*******************************************************************************
  Data Structures
*******************************************************************************/
//
State state;

// Wrover kit lcd
WROVER_KIT_LCD tft;

// MPU6050 Accelerometer
MPU6050 mpu6050(Wire);

// mems data
struct Mems {
  float accX;
  float accY;
  float accZ;
  float gyroX;
  float gyroY;
  float gyroZ;
};

Mems mems;

/*******************************************************************************
  System Time management
*******************************************************************************/
long timer = 0;

/*******************************************************************************
  Initializing all components and network
*******************************************************************************/

bool initialize(bool wait = false)
{

}


/*******************************************************************************
  Storage Management
*******************************************************************************/

void writeFile(fs::FS &fs, const char * path, const char * message){
  
  File file = fs.open(path, FILE_WRITE);
  if(!file){
    tft.println("Failed to open file for writing.");
    return;
  }
  if(file.print(message)){
     tft.println("File written.");
  } else {
    tft.println("Write failed.");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    File file = fs.open(path, FILE_APPEND);
    if(!file){
      Serial.println("Failed to open file for appending");
      return;
    }
    if(file.print(message)){
      Serial.println("Message appended");
    } else {
      Serial.println("Append failed");
    }
    file.close();
}

bool checkSDCard()
{
  // state &= ~STATE_SD_READY;
  if(!SD_MMC.begin()){
    Serial.println("Card Mount Failed");
    return false;
  }

  // state |= STATE_SD_READY;
  return true;
}

void log_data(String data)
{
  appendFile(SD_MMC, "/memsdata.csv", data.c_str());
}
/*******************************************************************************
  LCD API:
*******************************************************************************/
void lcd_default_settings()
{
  tft.fillScreen(WROVER_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WROVER_WHITE);  tft.setTextSize(1);
}
void lcd_print(String text) 
{
  tft.println(text);
}

/*******************************************************************************
  Accelerometer MPU6050 API:
*******************************************************************************/

void process_mems(void *parameter)
{
  mpu6050.update();

  if(millis() - timer > 1000)
  {
    mems.accX = mpu6050.getAccX();
    mems.accY = mpu6050.getAccY();
    mems.accZ = mpu6050.getAccZ();
    mems.gyroX = mpu6050.getGyroX();
    mems.gyroY = mpu6050.getGyroY();
    mems.gyroZ = mpu6050.getGyroZ();
    timer = millis();
    String lcd_output = "AccX" + String(mems.accX) + " AccY" + String(mems.accY) + " AccZ" + String(mems.accZ) + "\n" + 
    "GyroX" + String(mems.gyroX) + " GyroY" + String(mems.gyroY) + " GyroZ" + String(mems.gyroZ) ;

    String csv_output =  String(mems.accX) + ";" + String(mems.accY) + ";" + String(mems.accY) + ";"
    + String(mems.gyroX) + ";" + String(mems.gyroY) + ";" + String(mems.gyroZ) + "\n" ;

    lcd_print(lcd_output);
    log_data(csv_output);
  }

}

/*******************************************************************************
  Setup up peripherals
*******************************************************************************/

void setup() 
{
  // Start uart serial communication
  Serial.begin(112500);
  delay(1000);

  //Start tft
  tft.begin();
  lcd_default_settings();
  lcd_print("\n System initialized");
  delay(1000);

  //Start MPU6050
  Wire.begin(26,27);
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  delay(1000);

  //Start SD Card
  if(checkSDCard())
  {
    // create a new csv file with column headings
    writeFile(SD_MMC, "/memsdata.csv", "accx;accy;accz;gyrox;gyroy;gyroz\r\n");
  }
  else
  {
    Serial.println("SD card failure.");
    tft.println("SD card failure.");
  }

  tft.println("SD card is operational.");

  // xTaskCreate(process_mems, "process_mems", 1000000, NULL, 1, NULL);
}

/*******************************************************************************
  Main loop 
*******************************************************************************/

void loop() 
{
  process_mems(NULL);
}