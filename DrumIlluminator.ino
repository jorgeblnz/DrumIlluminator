/****************************************************
*  Programa "DrumIlluminator.ino" para Arduino UNO
*
* Archivo:  DrumIlluminator.ino
* Autor:    Jorge Ignacio Balanz Pino
* Fecha:    mayo de 2016
* Descripción: Programa principal para Arduino UNO.
*     Hace que se ilumine una batería acústica,
*     conectando unos sensores piezoeléctricos a
*     cada tambor (o componente) de la batería e
*     introduciendo en su interior unos LED.
*
****************************************************/

//Incluyo una librería de creación propia,
//  desde la que se hacen el resto 
//  de importaciones necesarias:
#include "DumIlluminatorFunctions.h"

//Cabeceras de funciones (definidas en este archivo):
void showLogo(boolean withserial=false);
void updateThresholds(boolean up);
void checkTimers();
void checkButtons();

//Constantes
const int thresUpPin = 9;  // Pin Arduino para subir los umbrales.
const int thresDwnPin = 10; // Pin Arduino para bajar los umbrales.
const int testPin = 11;  // Pin Arduino para lanzar un test de los LED.
const int lightPinIn = 12; // Pin Arduino para poner o quitar la iluminación constante (ENTRADA).
const int lightPinOut = 8; // Pin Arduino para poner o quitar la iluminación constante (SALIDA).
const int signaling = 13; //Pin Arduino que se utilizará para algunas señalizaciones.
const boolean T_UP = true;  // alias de true para simbolizar subida de umbral
const boolean T_DOWN = false;  // alias de false para simbolizar bajada de umbral
const int THRES_INTERVAL = 50; // Unidades en que se aumentan o disminuyen los umbrales
const long REPOSO = 5000; //Tiempo de inactividad que se considera para reposo.
const int LIGHT_DELAY = 100; //Tiempo que los LED se quedan encendidos cuando un sensor detecta algo.
const boolean __DEBUG = true; //Indica si se debe escribir información de debug por el puerto serie
const boolean __WITH_LCD = false; //Indica si hay un LCD conectado al puerto I2C (SDA-SCL o A4-A5)
const boolean __ACTIV_LCD = true; //Indica si se debe escribir información de entradas analógicas en el LCD (sólo si __WITH_LCD=true)

//Variables globales:
LiquidCrystal_I2C lcd(0x27,16,2);  // Establece la dirección 0x27 para el LCD de 16 caracteres y dos líneas. La variable "lcd" es el Objeto LCD 

byte ledarray[] = {2,3,4,5,6,7}; //Salidas para los LED que se deben encender.
int thresholds[] = {500, 500, 500, 500, 500, 500}; //Array de 6 enteros que representan los umbrales que indican si se enciende un LED.
int analogics[] = {A0,A1,A2,A3,A4,A5}; //Array de entradas analógicas a escanear.
long lightstime[] = {0,0,0,0,0,0}; //Array de temporizadores para dejar los LED: recoge los instantes en que debe apagarse cada LED.
int lastAnalogIndex = 5; //Índice en el que se terminan de escanear entradas analógicas.
long ultimaactividad = 0; //Tiempo (timestamp) para controlar el tiempo de inactividad y presentar logo en el LCD (si está conectado).

/*** INICIALIZACIÓN ***/
void setup()
{
  /*
   * MISCELÁNEA
   */
   //Inicializar Serial:
  if(__DEBUG){
     Serial.begin (9600); //Inicializamos comunicación serie
  }

  /*
   * DISPLAY (LCD 16x2)
   */
  if(__WITH_LCD){
    //Si se ha utiliza el LCD, sólo se usan 4 entradas analógicas como máximo
    lastAnalogIndex = (lastAnalogIndex>3 ? 3 : lastAnalogIndex); //A4 y A5 se usan para el LCD (I2C 0x27 / SCL-SDA)
    initializeLCD(); //Inicio y Presentación en el LCD
  }
  //Mostramos presentación
  showLogo(true);
  delay(500);

  //Establecemos el modo de los pines
  for(int i=0; i<=lastAnalogIndex; i++){
    pinMode(ledarray[i], OUTPUT);
  }
  //Pin de señalización
  pinMode(signaling,OUTPUT); //Este pin señalizará ciertas actividades

  //Pines para funciones añadidas
  pinMode(thresUpPin, INPUT);
  pinMode(thresDwnPin, INPUT);
  pinMode(testPin, INPUT);
  pinMode(lightPinIn, INPUT);
  pinMode(lightPinOut, OUTPUT);//Salida!!

  //Hacemos una comprobación de LED al inicio:
  checkLEDS(1000);
  
} //Fin SETUP

/*** ITERACIÓN ***/
void loop()
{
  
  //Comprobamos los diferentes temporizadores:
  checkTimers();

  //Comprobamos los botones:
  checkButtons();
 
  //REALIZAMOS LECTURA DE LAS ENTRADAS ANALÓGICAS
  checkAnalogics();
  
} //Fin LOOP


