#include <iostream>
#include <random>

// Definiciones para las cartas de póker
const int PALO_CORAZONES = 0;
const int PALO_DIAMANTES = 1;
const int PALO_TREBOLES = 2;
const int PALO_PICAS = 3;
conts int PALO_JOKER = 4;// palo para el joker

const int VALOR_2 = 12;
const int VALOR_3 = 1;
const int VALOR_4 = 2;
const int VALOR_5 = 3;
const int VALOR_6 = 4;
const int VALOR_7 = 5;
const int VALOR_8 = 6;
const int VALOR_9 = 7;
const int VALOR_10 = 8;
const int VALOR_J = 9;
const int VALOR_Q = 10;
const int VALOR_K = 11;
const int VALOR_A = 13;
conts int VALOR_JOKER=14;

const char* palos_str[] = {"Corazones", "Diamantes", "Tréboles", "Picas","Joker"};
conts char* valores_joker[]={"JOKER"};
const char* valores_str[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A",};
//Contar cuantos palos tengo,valores para calcular el nuemro de cartas y definir rondas
const int NUM_PALOS = 5;
const int NUM_VALORES = 14;
const int NUM_CARTAS = 54;// 52 cartas de los palos + 2 Joker
const int NUM_JUGADORES = 4;
const int CARTAS_POR_JUGADOR = NUM_CARTAS / NUM_JUGADORES;
const int NUM_RONDAS = 3;

// Estructura para representar una carta
typedef struct {
    int palo;
    int valor;
} Carta;

// Estructura para representar la información de un jugador
typedef struct {
    int id;
    Carta mano[CARTAS_POR_JUGADOR];
    int cartasEnMano;
    int puntaje;
    bool haPasado; // Nuevo: Indica si el jugador ha pasado en la baza actual
} Jugador;

