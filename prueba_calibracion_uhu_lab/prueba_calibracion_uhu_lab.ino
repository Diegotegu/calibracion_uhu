

//-------------------------------------------------------------------------------------
// experimento para calibracion de sensores de suelo en UHU
//se pone 2000g de tierra en un vaso
//se pone a funcionar el arduino con la SD, 1 sensor en el medio del vaso y 1 galga
//se añaden XXXXX g de agua de forma que quede uniforme con lo que se empieza con ZZZZZ de contenido de agua gravimétrico
//se deja secar al aire hasta que se estabilice la medida de peso a 2000g de nuevo
//repetir operación con el sensor en un lateral del vaso
//-------------------------------------------------------------------------------------

//para poner este código en funcionamiento correctamente solo hay que fijar la fecha y la hora


#include <HX711_ADC.h>
#include <RTCZero.h>
#include <SPI.h>
#include <SD.h>

/* Create an rtc object */
RTCZero rtc;



/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 30;
const byte hours = 11;

/* Change these values to set the current initial date */
const byte day = 14;
const byte month = 11;
const byte year = 19;
//HX711 constructor (dout pin(verde), sck pin(naranja))
HX711_ADC basc1(1, 0); //HX711 1
#define freq_msg 60 //Frequency (in minutes for message)
boolean debugger = false;
const int chipSelect = 7;
const float calValue_1 = -395.63; // calibration value load cell 1
const String cabecera = "dia,mes,anio,hora,minuto,basc,sens1,sens2";


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);// Configuring the onboard LED for status info (led on= sd card initialized // led flash= writing on SD
  digitalWrite(LED_BUILTIN, LOW);
 
  if (debugger) {
    Serial.begin(9600);
    while (!Serial);
  }
  Serial1.begin(9600);
  delay(10);
  
  //////////////////////////////////////////////////////////Basculas
  basc1.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  basc1.start(stabilisingtime);
  byte basc1_rdy = 0;
  if (debugger) {
    if (basc1.getTareTimeoutFlag()) {
      Serial.println("Tare timeout, check MCU>HX711 no.1 wiring and pin designations");
    }
  }
  basc1.setCalFactor(calValue_1); // user set calibration value (float)
  if (debugger) {
    Serial.println("Startup + tare is complete");
  }

  rtc.begin(); // initialize RTC
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);
    if (debugger) {
    Serial.println("RTC started: date and time are set");
  }

 if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  File archivo = SD.open("datalog.txt", FILE_WRITE);
  if (archivo) {
    archivo.println(cabecera);
    archivo.close();
  }
  
}


void loop() {
  float a, b, c;
  String datos="";
  basc1.begin();
  basc1.setCalFactor(calValue_1);
  a=0;
  b=0;
  c=0;
  for (int i = 0; i < 100; i++) {
    basc1.update();
    delay(100);
    a += basc1.getData();
    b += analogRead(A0);
    c += analogRead(A1);
  }
  a /= 100;
  b /= 100;
  c /= 100;
  
  datos=String(rtc.getDay())+","+String(rtc.getMonth())+","+String(rtc.getYear())+","+String(rtc.getHours())+","+String(rtc.getMinutes())+","+String(a)+","+String(b)+","+String(c);

  if (debugger) {
    Serial.println("--------------------------------------------------------------------------------------------------------------");
    Serial.println("medida desde los sensores:");
    Serial.println("bascula: " + String(a));
    Serial.println("sensor A0: " + String(b));
    Serial.println("sensor A1: " + String(c));
    Serial.println("--------------------------------------------------------------------------------------------------------------");
    Serial.println("escribo en la SD:");
    Serial.println(datos);
    Serial.println("--------------------------------------------------------------------------------------------------------------");
  }
 
File archivo = SD.open("datalog.txt", FILE_WRITE);
  if (archivo) {
    archivo.println(datos);
    archivo.close();
  }
 
  //delay(1000);
}


void dummy() {
  volatile int aaa = 0;
}


void reboot() {
  NVIC_SystemReset();
  while (1);
}
