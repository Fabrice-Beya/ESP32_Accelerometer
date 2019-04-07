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

/*******************************************************************************
  System States
*******************************************************************************/

#define STATE_STORAGE_READY 0x1
#define STATE_MEMS_READY 0x2
#define STATE_OLED_READY 0x3
#define STATE_WORKING 0x4

class State {
public:
  bool check(uint16_t flags) { return (m_state & flags) == flags; }
  void set(uint16_t flags) { m_state |= flags; }
  void clear(uint16_t flags) { m_state &= ~flags; }
private:
  uint16_t m_state = 0;
};

/*******************************************************************************
  Initializing all components and network
*******************************************************************************/
bool initialize(bool wait = false)
{

}

/*******************************************************************************
  Setup up peripherals
*******************************************************************************/

void setup() {
  // put your setup code here, to run once:
}

/*******************************************************************************
  Main loop 
*******************************************************************************/

void loop() {
  // put your main code here, to run repeatedly:
}