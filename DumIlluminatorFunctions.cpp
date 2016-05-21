/***************************************************
*	Colección de Funciones para Arduino
* Archivo:	DumIlluminatorFunctions.cpp
* Autor:	Jorge Ignacio Balanz Pino
* Fecha:	mayo de 2016
* Descripción: 	Colección de funciones que me 
 				facilitarán las tareas de programación.
*
****************************************************/

#include "DumIlluminatorFunctions.h" 

/***************************************************
*  ESCRIBIR EN EL DISPLAY 
*   La librería LiquidCrystal_I2C.h no escribe
* cadenas de caracteres (strings) ni números
* enteros: sólo el primer carácter/dígito.
*   Estas funciones escriben el número/string 
* al completo.
****************************************************/

/**
* Función que escribe una cadena de caracteres completa (o 
* lo que entre en la línea) en el display.
* @param lcd Objeto LiquidCrystal_I2C (LCD con interfaz I2C) convenientemente creado e inicializado.
* @param txt Cadena de caracteres a escribir.
* @param line Fila del display donde escribirá (la primera es la 0).
* @param pos Posición (columna) dentro de la línea donde empieza a escribir.
*/
void LCDwriteText(LiquidCrystal_I2C lcd, String txt, int line, int pos){
  char buff[17];
  int i;
  txt.toCharArray(buff,17);
  if(line>0)
    line=1;
  if(line<1)
    line=0; 
  lcd.setCursor(pos,line);
  for(i=0; i+pos<17 && buff[i]!='\0'; i++){
    lcd.print(buff[i]);
  }
  //lcd.close();
}

//Hasta 24 caracteres en una línea(luego salta)
/**
* Función que escribe una cadena de caracteres completa (hasta
* 24 caracteres sin saltar de línea) en el display con scroll.
* El texto se va mostrando en la pantalla con movimiento.
*
* @param lcd Objeto LiquidCrystal_I2C (LCD con interfaz I2C) convenientemente creado e inicializado.
* @param txt Cadena de caracteres a escribir.
* @param line Fila del display donde escribirá (la primera es la 0).
* @param cols Número de caracteres (columnas) que tiene una línea (para empezar a escribir).
*/
void LCDwriteTextScroll(LiquidCrystal_I2C lcd, String txt, int line, int cols){
  char buff[127];
  unsigned int i;
  txt.toCharArray(buff,txt.length()+1);
  if(line>0)
    line=1;
  if(line<1)
    line=0; 
  lcd.setCursor(cols,line);
   // ponemos el display con scroll automático:
  lcd.autoscroll();
  for(i=0; i<txt.length() && buff[i]!='\0'; i++){
    lcd.print(buff[i]);
    delay(50);
  }
   // quitamos el scrolling automático:
  lcd.noAutoscroll();
}

/**
* Función que escribe un número entero completo (o 
* lo que entre en la línea) en el display.
* @param lcd Objeto LiquidCrystal_I2C (LCD con interfaz I2C) convenientemente creado e inicializado.
* @param value Número entero (long) a escribir.
* @param line Fila del display donde escribirá (la primera es la 0).
* @param pos Posición (columna) dentro de la línea donde empieza a escribir.
*/
void LCDwriteNumber(LiquidCrystal_I2C lcd, long value, int line, int pos){
  LCDwriteText(lcd,String(value,DEC),line,pos);
}


