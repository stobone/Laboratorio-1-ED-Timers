// PROYECTO INSTRUMENTAL 2
// Laboratorio #1: Timers
// Módulo: Electrónica Digital
// Realizado por: 
              //Cristian Lopera Trujillo
              //Jose Manuel Machado Loaiza
              //Sebastián Tobón Echavarría

#include <Arduino.h>
#include <avr/io.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

// Variables para interrupción del timer
volatile unsigned int cont = 0;
unsigned int overflow = 0;

// Inicialización de pines LCD
LiquidCrystal lcd(47, 49, 45, 43, 41, 39);  // RS, E, D4, D5, D6, D7

// Configuración de teclado
const byte FILAS = 4;
const byte COLUMNAS = 3;
char keys[FILAS][COLUMNAS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pinesFilas[FILAS] = {18, 19, 20, 21}; // A, B, C, D
byte pinesColumnas[COLUMNAS] = {17, 16, 15}; // 1, 2, 3

Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// Variables para ingresar el tiempo
char tecla;
int cifras[4];
byte indice = 0;

// Configuración de indicadores
#define ledPin 2
#define motorPin 3

void setup() {

  // Configuración inicial de la LCD
  lcd.begin(16, 2);            
  lcd.setCursor(0,0);
  lcd.print("Horno microondas");
  lcd.setCursor(0,1);
  lcd.print(0);
  lcd.setCursor(1,1);
  lcd.print(0);
  lcd.setCursor(2,1);
  lcd.print(":");
  lcd.setCursor(3,1);
  lcd.print(0);
  lcd.setCursor(4,1);
  lcd.print(0);

  // Configuración de los registros del timer 1    
  TCCR1A = 0;                           // El registro de control A queda todo en 0
  TCCR1B = 0;                           // Limpia el registrador
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Configura prescaler para 1024
 
  TCNT1 = 0xE17B;                       // Inicia timer para desbordamiento 1 segundo
                                        // 57723 = 0xE17B

  // Indicadores
  pinMode(motorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
};

void loop() {
  
  // Configuración para ingresar el tiempo y visualizar en la LCD
  tecla = teclado.getKey();  
  int tecla_int = String(tecla).toInt();
  if (tecla && tecla != '*' && tecla != '#'){
    cifras[indice] = tecla_int;
    indice++;
    lcd.setCursor(0, 1);         
    lcd.print(cifras[0]);
    lcd.setCursor(1, 1);         
    lcd.print(cifras[1]);  
    lcd.setCursor(3, 1);         
    lcd.print(cifras[2]);  
    lcd.setCursor(4, 1);         
    lcd.print(cifras[3]);  
  };

  // Inicio del proceso de cocción
  if (tecla == '*'){
    lcd.setCursor(0, 0);         
    lcd.print("Cocinando...    ");
    digitalWrite(motorPin, HIGH);   
    TIMSK1 |= (1 << TOIE1);  // Habilita la interrupción del timer 1
  };

  // Configuración de cancelación del proceso
  if (tecla == '#'){
    digitalWrite(motorPin, LOW);
    lcd.setCursor(0,0);
    lcd.print("Cancelado       ");
    lcd.setCursor(0,1);
    lcd.print(0);
    lcd.setCursor(1,1);
    lcd.print(0);
    lcd.setCursor(3,1);
    lcd.print(0);
    lcd.setCursor(4,1);
    lcd.print(0);   
  };
};


// Interrupción del timer 1
ISR(TIMER1_OVF_vect){

  cont++;

  // Tiempo de desborde: Ingresado por el usuario (calculado en segundos)
  overflow = cifras[3] + cifras[2]*10 + cifras[1]*60 + cifras[0]*600;

  if (cont < overflow*2){                 // Proceso de cocción y conteo regresivo del tiempo
    lcd.setCursor(0,1);
    lcd.print((overflow - cont/2) / 600);
    lcd.setCursor(1,1);
    lcd.print((overflow - cont/2) / 60);
    lcd.setCursor(3,1);
    lcd.print((overflow - cont/2) / 10);
    lcd.setCursor(4,1);
    lcd.print((overflow - cont/2) % 10);
  } else {                                // Fin del proceso de cocción
    lcd.setCursor(4,1);
    lcd.print(0);
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
    digitalWrite(motorPin, LOW);
    lcd.setCursor(0, 0); 
    lcd.print("Retire la comida");
  }
} 