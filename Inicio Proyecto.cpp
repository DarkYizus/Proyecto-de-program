#include <iostream>
#include <random>

// Definiciones para las cartas de póker
const int PALO_CORAZONES = 0;
const int PALO_DIAMANTES = 1;
const int PALO_TREBOLES = 2;
const int PALO_PICAS = 3;
const int PALO_JOKER = 4;// palo para el joker

const int VALOR_3 = 0;
const int VALOR_4 = 1;
const int VALOR_5 = 2;
const int VALOR_6 = 3;
const int VALOR_7 = 4;
const int VALOR_8 = 5;
const int VALOR_9 = 6;
const int VALOR_10 = 7;
const int VALOR_J = 8;
const int VALOR_Q = 9;
const int VALOR_K = 10;
const int VALOR_A = 11;
const int VALOR_2 = 12;
const int VALOR_JOKER=13;

const char* palos_str[] = {"Corazones", "Diamantes", "Tréboles", "Picas","Joker"};
const char* valores_joker[]={"JOKER"};
const char* valores_str[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A",};
//Contar cuantos palos tengo,valores para calcular el nuemro de cartas y definir rondas
const int NUM_PALOS_REGULARES = 4;
const int NUM_VALORES_REGULARES = 13;
const int NUM_JOKERS=2;
const int NUM_CARTAS = (NUM_PALOS_REGULARES*NUM_VALORES_REGULARES)+NUM_JOKERS;// 52 cartas de los palos + 2 Joker
const int NUM_JUGADORES = 4;
const int CARTAS_POR_JUGADOR = NUM_CARTAS / NUM_JUGADORES;
const int NUM_RONDAS = 4;

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


// Función para crear la baraja
void crearBaraja(Carta* baraja) {
    Carta* cartaPtr = baraja;
    // Cartas regulares
    for (int p = 0; p < NUM_PALOS_REGULARES; ++p) {
        for (int v = 0; v < NUM_VALORES_REGULARES; ++v) { // Corregido: v < NUM_VALORES_REGULARES
            cartaPtr->palo = p;
            cartaPtr->valor = v;
            cartaPtr++;
        }
    }
    
// Jokers
for (int i=0;i<NUM_JOKERS;++i){
    cartaPtr->palo=PALO_JOKER;
    cartaPtr->valor=VALOR_JOKER;
    cartaPtr++;
}
}

// Función para imprimir una carta
void imprimirCarta(const Carta& carta) {
    if (carta.palo == PALO_JOKER) {
        std::cout << "JOKER";
    } else {
        std::cout << valores_str[carta.valor] << " de " << palos_str[carta.palo];
    }
}

// --- Función para barajar las cartas (con <random>) ---
// Se declara el generador y la distribución como 'static'
// para que se inicialicen una sola vez y mantengan su estado.
// sino podriamos pasarlos como parámetros.
static std::random_device rd; // Fuente  para la semilla
static std::mt19937 generador(rd()); // Generador de números aleatorios de Mersenne Twister

void barajar(Carta* baraja, int numCartas) {
    // Usamos el generador y la distribución para obtener un índice aleatorio
    for (int i = numCartas - 1; i > 0; --i) {
        // La distribución se creará en cada iteración
        std::uniform_int_distribution<> dis(0, i);
        int j = dis(generador); // Obtiene un número aleatorio entre 0 e i

        // Intercambiar baraja[i] con baraja[j]
        Carta temp = baraja[i];
        baraja[i] = baraja[j];
        baraja[j] = temp;
    }
}

// --- Función para repartir cartas ---
void repartirCartas(Carta* baraja, Jugador* jugadores, int numJugadores, int cartasPorJugador) {
    int indiceCartaActual = 0; // Índice para saber qué carta de la baraja vamos a repartir

    // Iterar para repartir cada ronda de cartas
    for (int i = 0; i < cartasPorJugador; ++i) {
        // Iterar por cada jugador para darle una carta
        for (int j = 0; j < numJugadores; ++j) {
            if (indiceCartaActual < NUM_CARTAS) { // Asegurarse de que quedan cartas en la baraja
                // Añadir la carta a la mano del jugador
                jugadores[j].mano[jugadores[j].cartasEnMano] = baraja[indiceCartaActual];
                jugadores[j].cartasEnMano++; // Incrementar el contador de cartas en la mano
                indiceCartaActual++; // Moverse a la siguiente carta de la baraja
            } else {
                std::cout << "¡Advertencia: No hay suficientes cartas para repartir a todos los jugadores!" << std::endl;
                return; // Salir si no quedan cartas, solo por llenar el else, estamos en Venezuela, hay que simular escaces
            }
        }
    }
}

//el primero que empieza tiene q tener el 3 de diamantes 
int PrimerJugador(Jugador* jugadores_ptr) { 
    Jugador* actualJugador_ptr = jugadores_ptr; 
    for (int i = 0; i < NUM_JUGADORES; ++i) {
        Carta* cartaEnMano_ptr = actualJugador_ptr->mano; 

        for (int j = 0; j < actualJugador_ptr->cartasEnMano; ++j) {
            if (cartaEnMano_ptr->palo == PALO_DIAMANTES && cartaEnMano_ptr->valor == VALOR_3) {
                return (int)(actualJugador_ptr - jugadores_ptr); 
            }
            cartaEnMano_ptr++; 
        }
        actualJugador_ptr++; 
    return -1; 
}
}

// cuando se muestre la mano de los jugadores ordenado por metodo burbuja
void ordenarcartasburbuja(Jugador* jugador) {
    for (int i = 0; i < jugador->cartasEnMano - 1; ++i) {
        for (int j = 0; j < jugador->cartasEnMano - i - 1; ++j) {
            if (jugador->mano[j].valor > jugador->mano[j + 1].valor ||
                (jugador->mano[j].valor == jugador->mano[j + 1].valor && 
                 jugador->mano[j].palo > jugador->mano[j + 1].palo)) {
                Carta temp = jugador->mano[j];
                jugador->mano[j] = jugador->mano[j + 1];
                jugador->mano[j + 1] = temp;
            }
        }
    }
}

bool cartasigaules(const Carta* cartas, int cantidad) {
    if (cantidad <= 0) return false;

    for (int i = 1; i < cantidad; ++i) {
        if (cartas[i].valor != cartas[0].valor) {
            return false;
        }
    }
    return true;
}
             
  




// Iniciar el juego por rondas
    for (int ronda = 0; ronda < NUM_RONDAS; ++ronda) {
        std::cout << "\n===== Ronda " << ronda + 1 << " =====" << std::endl;
        
        
        jugarRonda(jugadores, NUM_JUGADORES, CARTAS_POR_JUGADOR, primerJugadorGeneral);
        
    }

    // Aquí podrías determinar el ganador final del juego
    std::cout << "\n===== ¡Fin del Juego! =====" << std::endl;
    int ganadorJuego = -1;
    int puntajeMax = -1;
    for (int i = 0; i < NUM_JUGADORES; ++i) {
        if (jugadores[i].puntaje > puntajeMax) {
            puntajeMax = jugadores[i].puntaje;
            ganadorJuego = jugadores[i].id;
        }
    }
    std::cout << "El ganador final es el Jugador " << ganadorJuego << " con " << puntajeMax << " puntos." << std::endl;

    return 0;
}


