#include <WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>

  #define SS_PIN 5
  #define RST_PIN 22
  #define sensor_rfid 12
  #define pin_Buzzer  4

//const char* ssid     = "HUAWEI.....44....M";
//const char* password = "Paraque.";

const char* ssid     = "INFINITUM756B";
const char* password = "3RnKKAFUsD";


const String UserReg_1 = "204F8949";
const String UserReg_2 = "17420BA9";
const String UserReg_3 = "2BF10AA9";


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];

String DatoHex;
int rfid_enable=0;
int canal=0, frec=2000, resolucion=8;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println(F("RFID prueba de conexión con el servidor"));
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conexión OK!");
  Serial.print("IP Local: ");
  Serial.println(WiFi.localIP());
  


  //------RFID------
  Ajustes();
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

  if ( ! rfid.PICC_IsNewCardPresent()){return;}
     
     // Verify if the NUID has been readed
     if ( ! rfid.PICC_ReadCardSerial()){return;}
     if(rfid_enable==1){
     Serial.print(F("Estado: "));
     MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //   Serial.println(rfid.PICC_GetTypeName(piccType));
     // Check is the PICC of Classic MIFARE type
//     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
//     {
//       Serial.println("Su Tarjeta no es del tipo MIFARE Classic.");
//       return;
//     }
    
     if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] )
     {
       Serial.println("Se ha detectado una nueva tarjeta.");
       
       // Store NUID into nuidPICC array
       for (byte i = 0; i < 4; i++) {nuidPICC[i] = rfid.uid.uidByte[i];}
    
       DatoHex = printHex(rfid.uid.uidByte, rfid.uid.size);
       Serial.print("Codigo Tarjeta: "); Serial.println(DatoHex);

         EnvioDatos();
        RecepcionDatos(); 
       if(UserReg_1 == DatoHex)
       {

      
        Serial.println("PUEDE INGRESAR"); 
            Buzzer_acceso();
       }
       else if(UserReg_2 == DatoHex)
       {
        
        Serial.println("PUEDE INGRESAR");
        Buzzer_acceso();
       }
       else if(UserReg_3 == DatoHex)
       {
        Serial.println("PUEDE INGRESAR");
        Buzzer_acceso();
       } 
       else
       {
        Serial.println("NO ESTA REGISTRADO - PROHIBIDO EL INGRESO--- ALERTA");
        Buzzer_no_acceso();
       }  
       Serial.println();
     }
     else 
     {
      Serial.println("Tarjeta leida previamente");
     }
     rfid_enable=0;
     }
     // Halt PICC
     rfid.PICC_HaltA();
     // Stop encryption on PCD
     rfid.PCD_StopCrypto1();
 
}




void EnvioDatos(){
  if (WiFi.status() == WL_CONNECTED){ 
     HTTPClient http;  // creo el objeto http
     String datos_a_enviar = "Registro=" + String(DatoHex);  

     http.begin(client,"http://asistencia-rfid-ades.000webhostapp.com/EspPost.php");
     http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // defino texto plano..

     int codigo_respuesta = http.POST(datos_a_enviar);

     if (codigo_respuesta>0){
      Serial.println("Código HTTP: "+ String(codigo_respuesta));
        if (codigo_respuesta == 200){
          String cuerpo_respuesta = http.getString();
          Serial.println("El servidor respondió: ");
          Serial.println(cuerpo_respuesta);
        }
     } else {
        Serial.print("Error enviado POST, código: ");
        Serial.println(codigo_respuesta);
     }

       http.end();  // libero recursos
       
  } else {
     Serial.println("Error en la conexion WIFI");
  }
  delay(100); //espera 60s
}


