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
int encontrarPrimerJugador(Jugador* jugadores_ptr) { 
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
void ordenarCartas(Jugador* jugador) {
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

Para crear una función de rondas para tu juego de cartas en C++, considerando tus restricciones (solo las librerías dadas, punteros y sin usar std::array), necesitamos una estrategia para manejar el flujo del juego, las bazas y las acciones de los jugadores.
Aquí te presento una propuesta para la función jugarRonda y algunas consideraciones clave, ya que la lógica de un juego de cartas puede volverse compleja rápidamente.
Estructura para la Función de Rondas
Para una función de rondas, necesitamos considerar lo siguiente:
 * Bazas: Una ronda se compone de varias bazas. Cada baza implica que cada jugador ponga una carta.
 * Turnos: Los jugadores toman turnos para jugar sus cartas.
 * Carta ganadora de la baza: Después de que todos los jugadores han puesto una carta, se determina quién gana la baza.
 * Puntos: Se asignan puntos al ganador de la baza.
 * Fin de ronda: La ronda termina cuando no quedan cartas por jugar o se cumple una condición específica (ej. un número de bazas).
Considerando tus restricciones, te propongo una función jugarRonda que se encargará de gestionar el flujo de las bazas dentro de una ronda.
#include <iostream>
#include <random>

// Definiciones para las cartas de póker
const int PALO_CORAZONES = 0;
const int PALO_DIAMANTES = 1;
const int PALO_TREBOLES = 2;
const int PALO_PICAS = 3;
const int PALO_JOKER = 4; // palo para el joker

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
const int VALOR_JOKER = 13;

const char* palos_str[] = {"Corazones", "Diamantes", "Tréboles", "Picas", "Joker"};
const char* valores_str[] = {"3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A", "2", "JOKER"}; // Ajustado para incluir 2 y JOKER

//Contar cuantos palos tengo,valores para calcular el nuemro de cartas y definir rondas
const int NUM_PALOS_REGULARES = 4;
const int NUM_VALORES_REGULARES = 13; // Valores de 3 a As, y el 2
const int NUM_JOKERS = 2;
const int NUM_CARTAS = (NUM_PALOS_REGULARES * NUM_VALORES_REGULARES) + NUM_JOKERS; // 52 cartas de los palos + 2 Joker
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
    Carta mano[CARTAS_POR_JUGADOR]; // No podemos usar punteros directos para un array sin tamaño fijo aquí
    int cartasEnMano;
    int puntaje;
    bool haPasado; // Nuevo: Indica si el jugador ha pasado en la baza actual
} Jugador;


// Función para crear la baraja
void crearBaraja(Carta* baraja) {
    Carta* cartaPtr = baraja;
    // Cartas regulares
    for (int p = 0; p < NUM_PALOS_REGULARES; ++p) {
        // Ajuste para que los valores_str correspondan con VALOR_3 en adelante
        for (int v = 0; v < NUM_VALORES_REGULARES; ++v) {
            cartaPtr->palo = p;
            cartaPtr->valor = v;
            cartaPtr++;
        }
    }

    // Jokers
    for (int i = 0; i < NUM_JOKERS; ++i) {
        cartaPtr->palo = PALO_JOKER;
        cartaPtr->valor = VALOR_JOKER;
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
static std::random_device rd;
static std::mt19937 generador(rd());

void barajar(Carta* baraja, int numCartas) {
    for (int i = numCartas - 1; i > 0; --i) {
        std::uniform_int_distribution<> dis(0, i);
        int j = dis(generador);

        Carta temp = baraja[i];
        baraja[i] = baraja[j];
        baraja[j] = temp;
    }
}

// --- Función para repartir cartas ---
void repartirCartas(Carta* baraja, Jugador* jugadores, int numJugadores, int cartasPorJugador) {
    int indiceCartaActual = 0;

    for (int i = 0; i < cartasPorJugador; ++i) {
        for (int j = 0; j < numJugadores; ++j) {
            if (indiceCartaActual < NUM_CARTAS) {
                jugadores[j].mano[jugadores[j].cartasEnMano] = baraja[indiceCartaActual];
                jugadores[j].cartasEnMano++;
                indiceCartaActual++;
            } else {
                std::cout << "¡Advertencia: No hay suficientes cartas para repartir a todos los jugadores!" << std::endl;
                return;
            }
        }
    }
}

//el primero que empieza tiene q tener el 3 de diamantes
int encontrarPrimerJugador(Jugador* jugadores_ptr) {
    for (int i = 0; i < NUM_JUGADORES; ++i) {
        Jugador* actualJugador_ptr = jugadores_ptr + i; // Puntero al jugador actual

        for (int j = 0; j < actualJugador_ptr->cartasEnMano; ++j) {
            Carta* cartaEnMano_ptr = actualJugador_ptr->mano + j; // Puntero a la carta actual en la mano
            if (cartaEnMano_ptr->palo == PALO_DIAMANTES && cartaEnMano_ptr->valor == VALOR_3) {
                return i; // Retorna el índice del jugador
            }
        }
    }
    return -1; // No se encontró el jugador
}

// cuando se muestre la mano de los jugadores ordenado por metodo burbuja
void ordenarCartas(Jugador* jugador) {
    for (int i = 0; i < jugador->cartasEnMano - 1; ++i) {
        for (int j = 0; j < jugador->cartasEnMano - i - 1; ++j) {
            // Criterio de ordenación: primero por valor, luego por palo
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


