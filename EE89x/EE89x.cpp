/**
 * EE89x Library
 *
 * Copyright 2017 Borislav Lazarov <bv.lazarov@gmail.com>
 * Based on the code provided from the manifacture:
 * http://downloads.epluse.com/fileadmin/data/sw/Specification_E2_Interface.pdf
 * and examples provided by http://blueberrye.io
 * 
 * Manages communication with EE892 and EE893 NDIR CO2 sensor module
 * from E+E Electronik  (www.epluse.com/EE893).
 */

 #include "Arduino.h"
 #include "EE89x.h"

 // Definitions
 //-----------------------------------------------------------------------------
 #define DELAY_FACTOR 20 //Delay factor of 20 has been found applicable for 16MHz Arduino µC
 #define _gSensorType_LBCmd 0x11
 #define _gSensorType_HBCmd 0x41
 #define _gSensorParameters_Cmd 0x31
 #define _gCO2raw_LBCmd 0xC1
 #define _gCO2raw_HBCmd 0xD1
 #define _gCO2average_LBCmd 0xE1
 #define _gCO2average_HBCmd 0xF1


 EE89x::EE89x(int dataPin, int clockPin)
 {
   _dataPin = dataPin;
   _clockPin = clockPin;
 }

 /* ================  Public methods ================ */

 /**
  * Reads the sensor type
  */
 int EE89x::readSensortype()
 {
   unsigned int _sensorGroup;
   unsigned char _group_LB;
   unsigned char _group_HB;

   _group_LB = readAByte(_gSensorType_LBCmd, _dataPin, _clockPin);
   _group_HB = readAByte(_gSensorType_HBCmd, _dataPin, _clockPin);

   _sensorGroup = (_group_HB*256+_group_LB);

   return (_sensorGroup);
 }

 /**
  * Reads the sensor's supported parameters
  * Every bit represents a physical measurement
  * The active physical measured parameters of the sensor are presened as a byte in the format:
  *      bit 7   |    bit 6    |    bit 5    |    bit 4    |  bit 3 |      bit 2      |       bit 1       |     bit 0     |
  * reserved | reserved | reserved | reserved | CO2 | air velocity | temperature | humidity |
  * 1 - supported
  * 0 - not supported
  */
 unsigned char EE89x::readSensorparameters()
 {
   unsigned char _sensorParameters;

   unsigned char _parameters = readAByte(_gSensorParameters_Cmd, _dataPin, _clockPin);
   if (_parameters == 0xFF) {
     _sensorParameters = 0x00;
   } else {
       _parameters = _parameters << 4;
       _parameters = _parameters >> 4;
       _sensorParameters = _parameters;
     }
   return (_sensorParameters);
 }

 /**
  * Reads the current CO2 value (fast response). This measurement has fast response and
  * low accuracy level.
  */
 int EE89x::readCO2raw()
 {
   int _co2raw;
   unsigned char _co2_LB;
   unsigned char _co2_HB;


   _co2_LB = readAByte(_gCO2raw_LBCmd, _dataPin, _clockPin);
   _co2_HB = readAByte(_gCO2raw_HBCmd, _dataPin, _clockPin);

   _co2raw = (_co2_HB*256)+_co2_LB;

   return (_co2raw);
 }

 /**
  * Reads the average CO2 value from at least 11 measurements (slow response)
  */
 int EE89x::readCO2average()
 {
   int _co2average;
   unsigned char _co2_LB;
   unsigned char _co2_HB;

   _co2_LB = readAByte(_gCO2average_LBCmd, _dataPin, _clockPin);
   _co2_HB = readAByte(_gCO2average_HBCmd, _dataPin, _clockPin);

   _co2average = (_co2_HB*256)+_co2_LB;

   return (_co2average);
 }

 /* ================  Private methods ================ */

 //Command to read to the  measurement byte
 unsigned char EE89x::readAByte(unsigned char _gBytecmd, int _dataPin, int _clockPin)
 {
   unsigned char _valueByte;
   unsigned char _checkSumByte;
   int _statusByte = 1;
   int _ntrysByte = 1;

   while (_statusByte && _ntrysByte < 3)
   {
   busStart(_dataPin, _clockPin);
   sendByte(_gBytecmd, _dataPin, _clockPin);
   if (checkAck( _dataPin, _clockPin) == 0) {
    _valueByte = readByte( _dataPin, _clockPin);
    sendACK( _dataPin, _clockPin);
    _checkSumByte = readByte(_dataPin, _clockPin);
    sendNAK(_dataPin, _clockPin);
    busStop(_dataPin, _clockPin);
  } else {
    //Serial.println("error _valueByte ack = 1");
    _ntrysByte++;
    busStop(_dataPin, _clockPin);
  }
  if (_checkSumByte != ((_valueByte + _gBytecmd) % 0x100)) {
     //Serial.println("error _valueByte checkSum not match");
     _valueByte = 0xFF;
   } else { _statusByte = 0;}
 }
 return (_valueByte);
}

 /**
 * Bus delay function
 */
 void EE89x::busDelay(unsigned int count) {
  volatile unsigned int count2;
  count2 = count;
  count2 = count2 * DELAY_FACTOR;
  while (-- count2 != 0);
 }

 /**
  * Start bit
  */
 void EE89x::busStart(int _dataPin, int _clockPin)
{
  pinMode (_dataPin, OUTPUT);
  pinMode (_clockPin, OUTPUT);
  digitalWrite(_dataPin, HIGH);
  digitalWrite(_clockPin, HIGH);
  busDelay (30);
  digitalWrite(_dataPin, LOW);
  busDelay (30);
 }

 /**
 * Stop bit
 */
 void EE89x::busStop(int _dataPin, int _clockPin)
{
 pinMode(_dataPin, OUTPUT);
 pinMode(_clockPin, OUTPUT);
 digitalWrite(_clockPin, LOW);
 busDelay(20);
 digitalWrite(_dataPin, LOW);
 busDelay(20);
 digitalWrite(_clockPin, HIGH);
 busDelay (20);
 digitalWrite(_dataPin, HIGH);
 }

 /**
 * Send a byte
 */
 void EE89x::sendByte(unsigned char _value, int _dataPin, int _clockPin)
{
  unsigned char mask;

  for (mask = 0x80; mask > 0; mask >>= 1)
  {
    pinMode (_dataPin, OUTPUT);
    pinMode (_clockPin, OUTPUT);
    digitalWrite(_clockPin, LOW);
    busDelay(10);
    if ((_value & mask) !=0)
    {
      digitalWrite(_dataPin, HIGH);
    } else {
      digitalWrite(_dataPin, LOW);
    }
    busDelay(20);
    digitalWrite(_clockPin, HIGH);
    busDelay(30);
    digitalWrite(_clockPin, LOW);
  }
  digitalWrite(_dataPin, HIGH);
 }

 /**
 * Read byte
 */
 unsigned char EE89x::readByte (int _dataPin, int _clockPin)
{
  unsigned char data_in = 0x00;
  unsigned char mask = 0x80;

  for (mask = 0x80; mask>0; mask >>=1)
  {
    pinMode (_clockPin, OUTPUT);
    digitalWrite(_clockPin, LOW);
    busDelay(30);
    digitalWrite(_clockPin, HIGH);
    busDelay(15);
    pinMode (_dataPin, INPUT);
    if (digitalRead(_dataPin))
    {
      data_in |= mask;
    }
    busDelay(15);
    digitalWrite(_clockPin, LOW);
  }
  return data_in;
 }

 /**
 * Check ACK
 */
 bool EE89x::checkAck (int _dataPin, int _clockPin)
 {
  bool input;
  pinMode (_clockPin, OUTPUT);
  digitalWrite(_clockPin, LOW);
  busDelay(30);
  digitalWrite(_clockPin, HIGH);
  busDelay(15);
  pinMode (_dataPin, INPUT);
  input = digitalRead(_dataPin);
  busDelay(15);
  return input;
 }

 /**
 * Send ACK
 */
 void EE89x::sendACK(int _dataPin, int _clockPin)
 {
 pinMode (_dataPin, OUTPUT);
 pinMode (_clockPin, OUTPUT);
 digitalWrite(_clockPin, LOW);
 busDelay(15);
 digitalWrite(_dataPin, LOW);
 busDelay(15);
 digitalWrite(_clockPin, HIGH);
 busDelay (28);
 digitalWrite(_clockPin, LOW);
 busDelay (2);
 digitalWrite(_dataPin, HIGH);
 }

 /**
 * Send NAK
 */
 void EE89x::sendNAK(int _dataPin, int _clockPin)
 {
 pinMode (_dataPin, OUTPUT);
 pinMode (_clockPin, OUTPUT);
 digitalWrite(_clockPin, LOW);
 busDelay(15);
 digitalWrite(_dataPin, HIGH);
 busDelay(15);
 digitalWrite(_clockPin, HIGH);
 busDelay (30);
 digitalWrite(_clockPin, HIGH);
 }
