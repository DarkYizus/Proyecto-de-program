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

//  Tipos de Manos de Póker 
enum TipoManoPoker {
    CARTA_ALTA = 0,
    PAREJA,
    DOBLE_PAREJA,
    TRIO,
    ESCALERA,
    COLOR,
    FULL,
    POKER,
    ESCALERA_COLOR,
    ESCALERA_REAL
};

// Nombres legibles para los tipos de mano
const char* tipo_mano_str[] = {
    "Carta Alta",
    "Pareja",
    "Doble Pareja",
    "Trío",
    "Escalera",
    "Color",
    "Full",
    "Póker",
    "Escalera de Color",
    "Escalera Real"
};


// Estructura para representar una carta
typedef struct {
    int palo;
    int valor;
} Carta;

// Estructura para representar la información de un jugador (modificada)
typedef struct {
    int id;
    Carta mano[CARTAS_POR_JUGADOR];
    int cartasEnMano;
    int puntaje;
    bool haPasado; //  Indica si el jugador ha pasado en la baza actual
    TipoManoPoker tipoMano; // < Tipo de mano de póker del jugador
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
        // Asegurarse de que el índice sea válido para valores_str
        if (carta.valor >= 0 && carta.valor < NUM_VALORES_REGULARES) {
            std::cout << valores_str[carta.valor] << " de " << palos_str[carta.palo];
        } else {
            std::cout << "Carta inválida (Valor: " << carta.valor << ", Palo: " << carta.palo << ")";
        }
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

// se muestra la mano de los jugadores ordenado por metodo burbuja
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

// Funcion auxiliar para verificar si hay 'n' cartas del mismo valor
bool tieneN_elMismoValor(const Carta mano[], int numCartas, int n, int& valorEncontrado) {
    int conteoValores[NUM_VALORES_REGULARES] = {0};
    int jokersEnMano = 0;

    for (int i = 0; i < numCartas; ++i) {
        if (mano[i].palo == PALO_JOKER) {
            jokersEnMano++;
        } else {
            conteoValores[mano[i].valor]++;
        }
    }

    for (int i = NUM_VALORES_REGULARES - 1; i >= 0; --i) { // Recorrer de mayor a menor valor
        if (conteoValores[i] + jokersEnMano >= n) { // Si con los jokers se alcanza 'n'
            valorEncontrado = i;
            return true;
        }
    }
    return false;
}

// Función para evaluar la mano de póker
// Asume que la mano ya está ordenada por valor.
TipoManoPoker evaluarManoPoker(Carta mano[], int numCartasEnMano) {
    // Aquí se evalúan todas las cartas en mano del jugador (CARTAS_POR_JUGADOR).
    const int CARTAS_A_EVALUAR = 5; // Asumimos que se evalúan las 5 mejores cartas
    if (numCartasEnMano < CARTAS_A_EVALUAR) {
        return CARTA_ALTA; // O alguna otra indicación de mano incompleta
    }

    // Contadores de frecuencia para valores y palos
    int conteoValores[NUM_VALORES_REGULARES + 1] = {0}; // +1 para el VALOR_JOKER
    int conteoPalos[NUM_PALOS_REGULARES] = {0};
    int numJokersEnMano = 0;

    // Primer paso: Contar cartas y Jokers
    for (int i = 0; i < numCartasEnMano; ++i) {
        if (mano[i].palo == PALO_JOKER) {
            numJokersEnMano++;
        } else {
            conteoValores[mano[i].valor]++;
            conteoPalos[mano[i].palo]++;
        }
    }

    // --- Variables para detectar combinaciones ---
    int numParejas = 0;
    int valorPareja1 = -1; // Valor de la pareja más alta
    int valorPareja2 = -1; // Valor de la segunda pareja
    int numTrios = 0;
    int valorTrio = -1;
    int numPokers = 0;
    int valorPoker = -1;

    // Identificar pares, tríos, pókers (sin considerar Jokers todavía)
    for (int i = NUM_VALORES_REGULARES - 1; i >= 0; --i) { // Recorrer de mayor a menor valor
        if (conteoValores[i] == 4) {
            numPokers++;
            valorPoker = i;
        } else if (conteoValores[i] == 3) {
            numTrios++;
            valorTrio = i;
        } else if (conteoValores[i] == 2) {
            numParejas++;
            if (valorPareja1 == -1) valorPareja1 = i;
            else if (valorPareja2 == -1) valorPareja2 = i; // Solo necesitamos las dos más altas
        }
    }

    //  Detección de manos (de la más fuerte a la más débil) (Gracias Balatro)

    // 1. Escalera Real y Escalera de Color
    bool esColor = false;
    for (int p = 0; p < NUM_PALOS_REGULARES; ++p) {
        if (conteoPalos[p] + numJokersEnMano >= CARTAS_A_EVALUAR) {
            esColor = true;
            break;
        }
    }

    bool esEscalera = false;
    // Para Escalera, se necesita una lógica más robusta con Jokers.
    // Creamos un conjunto de valores únicos presentes en la mano (sin Jokers).
    int valoresUnicos[NUM_VALORES_REGULARES];
    int countUnicos = 0;
    for (int i = 0; i < NUM_VALORES_REGULARES; ++i) {
        if (conteoValores[i] > 0) {
            valoresUnicos[countUnicos++] = i;
        }
    }

    if (countUnicos > 0) {
        // Bubble Sort para los valores únicos
        for (int i = 0; i < countUnicos - 1; ++i) {
            for (int j = 0; j < countUnicos - i - 1; ++j) {
                if (valoresUnicos[j] > valoresUnicos[j+1]) {
                    int temp = valoresUnicos[j];
                    valoresUnicos[j] = valoresUnicos[j+1];
                    valoresUnicos[j+1] = temp;
                }
            }
        }

        // Ahora, verifica secuencias con Jokers
        // El As (VALOR_A = 11) puede ser bajo para 2-3-4-5-A
        // Por simplicidad, asumiremos secuencias "lineales" con A como alto.

        int maxSecuencia = 0;
        for (int i = 0; i < countUnicos; ++i) {
            int currentSecuencia = 1;
            int jokersDisponibles = numJokersEnMano;
            for (int k = i; k < countUnicos -1; ++k) {
                if (valoresUnicos[k+1] == valoresUnicos[k] + 1) {
                    currentSecuencia++;
                } else if (valoresUnicos[k+1] > valoresUnicos[k] + 1) {
                    int gap = valoresUnicos[k+1] - (valoresUnicos[k] + 1);
                    if (jokersDisponibles >= gap) {
                        jokersDisponibles -= gap;
                        currentSecuencia += (gap + 1);
                    } else {
                        break; // No suficientes jokers para cerrar el hueco
                    }
                } else { // Duplicado, no es secuencia
                    break;
                }
            }
            // Después del bucle, añade los jokers restantes al final de la secuencia potencial
            currentSecuencia += jokersDisponibles;
            if (currentSecuencia >= CARTAS_A_EVALUAR) {
                esEscalera = true;
                break;
            }
        }

        // Manejo especial para la escalera A-2-3-4-5 (si el A es el más bajo)
    }


    if (esEscalera && esColor) {
        // Verificar Escalera Real (10-J-Q-K-A del mismo palo)
        // Si el A es VALOR_A=11, K=10, Q=9, J=8, 10=7
        // Y si todas las cartas son del mismo palo (ya verificado por esColor)
        // Para que sea real, las 5 cartas de mayor valor (o completadas por joker) deben ser la secuencia 10-J-Q-K-A
        // Esto requiere una comprobación específica de los valores en la mano.
        // Para simplificar, si es escalera y color, y las cartas son A, K, Q, J, 10 (con o sin joker)
        bool tieneA = (conteoValores[VALOR_A] > 0 || numJokersEnMano > 0 && CARTAS_A_EVALUAR - (conteoValores[VALOR_10] + conteoValores[VALOR_J] + conteoValores[VALOR_Q] + conteoValores[VALOR_K]) <= numJokersEnMano); // Muy simplificado
        bool tieneK = (conteoValores[VALOR_K] > 0 || numJokersEnMano > 0 && CARTAS_A_EVALUAR - (conteoValores[VALOR_10] + conteoValores[VALOR_J] + conteoValores[VALOR_Q] + conteoValores[VALOR_A]) <= numJokersEnMano);
        bool tieneQ = (conteoValores[VALOR_Q] > 0 || numJokersEnMano > 0 && CARTAS_A_EVALUAR - (conteoValores[VALOR_10] + conteoValores[VALOR_J] + conteoValores[VALOR_K] + conteoValores[VALOR_A]) <= numJokersEnMano);
        bool tieneJ = (conteoValores[VALOR_J] > 0 || numJokersEnMano > 0 && CARTAS_A_EVALUAR - (conteoValores[VALOR_10] + conteoValores[VALOR_Q] + conteoValores[VALOR_K] + conteoValores[VALOR_A]) <= numJokersEnMano);
        bool tiene10 = (conteoValores[VALOR_10] > 0 || numJokersEnMano > 0 && CARTAS_A_EVALUAR - (conteoValores[VALOR_J] + conteoValores[VALOR_Q] + conteoValores[VALOR_K] + conteoValores[VALOR_A]) <= numJokersEnMano);

        if (tieneA && tieneK && tieneQ && tieneJ && tiene10) { // Lógica simplificada de si "puede" ser real con jokers.
            return ESCALERA_REAL;
        }
        return ESCALERA_COLOR;
    }

    // 2. Póker
    if (numPokers == 1 || (numTrios == 1 && numJokersEnMano >= 1) || (numParejas == 1 && numJokersEnMano >= 2)) {
        // Póker directo, o Trío + 1 Joker, o Pareja + 2 Jokers
        // Con 2 Jokers, también se podría formar un póker con 2 cartas del mismo valor.
        return POKER;
    }

    // 3. Full House (Trío y Pareja)
    if ((numTrios == 1 && numParejas >= 1) ||
        (numParejas >= 2 && numJokersEnMano >= 1) || // Dos parejas + 1 Joker = Full
        (numTrios == 0 && numParejas == 1 && numJokersEnMano >= 1 && (numTrios + numJokersEnMano >= 3))) { // Pareja + 1 Joker para trío y otra pareja
        return FULL;
    }

    // 4. Color (todas las cartas del mismo palo)
    if (esColor) {
        return COLOR;
    }

    // 5. Escalera (cinco cartas consecutivas de diferentes palos)
    if (esEscalera) {
        return ESCALERA;
    }

    // 6. Trío (tres cartas del mismo valor)
    if (numTrios == 1 || (numParejas == 1 && numJokersEnMano >= 1) || numJokersEnMano >= 2) {
        // Trío directo, o Pareja + 1 Joker, o 2 Jokers para formar un trío con cualquier carta
        return TRIO;
    }

    // 7. Doble Pareja (dos pares de cartas del mismo valor)
    // Asumimos que la mano ya está ordenada, así que los valores de pareja estarán juntos
    if (numParejas >= 2 || (numParejas == 1 && numJokersEnMano >= 1)) {
        // Doble pareja directa, o una pareja + 1 Joker para formar otra
        return DOBLE_PAREJA;
    }

    // 8. Pareja (dos cartas del mismo valor)
    if (numParejas == 1 || numJokersEnMano >= 1) { // Cualquier carta + un Joker
        return PAREJA;
    }

    // 9. Carta Alta (ninguna de las anteriores)
    return CARTA_ALTA;
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