/********************************************
 *          FUNCIONES AUXILIARES            *
 ********************************************/

////////   PROCESAR I/O   ///////////////////
/**
 * Hacer una lectura de las entradas analógicas y 
 * establecer en HIGH las salidas apropiadas:
 */
void checkAnalogics(){
  int lectura = 0;

  //Se comprueba cada entrada analógica:
  for(int i=0; i<=lastAnalogIndex; i++){
    //Sólo se hace lectura de la entrada analógica si el temporizador está caducado (es 0)
    if(lightstime[i]==0){
      lectura = analogRead(analogics[i]); //Leemos entrada analógica.
      //Comprobamos si supera su umbral
      if(lectura>thresholds[i]){
        //Si supera el umbral:
        digitalWrite(ledarray[i], HIGH); //Encendemos su LED
        lightstime[i] = millis()+LIGHT_DELAY; //Ponemos su temporizador
        //Presentamos el evento
        if(__DEBUG){
          Serial.println("Umbral superado en 'A"+ String(i) +"': "+String(lectura));
        }
        //Presentarlo en el LCD
        if(__WITH_LCD){
          ultimaactividad = millis()+REPOSO;
          if(__ACTIV_LCD){ //Sólo si hay que indicar la actividad por el LCD      
            lcd.clear();
            LCDwriteText(lcd,"Lectura de 'A"+ String(i) +"':",0,0);
            LCDwriteNumber(lcd,lectura,1,4);
          }
        }
      }
      else{
        digitalWrite(ledarray[i], LOW); //Apagamos su LED
      }
    }
  }
}

/**
 * Comprobar si se ha pulsado alguno de los botones
 */
void checkButtons(){
  int aux = digitalRead(thresUpPin);
  
  //Controlamos si se solicita cambio de umbrales
  if(aux == HIGH){
    updateThresholds(T_UP); //BOTÓN SUBIR
    delay(500); //Pulsación de 1/2 segundo máximo
  }
  aux = digitalRead(thresDwnPin); //BOTÓN BAJAR
  if(aux == HIGH){
    updateThresholds(T_DOWN);
    delay(500);
  }
  //Controlamos si se solicita Test de LED
  aux = digitalRead(testPin); //BOTÓN TEST
  if(aux == HIGH){
    checkLEDS(500);
  }
  //Controlamos si se solicita iluminación constante
  aux = digitalRead(lightPinIn); //BOTÓN Iluminación
  if(aux == HIGH){
    aux=digitalRead(lightPinOut);
    if(aux==HIGH){
      digitalWrite(lightPinOut,LOW);
      if(__WITH_LCD){
        ultimaactividad = millis()+REPOSO;
        lcd.clear();
        LCDwriteText(lcd,"Apagar Luces!",0,2);
      }
      if(__DEBUG){
        Serial.println("----------------------------------");
        Serial.println("    >>>>  Apagar Luces!! <<<<");  
        Serial.println("----------------------------------");
      }
    }
    else{
      digitalWrite(lightPinOut,HIGH);
      if(__WITH_LCD){
        ultimaactividad = millis()+REPOSO;
        lcd.clear();
        LCDwriteText(lcd,"Encender Luces!",0,1);
      }
      if(__DEBUG){
        Serial.println("----------------------------------");
        Serial.println("    >>>>  Encender Luces!! <<<<");  
        Serial.println("----------------------------------");
      }
    }
    delay(500); //Tiempo para que se suelte el pulsador.
  }  
}

///////////  MODIFICAR UMBRALES   ///////////////////
/**
 * Subir o bajar los umbrales a superar.
 * El porcentaje de incremento se establece en la constante THRES_INTERVAL.
 */
void updateThresholds(boolean up){
  if(up){
    for(int i=0; i<6; i++){
      thresholds[i] += THRES_INTERVAL;
      if(thresholds[i]>1000) //Limitamos el rango
        thresholds[i]=1000;
    }
  }
  else{
    for(int i=0; i<6; i++){
      thresholds[i] -= THRES_INTERVAL;
      if(thresholds[i]<THRES_INTERVAL)
        thresholds[i]=THRES_INTERVAL;
    }
  }
  //Presentamos los cambios, si procede:
  if(__WITH_LCD){
    String s = "A"+String(thresholds[0])+"; C"+String(thresholds[2])+"; E"+String(thresholds[4]);
    lcd.clear();
    LCDwriteText(lcd,"Cambio Umbrales:",0,0);
    LCDwriteText(lcd,s,1,0);
    ultimaactividad = millis()+REPOSO;
  }
  if(__DEBUG){
    Serial.println("----------------------------------");
    Serial.print(">>>> Cambio de Umbrales ");
    if(up) 
      Serial.println("(UP):");
    else
      Serial.println("(DWN):");  
    for(int i=0; i<6; i++){
      Serial.println("\t@ A"+String(i)+" --> "+String(thresholds[i]));
    }
    Serial.println("----------------------------------");
  }
}

