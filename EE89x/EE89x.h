/**
 * EE89x Library
 *
 * Copyright 2020 Borislav Lazarov <bv.lazarov@gmail.com>
 * Based on the code provided from the manifacture:
 *     http://downloads.epluse.com/fileadmin/data/sw/Specification_E2_Interface.pdf
 *
 * Manages communication with EE893 and EE894 NDIR CO2 sensor module
 * from E+E Electronik  (www.epluse.com/EE894).
 */
 #ifndef EE89x_h
 #define EE89x_h
 
 

 class EE89x
 {
   public:
     EE89x (int dataPin, int clockPin);
     int readSensortype();
     unsigned char readSensorparameters();
	   unsigned char readStatusbyte();
     float readCO2average();
     float readHumidity();
     float readTemperature();
     float readPressure();

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