/*
  |Max32664 pin label| Arduino Connection  |Pin Function      |
  |----------------- |---------------------|------------------|
  | SDA              | A4                  |  Serial Data     |
  | SCL              | A5                  |  Serial Clock    |
  | Vin              | 5V                  |  Power           |
  | GND              | Gnd                 |  Gnd             |
  | MFIO Pin         | 02                  |  MFIO            |
  | RESET Pin        | 04                  |  Reset           |
  |-----------------------------------------------------------|

  The following variables are automatically generated and updated when changes are made to the Thing: 
    - float DIAreading
    - float HRreading 
    - float O2reading
    - float SYSreading 
    - bool VBswitch (soon)

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include <Wire.h>
#include "max32664.h"

#define RESET_PIN 04
#define MFIO_PIN 02
#define RAWDATA_BUFFLEN 250
#include "thingProperties.h"

max32664 MAX32664(RESET_PIN, MFIO_PIN, RAWDATA_BUFFLEN);

void mfioInterruptHndlr(){
  //Serial.println("i");
}

void enableInterruptPin(){

  //pinMode(mfioPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MAX32664.mfioPin), mfioInterruptHndlr, FALLING);

}

void loadAlgomodeParameters(){

  algomodeInitialiser algoParameters;
  /*  Replace the predefined values with the calibration values taken with a reference spo2 device in a controlled environt.
      Please have a look here for more information, https://pdfserv.maximintegrated.com/en/an/an6921-measuring-blood-pressure-MAX32664D.pdf
      https://github.com/Protocentral/protocentral-pulse-express/blob/master/docs/SpO2-Measurement-Maxim-MAX32664-Sensor-Hub.pdf
  */

  algoParameters.calibValSys[0] = 120;
  algoParameters.calibValSys[1] = 122;
  algoParameters.calibValSys[2] = 125;

  algoParameters.calibValDia[0] = 80;
  algoParameters.calibValDia[1] = 81;
  algoParameters.calibValDia[2] = 82;

  algoParameters.spo2CalibCoefA = 1.5958422;
  algoParameters.spo2CalibCoefB = -34.659664;
  algoParameters.spo2CalibCoefC = 112.68987;

  MAX32664.loadAlgorithmParameters(&algoParameters);
}

void setup(){

  Serial.begin(57600);
  
  initProperties();
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  Wire.begin();

  loadAlgomodeParameters();

  int result = MAX32664.hubBegin();
  if (result == CMD_SUCCESS){
    Serial.println("Sensorhub begin!");
  }else{
    //stay here.
    while(1){
      Serial.println("Could not communicate with the sensor! please make proper connections");
      delay(5000);
    }
  }

  bool ret = MAX32664.startBPTcalibration();
  while(!ret){

    delay(10000);
    Serial.println("failed calib, please retsart");
    //ret = MAX32664.startBPTcalibration();
  }

  delay(1000);

  //Serial.println("start in estimation mode");
  ret = MAX32664.configAlgoInEstimationMode();
  while(!ret){

    //Serial.println("failed est mode");
    ret = MAX32664.configAlgoInEstimationMode();
    delay(10000);
  }

  //MAX32664.enableInterruptPin();
  Serial.println("Getting the device ready..");
  delay(1000);
}

void loop(){

  uint8_t num_samples = MAX32664.readSamples();

  if(num_samples){

    SYSreading = MAX32664.max32664Output.sys;
    DIAreading = MAX32664.max32664Output.dia;
    HRreading = MAX32664.max32664Output.hr;
    O2reading = MAX32664.max32664Output.spo2;
  }

  delay(100);
}

void onVBswitchChange() {
  
}
