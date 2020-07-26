/**
 * ReadEE89xValues
 *
 * Read CO2 concentration values from an EE89x-series (EE892 and EE893)
 * CO2 sensor modules from E+E Electronik  (www.epluse.com)
 *
 * 2017 Borislav Lazarov <bv.lazarov@gmail.com>
 */

#include <EE89x.h>

// Specify data and clock connections and instantiate EE89x object
#define dataPin 4
#define clockPin 5
EE89x ee89x(dataPin, clockPin);

void setup()
{
  Serial.begin(9600); // Open serial connection to report values to host
  Serial.println("Starting up");
  delay(5000);
  Serial.println("*****************Sensor information********************");
  Serial.print("sensor type EE");
  Serial.println (ee89x.readSensortype(), DEC); //read and print to the host the sensor type
  Serial.println("*******************************************************");
  delay(10000);//the sensor needs at least 15s after the power up to complete the measurement
}

void loop() {
  float temp; 
  float hum;
  float press;
  int co2_average;

  // Read values from the sensor
  temp = ee89x.readTemperature(); //read the temperature value from the sensor
  hum = ee89x.readHumidity(); //read the Relative Humidity value from the sensor
  press = ee89x.readPressure(); //read the Ambient Pressure value from the sensor
  co2_average = ee89x.readCO2average(); //read the average CO2 value from at least 11 single masurements

  // Print the values to the serial port
  Serial.print("Temperature: ");
  Serial.print(temp, 1);
  Serial.print(" °C / Humidity: ");
  Serial.print(hum, 1);
  Serial.print(" % / Atm. Pressure: ");
  Serial.print(" mbar / CO2 concentration: ");
  Serial.print(co2_average, DEC);
  Serial.println(" ppm");
  delay(20000); //the min measurement interval of the sensor is 15s

}