/********************************
*			DISPLAY
*********************************/
/////////    MENSAJES DISPLAY   /////////////////////
/**
 * Inicializar/Presentación inicial del LCD
 */
void initializeLCD(){
    // Inicializamos Display
    lcd.init();  // Inicializamos el LCD 
    lcd.backlight();
    // Mensaje de bienvenida.
    lcd.clear();
    LCDwriteText(lcd,"Bienvenido al...",0,0);
    delay(500);
    LCDwriteTextScroll(lcd,"................",0,0);
    lcd.clear();
    LCDwriteTextScroll(lcd,"Drum-Illuminator",0,0);
    delay(500);
    lcd.clear();
    LCDwriteText(lcd,"Drum-Illuminator",0,0);
    LCDwriteText(lcd,"By Jorge Balanz!",1,0);
    for(int i=0; i<3; i++){
      lcd.noDisplay();
      delay(300);
      lcd.display();
      delay(500);
    }//Fin for
}
/**
 * Presentar en el display un texto de espera/reposo:
 */
void showLogo(boolean withserial){
  if(__WITH_LCD){
    lcd.clear();
    LCDwriteText(lcd,"Drum-Illuminator",0,0);
    LCDwriteText(lcd,"By Jorge Balanz!",1,0);
  }
  if(withserial && __DEBUG){
    Serial.println("***********************************************");
    Serial.println("      Bienvenido al DrumIlluminator!");
    Serial.println("         (By Jorge Balanz)");
    if(!__WITH_LCD)
      Serial.println("      Funcionamiento SIN Display LCD.");
    Serial.println("***********************************************\n");
  }
}

/***********
*    OTROS
************/
/**
 * Comprobar los temporizadores
 */
void checkTimers(){
  long t = 0;

  t = millis(); //Momento actual
   
  //Controlamos si se ha superado tiempo inactividad
  //Esto sólo se utiliza para presentar el "logo" en el
  // display LCD
  if(__WITH_LCD && (ultimaactividad>0) && (t > ultimaactividad)){
    showLogo(); //Texto de estado "en reposo"
    ultimaactividad = 0; //Reseteo para refrescar sólo una vez
    //Lo selñalizo en el pin 13
    digitalWrite(signaling, HIGH);
    delay(LIGHT_DELAY); //Lo señalizo el tiempo que dura encendido un tambor
    digitalWrite(signaling, LOW);
  }

  //Comprobamos los temporizadores de los LED que estén encendidos:
  for (int k=0; k<=lastAnalogIndex; k++){
    if(t>lightstime[k]){
      lightstime[k] = 0; // Ponemos el temporizador de esa luz a 0
    }
  }
  
}
/**
 * Hacer un chequeo de los colores de los LEDs
 * El chequeo debe encender las luces en el siguiente orden: 
 *  Verde, Azul, Rojo,
 *  VerdeAzul, RojoAzul, VerdeRojo,
 *  VerdeAzulRojo.
 * 
 * @param arr Array de Bytes que se utilizará para cargar los datos en los registros de desplazamiento
 * @param dely Tiempo de espera con cada color de prueba.
 * @param lcd Display LCD (objeto) donde se imprimirá información del test.
 */
void checkLEDS(int dely){
  if(__WITH_LCD){
    lcd.clear();
    LCDwriteText(lcd,"Comienza el Test:",0,0);
    delay(1500);
  }
  if(__DEBUG){
    Serial.println("**********************************************************");
    Serial.println("   Comienza el Test de LED...");
    Serial.println("\n\tENCENDER: ");
  }
  for(int i=0; i<=lastAnalogIndex; i++){
    digitalWrite(ledarray[i],HIGH);
    if(__WITH_LCD){
      lcd.clear();
      LCDwriteText(lcd,"ENCIENDE LED "+String(i),0,1);
    }
    if(__DEBUG){
      Serial.print("\tLED "+String(i));
    }
    delay(dely);
  }
  if(__DEBUG){
    Serial.println("\n\tAPAGAR: ");
  }
  for(int i=0; i<=lastAnalogIndex; i++){
    digitalWrite(ledarray[i],LOW);
    if(__WITH_LCD){
      lcd.clear();
      LCDwriteText(lcd,"APAGA LED "+String(i),0,2);
    }
    if(__DEBUG){
      Serial.print("\tLED "+String(i));
    }
    delay(dely);    
  }
  if(__WITH_LCD){
    lcd.clear();
    LCDwriteText(lcd,"TODOS!",0,3);
  }
  if(__DEBUG){
    Serial.print("\n\tTODOS JUNTOS!");
  }
  for(int i=0; i<=lastAnalogIndex; i++){
    digitalWrite(ledarray[i],HIGH);
  }
  delay(dely);    
  for(int i=0; i<=lastAnalogIndex; i++){
    digitalWrite(ledarray[i],LOW);
  }
  if(__WITH_LCD){
    showLogo();
  }
  if(__DEBUG){
    Serial.println("\n\t\tTest Finalizado!");
    Serial.println("**********************************************************\n");
  }
}

