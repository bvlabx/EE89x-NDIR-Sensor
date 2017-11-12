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

 #ifndef EE89x_h
 #define EE89x_h

 #if (ARDUINO >= 100)
 #include "Arduino.h"
 #else
 #include "WProgram.h"
 #endif

 class EE89x
 {
   public:
     EE89x (int dataPin, int clockPin);
     int readSensortype();
     unsigned char readSensorparameters();
     int readCO2raw();
     int readCO2average();

   private:
     int _dataPin;
     int _clockPin;
     unsigned char readAByte(unsigned char _gBytecmd, int _dataPin, int _clockPin);
     void busDelay(unsigned int count);
     void busStart(int _dataPin, int _clockPin);
     void busStop(int _dataPin, int _clockPin);
     void sendByte(unsigned char _value, int _dataPin, int _clockPin);
     unsigned char readByte (int _dataPin, int _clockPin);
     bool checkAck (int _dataPin, int _clockPin);
     void sendACK(int _dataPin, int _clockPin);
     void sendNAK(int _dataPin, int _clockPin);
  };

#endif
