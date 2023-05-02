
#include <Arduino.h>

#define SENSOR_PIN_entrada 12
#define LED_OUT_entrada    13

#define SENSOR_PIN_salida 27
#define LED_OUT_salida    14

#define sensor_rfid 33

//ESTE YA JALA CHIDO 
//EL ESP FUNCIONA DE INTERMEDIARIO PARA PRENDER EL LED SEGÚN LA SEÑAL DEL SENSOR 
int sensor_entrada=0;
int sensor_salida=0;
int entrada=0;
int salida=0; 


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);

//ARREGLO PARA SENSOR QUE DETECTA ENTRADAS

pinMode(SENSOR_PIN_entrada, INPUT_PULLUP);
pinMode(LED_OUT_entrada, OUTPUT);
pinMode(sensor_rfid, OUTPUT);
digitalWrite(LED_OUT_entrada,HIGH);
digitalWrite(sensor_rfid,LOW);
attachInterrupt(SENSOR_PIN_entrada, isr_entrada, FALLING);


//ARREGLO PARA SENSOR QUE DETECTA SALIDAS

pinMode(SENSOR_PIN_salida, INPUT_PULLUP);
pinMode(LED_OUT_salida, OUTPUT);
digitalWrite(LED_OUT_salida,HIGH);
attachInterrupt(SENSOR_PIN_salida, isr_salida, FALLING);
}

void isr_entrada()
{

 if (salida==1){
  digitalWrite(LED_OUT_salida,LOW);
  sensor_salida++;
  delay(1000);
  entrada=0;
  salida=0;
 }
 
  entrada=1;


}

void isr_salida()
{
 
 if (entrada==1){
  digitalWrite(LED_OUT_entrada,LOW);
  digitalWrite(sensor_rfid,HIGH);
  sensor_entrada++;
  delay(1000);
  
   entrada=0;
  salida=0;
  delay(5000);
 }
 else
  salida=1;
 
}

void loop() {
  // put your main code here, to run repeatedly:

int entrada=0;
int salida=0;


digitalWrite(LED_OUT_entrada,HIGH);
digitalWrite(LED_OUT_salida,HIGH);
digitalWrite(sensor_rfid,LOW);
Serial.println(sensor_entrada);
Serial.println(sensor_salida);

 
} 