void RecepcionDatos(){
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
//En la variable "&DatoHex=" se almacena el codigo RFID detectado
    
    HTTPClient http;

    String datos_a_enviar = "DatoHex=" + String(DatoHex);  

     http.begin(client,"http://asistencia-rfid-ades.000webhostapp.com/EspGet.php");
     http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // defino texto plano..

     int codigo_respuesta_get = http.POST(datos_a_enviar);

     if (codigo_respuesta_get>0){
      Serial.println("Código HTTP GET: "+ String(codigo_respuesta_get));
        if (codigo_respuesta_get == 200){
          String cuerpo_respuesta_get = http.getString();
          Serial.println("El servidor para GET respondió: ");
          Serial.println(cuerpo_respuesta_get);
        }
     } else {
        Serial.print("Error enviado POST, código: ");
        Serial.println(codigo_respuesta_get);
     }

       http.end();  // libero recursos
       Serial.println("-------Asistencia registrada--------");
  }
  else {
     Serial.println("Error en la conexion WIFI");
  }
  delay(100); //espera 60s
  
    


   // ------PARTE GET------

   if(WiFi.status()== WL_CONNECTED){ 
    HTTPClient http;
    http.begin("http://asistencia-rfid-ades.000webhostapp.com/EspGet.php");        //Indicamos el destino
    http.addHeader("Content-Type", "plain-text"); //Preparamos el header text/plain si solo vamos a enviar texto plano sin un paradigma llave:valor.

    int codigo_respuesta = http.GET();   //Enviamos el post pasándole, los datos que queremos enviar. (esta función nos devuelve un código que guardamos en un int)

    if(codigo_respuesta>0){
      Serial.println("Código HTTP ► " + String(codigo_respuesta));   //Print return code

      if(codigo_respuesta == 200){
        String cuerpo_respuesta = http.getString();
        Serial.println("El servidor respondió ▼ ");
        Serial.println(cuerpo_respuesta);

      }

    }else{

     Serial.print("Error enviando POST, código: ");
     Serial.println(codigo_respuesta);

    }

    http.end();  //libero recursos
   }   else{Serial.println("Error en la conexión WIFI");

  }

   delay(100);
  }



void Ajustes(){
    pinMode(sensor_rfid, INPUT_PULLUP);
   rfid_enable=0;
   SPI.begin(); // Init SPI bus
   rfid.PCD_Init(); // Init MFRC522
   Serial.println();
   Serial.print(F("Reader :"));
   attachInterrupt(sensor_rfid, isr_sensor_rfid, FALLING);
   rfid.PCD_DumpVersionToSerial();
   for (byte i = 0; i < 6; i++) {
     key.keyByte[i] = 0xFF;
   } 
   DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
   Serial.println();
   Serial.println();
   Serial.println("Iniciando el Programa de Lectura RFID");
}






String printHex(byte *buffer, byte bufferSize)
{  
   String DatoHexAux = "";
   for (byte i = 0; i < bufferSize; i++) 
   {
       if (buffer[i] < 0x10)
       {
        DatoHexAux = DatoHexAux + "0";
        DatoHexAux = DatoHexAux + String(buffer[i], HEX);  
       }
       else { DatoHexAux = DatoHexAux + String(buffer[i], HEX); }
   }
   
   for (int i = 0; i < DatoHexAux.length(); i++) {DatoHexAux[i] = toupper(DatoHexAux[i]);}
   return DatoHexAux;
}

void isr_sensor_rfid()
{

rfid_enable=1;
}

void Buzzer_acceso()
{
  int canal=0, frec=3000, resolucion=8;
 ledcSetup(canal,frec,resolucion);
 ledcAttachPin(pin_Buzzer, 0);
int frecuenciaIncrementa = 0;
  //Ciclo que incrementara de 0 a 10
  for (int i = 0; i< 4; ++i) {
    //Enviamos un tono 
    ledcWriteTone(0, frec);
    //La frecuencia incrementara de 30 en 30
    frecuenciaIncrementa += 30;
    //Esperamos 1 segundo
    delay(500);
  }
  frec=0;
  ledcSetup(canal,frec,resolucion);
 ledcAttachPin(pin_Buzzer, 0);
}

void Buzzer_no_acceso()
{
 int canal=0, frec=500, resolucion=8;
 ledcSetup(canal,frec,resolucion);
 ledcAttachPin(pin_Buzzer, 0);
int frecuenciaIncrementa = 0;
  //Ciclo que incrementara de 0 a 10
  for (int i = 0; i< 1; ++i) {
    //Enviamos un tono 
    ledcWriteTone(0, frec);
    delay(500);
  }
  frec=0;
  ledcSetup(canal,frec,resolucion);
 ledcAttachPin(pin_Buzzer, 0);
}
