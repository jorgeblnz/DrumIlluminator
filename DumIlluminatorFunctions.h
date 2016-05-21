/******************************************************
*  Archivo de cabecera de mis funciones para Arduino
* Archivo:	DumIlluminatorFunctions.h
* Autor:	Jorge Ignacio Balanz Pino
* Fecha:	mayo de 2016
* Descripción: 	Colección de funciones que me 
 				facilitarán las tareas de programación 
				(definidas en DumIlluminatorFunctions.cpp).
*
******************************************************/
//Librerías a importar:
#include "Arduino.h"  //Viene de serie en Arduino IDE
#include <Wire.h>     //Viene de serie en Arduino IDE
#include <LiquidCrystal_I2C.h> //Debe cargarse: Programa->Incluir Librería->Gestionar Librerías ---> 

//CONSTANTES 

// VARIABLES GLOBALES


/***************************************************
*	ESCRIBIR EN EL DISPLAY 
* 	La librería LiquidCrystal_I2C.h no escribe
* cadenas de caracteres (strings) ni números
* enteros: sólo el primere carácter/dígito.
* 	Estas funciones escriben el número/string 
* al completo.
****************************************************/

/**
* Función que escribe una cadena de caracteres completa (o 
* lo que entre en la línea) en el display.
* @param txt Cadena de caracteres a escribir.
* @param line Fila del display donde escribirá (la primera es la 0).
* @param pos Posición (columna) dentro de la línea donde empieza a escribir.
*/
void LCDwriteText(LiquidCrystal_I2C lcd, String txt, int line=0, int pos=0);
/**
* Función que escribe una cadena de caracteres completa (hasta
* 24 caracteres sin saltar de línea) en el display con scroll.
* El texto se va mostrando en la pantalla con movimiento.
*
* @param txt Cadena de caracteres a escribir.
* @param line Fila del display donde escribirá (la primera es la 0).
* @param cols Número de caracteres (columnas) que tiene una línea (para empezar a escribir).
*/
void LCDwriteTextScroll(LiquidCrystal_I2C lcd, String txt, int line=0, int cols=16);
/**
* Función que escribe un número entero completo (o 
* lo que entre en la línea) en el display.
* @param value Número entero (long) a escribir.
* @param line Fila del display donde escribirá (la primera es la 0).
* @param pos Posición (columna) dentro de la línea donde empieza a escribir.
*/
void LCDwriteNumber(LiquidCrystal_I2C lcd, long value, int line=0, int pos=0);

