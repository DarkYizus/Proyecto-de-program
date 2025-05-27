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
const int VALOR_2 = 12; // Se considera el 2 el valor más alto en tu definición
const int VALOR_JOKER = 13;

const char* palos_str[] = {"Corazones", "Diamantes", "Tréboles", "Picas","Joker"};
// Corrección de valores_str para que coincida con VALOR_X
const char* valores_str[] = {"3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A", "2"};

const int NUM_PALOS_REGULARES = 4;
const int NUM_VALORES_REGULARES = 13;
const int NUM_JOKERS = 2;
const int NUM_CARTAS = (NUM_PALOS_REGULARES * NUM_VALORES_REGULARES) + NUM_JOKERS;
const int NUM_JUGADORES = 4;
const int CARTAS_POR_JUGADOR = NUM_CARTAS / NUM_JUGADORES; // Cada jugador tendrá 13 cartas
const int NUM_RONDAS = 4; // definición de rondas

// Estructura para representar una carta
typedef struct {
    int palo;
    int valor;
} Carta;

//  Tipo de Jugada 
enum TipoJugada {
    NINGUNA = 0, // No es una jugada válida
    SINGLE,
    PAR,
    TRIO,
    POKER
};

// Nombres legibles para los tipos de jugada
const char* tipo_jugada_str[] = {
    "Ninguna",
    "Single",
    "Par",
    "Trío",
    "Póker"
};

// Estructura para almacenar la información de la jugada actual en la mesa
typedef struct {
    TipoJugada tipo;
    int valor; // Valor de la carta principal en la jugada
    int cantidadCartas; // Cuántas cartas se jugaron (para Single, Par, Trío, Poker)
    bool es8Stop; // Indica si la jugada fue un 8 Stop
    bool esJoker3Picas; // Indica si la jugada fue un Joker contra 3 de picas
    bool esRevolucion; // Indica si la jugada causó o mantuvo una revolución
} JugadaMesa;


// Estructura para representar la información de un jugador (modificada)
typedef struct {
    int id;
    Carta mano[CARTAS_POR_JUGADOR];
    int cartasEnMano;
    int puntaje;
    bool haPasado;
} Jugador;

// --- Estado del Juego ---
bool g_enRevolucion = false; // Variable global para el estado de la Revolución

// Función para crear la baraja
void crearBaraja(Carta* baraja) {
    Carta* cartaPtr = baraja;
    // Cartas regulares
    for (int p = 0; p < NUM_PALOS_REGULARES; ++p) {
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
        if (carta.valor >= 0 && carta.valor < NUM_VALORES_REGULARES && carta.palo >=0 && carta.palo < NUM_PALOS_REGULARES) {
            std::cout << valores_str[carta.valor] << " de " << palos_str[carta.palo];
        } else {
            std::cout << "Carta inválida (Valor: " << carta.valor << ", Palo: " << carta.palo << ")";
        }
    }
}

// Generadores de números aleatorios
static std::random_device rd;
static std::mt19937 generador(rd());

// Función para barajar las cartas
void barajar(Carta* baraja, int numCartas) {
    for (int i = numCartas - 1; i > 0; --i) {
        std::uniform_int_distribution<> dis(0, i);
        int j = dis(generador); // Obtiene un número aleatorio entre 0 e i

        Carta temp = baraja[i];
        baraja[i] = baraja[j];
        baraja[j] = temp;
    }
}

// Función para repartir cartas
void repartirCartas(Carta* baraja, Jugador* jugadores, int numJugadores, int cartasPorJugador) {
    int indiceCartaActual = 0;

    for (int i = 0; i < cartasPorJugador; ++i) {
        for (int j = 0; j < numJugadores; ++j) {
            if (indiceCartaActual < NUM_CARTAS) {
                jugadores[j].mano[jugadores[j].cartasEnMano] = baraja[indiceCartaActual];
                jugadores[j].cartasEnMano++;
                indiceCartaActual++;
            } else {
                std::cout << "¡Advertencia: No hay suficientes cartas para repartir a todos los jugadores!" << std::endl; //Simular escaces
                return;
            }
        }
    }
}

// Determina qué jugador tiene el 3 de diamantes para iniciar la primera ronda
int PrimerJugador(Jugador* jugadores_ptr) {
    for (int i = 0; i < NUM_JUGADORES; ++i) {
        for (int j = 0; j < jugadores_ptr[i].cartasEnMano; ++j) {
            if (jugadores_ptr[i].mano[j].palo == PALO_DIAMANTES && jugadores_ptr[i].mano[j].valor == VALOR_3) {
                return i; // Retorna el índice del jugador (0, 1, 2, 3)
            }
        }
    }
    return -1;
}

// Ordena la mano de un jugador por método burbuja
void ordenarcartasburbuja(Carta mano[], int numCartasEnMano) {
    for (int i = 0; i < numCartasEnMano - 1; ++i) {
        for (int j = 0; j < numCartasEnMano - i - 1; ++j) {
            // Criterio de ordenamiento: valor primero, luego palo (si valores iguales)
            if (mano[j].valor > mano[j + 1].valor ||
                (mano[j].valor == mano[j + 1].valor &&
                 mano[j].palo > mano[j + 1].palo)) {
                Carta temp = mano[j];
                mano[j] = mano[j + 1];
                mano[j + 1] = temp;
            }
        }
    }
}

//  Evaluar una jugada propuesta por el jugador
// Determina si un conjunto de cartas es una jugada válida (single, par, trío, póker)
// y su valor real considerando los Jokers.
TipoJugada obtenerTipoJugada(const Carta cartasJugadas[], int cantidadCartas, int& valorPrincipalJugada, bool& es8StopLocal, bool& esRevolucionLocal) {
    if (cantidadCartas <= 0 || cantidadCartas > 4) { // Las jugadas son de 1 a 4 cartas
        return NINGUNA;
    }

    int conteoValores[NUM_VALORES_REGULARES] = {0}; // Para cartas regulares
    int numJokersEnJugada = 0;
    valorPrincipalJugada = -1; // Inicializar

    // Contar Jokers y cartas regulares
    for (int i = 0; i < cantidadCartas; ++i) {
        if (cartasJugadas[i].palo == PALO_JOKER) {
            numJokersEnJugada++;
        } else {
            if (cartasJugadas[i].valor >= 0 && cartasJugadas[i].valor < NUM_VALORES_REGULARES) {
                conteoValores[cartasJugadas[i].valor]++;
                // Si es un 8, marcar para 8 Stop
                if (cartasJugadas[i].valor == VALOR_8) {
                    es8StopLocal = true;
                }
            } else {
                 // Error: carta con valor inválido en la jugada
                return NINGUNA;
            }
        }
    }

    // Identificar el valor principal de la jugada sin Jokers
    int valorMasFrecuente = -1;
    int maxFrecuencia = 0;
    // Iterar de mayor a menor valor para obtener el valor más alto en caso de empate de frecuencia
    for (int i = NUM_VALORES_REGULARES - 1; i >= 0; --i) {
        if (conteoValores[i] > maxFrecuencia) {
            maxFrecuencia = conteoValores[i];
            valorMasFrecuente = i;
        }
    }

    // Determinar el valor principal de la jugada con Jokers
    if (maxFrecuencia == 0 && numJokersEnJugada > 0) {
        // Caso especial: si solo hay Jokers en la jugada, o es un solo Joker.
        if (cantidadCartas == 1 && numJokersEnJugada == 1) {
            valorPrincipalJugada = VALOR_JOKER; // Jugada de Joker solo
            return SINGLE;
        }
        return NINGUNA; // Jugada inválida si solo hay múltiples jokers sin cartas regulares
    }

    // Verificar si la jugada es válida
    // La suma de cartas regulares del valor principal y Jokers debe ser igual a la cantidad de cartas jugadas
    if (maxFrecuencia + numJokersEnJugada == cantidadCartas) {
        valorPrincipalJugada = valorMasFrecuente; // El valor base de la jugada
        if (cantidadCartas == 1) return SINGLE;
        if (cantidadCartas == 2) return PAR;
        if (cantidadCartas == 3) return TRIO;
        if (cantidadCartas == 4) {
            esRevolucionLocal = true; // Un póker causa Revolución
            return POKER;
        }
    }
    return NINGUNA; // No es una jugada válida
}

// Función para determinar si una jugada es válida para responder a la jugada en mesa
bool esJugadaValida(const JugadaMesa& ultimaJugada, const Carta cartasJugadas[], int cantidadCartas, bool& es8StopLocal, bool& esJoker3PicasLocal, bool& esRevolucionLocal) {
    int valorPrincipalJugada; // Se actualizará dentro de obtenerTipoJugada
    TipoJugada tipoJugadaActual = obtenerTipoJugada(cartasJugadas, cantidadCartas, valorPrincipalJugada, es8StopLocal, esRevolucionLocal);

    if (tipoJugadaActual == NINGUNA) {
        return false; // No es una jugada válida (ni siquiera cumple con single, par, etc.)
    }

    // Regla especial: Reversión del 3 de picas
    if (ultimaJugada.tipo == SINGLE && ultimaJugada.valor == VALOR_JOKER &&
        tipoJugadaActual == SINGLE && cartasJugadas[0].palo == PALO_PICAS && cartasJugadas[0].valor == VALOR_3) {
        esJoker3PicasLocal = true;
        return true; // El 3 de picas le gana al Joker solo
    }

    // Si la mesa está vacía (primera jugada de la baza)
    if (ultimaJugada.tipo == NINGUNA) {
        return true; // Cualquier jugada válida es aceptada
    }

    // Si no es la primera jugada, debe coincidir la cantidad y ser de mayor valor (o menor en revolución)
    if (tipoJugadaActual == ultimaJugada.tipo) {
        // Considerar la Revolución: la jerarquía se invierte
        if (g_enRevolucion) {
            // En Revolución, menor valor gana a mayor valor (excepto Joker)
            // Joker siempre gana, incluso en revolución (si no es contra 3 de picas)
            if (valorPrincipalJugada == VALOR_JOKER && ultimaJugada.valor != VALOR_JOKER) {
                return true;
            }
            if (ultimaJugada.valor == VALOR_JOKER) return false; // No puedes ganar a un Joker en revolución (a menos que sea el 3 de picas)
            return valorPrincipalJugada < ultimaJugada.valor; // Menor valor gana
        } else {
            // Jerarquía normal: mayor valor gana
            if (valorPrincipalJugada == VALOR_JOKER) return true; // Joker siempre gana (si no es contra 3 de picas)
            if (ultimaJugada.valor == VALOR_JOKER) return false; // No puedes ganar a un Joker
            return valorPrincipalJugada > ultimaJugada.valor; // Mayor valor gana
        }
    }

    return false; // No es el mismo tipo de jugada (e.j., intentar jugar un par contra un trío)
}

// Función para eliminar cartas de la mano del jugador
void eliminarCartasDeMano(Jugador* jugador, const Carta cartasAeliminar[], int cantidad) {
    Carta tempMano[CARTAS_POR_JUGADOR]; // Una mano temporal para construir
    int tempManoIndex = 0;

    for (int i = 0; i < jugador->cartasEnMano; ++i) {
        bool esCartaAeliminar = false;
        for (int j = 0; j < cantidad; ++j) {
            // Compara palo y valor para identificar la carta a eliminar
            if (jugador->mano[i].palo == cartasAeliminar[j].palo &&
                jugador->mano[i].valor == cartasAeliminar[j].valor) {
                // Esta carta en la mano del jugador debe ser eliminada.
                // Marca como encontrada y evita añadirla a tempMano.
                esCartaAeliminar = true;
                break;
            }
        }
        if (!esCartaAeliminar) {
            tempMano[tempManoIndex++] = jugador->mano[i];
        }
    }

    // Copiar las cartas restantes de vuelta a la mano del jugador
    for (int i = 0; i < tempManoIndex; ++i) {
        jugador->mano[i] = tempMano[i];
    }
    jugador->cartasEnMano = tempManoIndex;
}

//  Limpieza de buffer de entrada
void limpiarBufferEntrada() {
    std::cin.clear();
    std::cin.ignore(9999, '\n');
}

void intercambiarCartas(Jugador* jugador1, Jugador* jugador2, int cantidad) {
    if (jugador1 == nullptr || jugador2 == nullptr || cantidad <= 0) {
        std::cout << "Error: Intentando intercambiar cartas con jugadores o cantidad inválida." << std::endl;
        return;
    }

    std::cout << "\nIntercambio de cartas entre Jugador " << jugador1->id << " y Jugador " << jugador2->id << " (" << cantidad << " cartas cada uno)." << std::endl;

    ordenarcartasburbuja(jugador1->mano, jugador1->cartasEnMano);
    ordenarcartasburbuja(jugador2->mano, jugador2->cartasEnMano);

    Carta cartasDeJugador1[CARTAS_POR_JUGADOR];
    Carta cartasDeJugador2[CARTAS_POR_JUGADOR];

    for (int i = 0; i < cantidad; ++i) {
        if (i < jugador1->cartasEnMano) {
            cartasDeJugador1[i] = jugador1->mano[i];
        } else {
            std::cout << "Advertencia: El Jugador " << jugador1->id << " no tiene suficientes cartas para dar." << std::endl;
            return;
        }
        if (i < jugador2->cartasEnMano) {
            cartasDeJugador2[i] = jugador2->mano[i];
        } else {
            std::cout << "Advertencia: El Jugador " << jugador2->id << " no tiene suficientes cartas para dar." << std::endl;
            return;
        }
    }

    std::cout << "Jugador " << jugador1->id << " da: ";
    for (int i = 0; i < cantidad; ++i) {
        imprimirCarta(cartasDeJugador1[i]);
        std::cout << (i == cantidad - 1 ? "" : ", ");
    }
    std::cout << std::endl;

    std::cout << "Jugador " << jugador2->id << " da: ";
    for (int i = 0; i < cantidad; ++i) {
        imprimirCarta(cartasDeJugador2[i]);
        std::cout << (i == cantidad - 1 ? "" : ", ");
    }
    std::cout << std::endl;

    eliminarCartasDeMano(jugador1, cartasDeJugador1, cantidad);
    eliminarCartasDeMano(jugador2, cartasDeJugador2, cantidad);

    for (int i = 0; i < cantidad; ++i) {
        jugador1->mano[jugador1->cartasEnMano++] = cartasDeJugador2[i];
        jugador2->mano[jugador2->cartasEnMano++] = cartasDeJugador1[i];
    }

    ordenarcartasburbuja(jugador1->mano, jugador1->cartasEnMano);
    ordenarcartasburbuja(jugador2->mano, jugador2->cartasEnMano);
}

// Función para simular una ronda de juego
void jugarRonda(Jugador* jugadores, int numJugadores, int cartasPorJugador, int primerJugadorIndex, int ordenSalida[]) {
    int pasesConsecutivos = 0;
    int jugadorActual = primerJugadorIndex;
    JugadaMesa ultimaJugadaMesa = {NINGUNA, -1, 0, false, false, false}; // Inicializar la jugada en mesa
    int ultimoJugadorQueJugoIndex = primerJugadorIndex; // Quién hizo la última jugada válida

    int jugadoresFuera = 0;
    bool jugadorEstaFuera[NUM_JUGADORES] = {false};
    // int ordenSalida[NUM_JUGADORES]; // Ahora se recibe como parámetro
    int indiceOrdenSalida = 0;

    std::cout << "\nBALATRO BALATREZ: El Jugador " << jugadores[primerJugadorIndex].id << " inicia la ronda." << std::endl;
    g_enRevolucion = false; // Resetear revolución al inicio de cada ronda.

    while (jugadoresFuera < (NUM_JUGADORES - 1)) { // La ronda termina cuando 3 jugadores se quedan sin cartas
        // Saltar a los jugadores que ya están fuera
        if (jugadorEstaFuera[jugadorActual]) {
            jugadorActual = (jugadorActual + 1) % numJugadores;
            continue;
        }

        std::cout << "\n--- Turno del Jugador " << jugadores[jugadorActual].id << " ---" << std::endl;
        std::cout << "Mano actual: ";
        ordenarcartasburbuja(jugadores[jugadorActual].mano, jugadores[jugadorActual].cartasEnMano); // Asegurar que la mano esté ordenada al mostrar
        for (int i = 0; i < jugadores[jugadorActual].cartasEnMano; ++i) {
            std::cout << "[" << i << "] "; // Mostrar índice para que el jugador elija
            imprimirCarta(jugadores[jugadorActual].mano[i]);
            std::cout << (i == jugadores[jugadorActual].cartasEnMano - 1 ? "" : ", ");
        }
        std::cout << " (" << jugadores[jugadorActual].cartasEnMano << " cartas)" << std::endl;

        std::cout << "Última jugada en mesa: ";
        if (ultimaJugadaMesa.tipo == NINGUNA) {
            std::cout << "Mesa vacía. Juega cualquier jugada válida." << std::endl;
        } else {
            std::cout << tipo_jugada_str[ultimaJugadaMesa.tipo] << " de ";
            if (ultimaJugadaMesa.valor == VALOR_JOKER) std::cout << "Joker";
            else std::cout << valores_str[ultimaJugadaMesa.valor];
            std::cout << " (" << ultimaJugadaMesa.cantidadCartas << " cartas)" << std::endl;
        }
        std::cout << "Estado de Revolución: " << (g_enRevolucion ? "ACTIVADA" : "DESACTIVADA") << std::endl;


        // --- INTERACCIÓN DEL JUGADOR ---
        Carta cartasJugadasElegidas[4]; // Almacena las cartas que el jugador elegirá
        int cantidadCartasElegidas = 0;
        bool jugadaExitosa = false;
        char opcion[5]; // Para leer 'p', 'j' o indices

        while (!jugadaExitosa) {
            std::cout << "\n¿Deseas Jugar cartas (j) o Pasar (p)? ";
            limpiarBufferEntrada(); // Limpiar antes de leer la opción
            std::cin >> opcion;

            if (opcion[0] == 'p' || opcion[0] == 'P') {
                std::cout << "El Jugador " << jugadores[jugadorActual].id << " pasa su turno." << std::endl;
                jugadores[jugadorActual].haPasado = true;
                pasesConsecutivos++;
                jugadaExitosa = true; // Se pasa, termina el turno
            } else if (opcion[0] == 'j' || opcion[0] == 'J') {
                int numCartasAProponer;
                std::cout << "Cuántas cartas quieres jugar (1-4)? ";
                std::cin >> numCartasAProponer;

                if (std::cin.fail() || numCartasAProponer < 1 || numCartasAProponer > 4 || numCartasAProponer > jugadores[jugadorActual].cartasEnMano) {
                    std::cout << "Cantidad de cartas inválida o no tienes suficientes cartas. Intenta de nuevo." << std::endl;
                    limpiarBufferEntrada(); // Limpiar después de una mala entrada numérica
                    continue; // Volver a pedir opción
                }

                cantidadCartasElegidas = numCartasAProponer;
                bool seleccionValida = true;
                std::cout << "Ingresa los ÍNDICES de las cartas que quieres jugar, separados por espacios: ";

                // Limpiar antes de leer los índices
                limpiarBufferEntrada(); // Esto es importante para el loop de lectura de índices

                for (int k = 0; k < cantidadCartasElegidas; ++k) {
                    int indiceCarta;
                    std::cin >> indiceCarta;
                    if (std::cin.fail() || indiceCarta < 0 || indiceCarta >= jugadores[jugadorActual].cartasEnMano) {
                        std::cout << "Índice de carta inválido: " << indiceCarta << ". Intenta de nuevo." << std::endl;
                        seleccionValida = false;
                        limpiarBufferEntrada(); // Limpiar después de una mala entrada numérica
                        break; // Salir del bucle de selección de cartas
                    }
                    // Copiar la carta seleccionada
                    cartasJugadasElegidas[k] = jugadores[jugadorActual].mano[indiceCarta];
                }

                if (!seleccionValida) {
                    // limpiarBufferEntrada() ya se llamó dentro del for si hubo error.
                    continue; // Volver a pedir opción
                }

                bool es8StopLocal = false;
                bool esJoker3PicasLocal = false;
                bool esRevolucionLocal = false;

                if (esJugadaValida(ultimaJugadaMesa, cartasJugadasElegidas, cantidadCartasElegidas, es8StopLocal, esJoker3PicasLocal, esRevolucionLocal)) {
                    std::cout << "¡Jugada ACEPTADA! El Jugador " << jugadores[jugadorActual].id << " juega: ";
                    for (int k = 0; k < cantidadCartasElegidas; ++k) {
                        imprimirCarta(cartasJugadasElegidas[k]);
                        std::cout << (k == cantidadCartasElegidas - 1 ? "" : ", ");
                    }
                    std::cout << std::endl;

                    eliminarCartasDeMano(&jugadores[jugadorActual], cartasJugadasElegidas, cantidadCartasElegidas);

                    // Actualizar la jugada en mesa con los valores correctos
                    ultimaJugadaMesa.tipo = obtenerTipoJugada(cartasJugadasElegidas, cantidadCartasElegidas, ultimaJugadaMesa.valor, ultimaJugadaMesa.es8Stop, ultimaJugadaMesa.esRevolucion);
                    ultimaJugadaMesa.cantidadCartas = cantidadCartasElegidas;
                    ultimaJugadaMesa.es8Stop = es8StopLocal; // Actualiza el flag
                    ultimaJugadaMesa.esJoker3Picas = esJoker3PicasLocal; // Actualiza el flag

                    // Aplicar reglas especiales de la jugada
                    if (ultimaJugadaMesa.es8Stop) {
                        std::cout << "¡8 Stop! La mesa se limpia. El Jugador " << jugadores[jugadorActual].id << " tiene el siguiente turno libre." << std::endl;
                        ultimaJugadaMesa = {NINGUNA, -1, 0, false, false, false}; // Limpiar mesa
                        pasesConsecutivos = 0; // Resetear pases
                        // No cambiar de jugador, el mismo jugador tiene otro turno
                    } else if (esRevolucionLocal) { // Solo si la jugada actual fue un Póker y activó la Revolución
                        g_enRevolucion = !g_enRevolucion; // Invertir el estado de la Revolución
                        std::cout << "¡Revolución! La jerarquía de cartas se ha " << (g_enRevolucion ? "invertido." : "restaurado.") << std::endl;
                    }

                    pasesConsecutivos = 0; // Se jugó una carta, resetear pases
                    jugadaExitosa = true; // Se jugó con éxito, termina el turno de interacción
                    ultimoJugadorQueJugoIndex = jugadorActual; // Este jugador hizo la última jugada válida

                } else {
                    std::cout << "Jugada inválida. Esa jugada no es permitida en este momento. Intenta de nuevo." << std::endl;
                }
            } else {
                std::cout << "Opción no reconocida. Por favor, ingresa 'j' para jugar o 'p' para pasar." << std::endl;
            }
        } // Fin del while (!jugadaExitosa)

        // Lógica de fin de baza (todos los jugadores activos han pasado)
        int jugadoresActivosQuePasaron = 0;
        for (int i = 0; i < numJugadores; ++i) {
            if (!jugadorEstaFuera[i] && jugadores[i].haPasado) {
                jugadoresActivosQuePasaron++;
            }
        }

        // Si todos los jugadores activos (excepto el que hizo la ultima jugada) han pasado
        // Y si la mesa no está ya vacía (ej. por un 8 Stop)
        // Se añade condición extra: que al menos un jugador haya jugado para que se considere una baza
        if (jugadoresActivosQuePasaron >= (numJugadores - jugadoresFuera) - 1 && ultimaJugadaMesa.tipo != NINGUNA) {
            std::cout << "¡Todos los jugadores activos han pasado! La mesa se limpia." << std::endl;
            ultimaJugadaMesa = {NINGUNA, -1, 0, false, false, false}; // Limpiar mesa
            pasesConsecutivos = 0; // Resetear pases
            jugadorActual = ultimoJugadorQueJugoIndex; // El jugador que hizo la última jugada tiene el siguiente turno
            std::cout << "El Jugador " << jugadores[jugadorActual].id << " tiene derecho a hacer cualquier jugada." << std::endl;
            // Reiniciar estado de "haPasado" para los jugadores que pasaron
            for (int i = 0; i < numJugadores; ++i) {
                jugadores[i].haPasado = false;
            }
        } else if (jugadaExitosa && !jugadores[jugadorActual].haPasado && !ultimaJugadaMesa.es8Stop) {
            // Si el jugador jugó (no pasó) y no fue un 8 Stop, avanzar al siguiente.
            jugadorActual = (jugadorActual + 1) % numJugadores;
        } else if (jugadores[jugadorActual].haPasado) {
            // Si el jugador pasó, avanzar al siguiente turno normalmente.
            jugadorActual = (jugadorActual + 1) % numJugadores;
        }
        // Si fue 8 Stop, el jugador actual sigue teniendo el turno, por lo que no se avanza.


        // Verificar si el jugador actual se quedó sin cartas (después de jugar)
        // chequeo para que no intente agregar al mismo jugador varias veces si ya está fuera
        if (jugadores[jugadorActual].cartasEnMano == 0 && !jugadorEstaFuera[jugadorActual]) {
            std::cout << "¡El Jugador " << jugadores[jugadorActual].id << " se ha quedado sin cartas!" << std::endl;
            jugadorEstaFuera[jugadorActual] = true;
            ordenSalida[indiceOrdenSalida++] = jugadores[jugadorActual].id; // Registrar orden de salida
            jugadoresFuera++;
            // El turno se pasará al siguiente jugador activo en la próxima iteración del bucle.
        }
    } // Fin del while (ronda)

    std::cout << "\n--- Ronda terminada ---" << std::endl;
    std::cout << "Orden de salida: ";
    for (int i = 0; i < indiceOrdenSalida; ++i) {
        std::cout << "Jugador " << ordenSalida[i] << (i == indiceOrdenSalida - 1 ? "" : ", ");
    }
    std::cout << std::endl;

    // Asignar puntos y rangos al final de la ronda
    // Los jugadores que no se quedaron sin cartas son los mendigos.
    int mendigoID = -1;
    for (int i = 0; i < numJugadores; ++i) {
        if (!jugadorEstaFuera[i]) { // El jugador que no salió es el mendigo
            mendigoID = jugadores[i].id;
            break;
        }
    }


    for (int i = 0; i < indiceOrdenSalida; ++i) {
        // Buscar el jugador por ID para asignar puntos
        int jugadorIndex = -1;
        for (int j = 0; j < numJugadores; ++j) {
            if (jugadores[j].id == ordenSalida[i]) {
                jugadorIndex = j;
                break;
            }
        }

        if (jugadorIndex != -1) {
            if (i == 0) { // Primer en salir: Magnate
                std::cout << "Jugador " << jugadores[jugadorIndex].id << ": Magnate (+30 puntos)" << std::endl;
                jugadores[jugadorIndex].puntaje += 30;
                // Almacenar este jugador como Magnate para la próxima ronda
                // (Necesitamos una variable global o un array para los rangos de la ronda anterior)
            } else if (i == 1) { // Segundo en salir: Rico
                std::cout << "Jugador " << jugadores[jugadorIndex].id << ": Rico (+20 puntos)";
                jugadores[jugadorIndex].puntaje += 20;
            } else if (i == 2) { // Tercero en salir: Pobre
                std::cout << "Jugador " << jugadores[jugadorIndex].id << ": Pobre (+10 puntos)" << std::endl;
                jugadores[jugadorIndex].puntaje += 10;
            }
        }
    }
    std::cout << "Jugador " << mendigoID << ": Mendigo (+0 puntos)" << std::endl;
}


// --- Función main principal ---
int main() {
    Carta baraja[NUM_CARTAS];
    Jugador jugadores[NUM_JUGADORES];
    int ordenSalidaRonda[NUM_JUGADORES];

    for (int i = 0; i < NUM_JUGADORES; ++i) {
        jugadores[i].id = i + 1;
        jugadores[i].cartasEnMano = 0;
        jugadores[i].puntaje = 0;
        jugadores[i].haPasado = false;
    }

    int magnateRondaAnteriorID = -1;
    int ricoRondaAnteriorID = -1;
    int pobreRondaAnteriorID = -1;
    int mendigoRondaAnteriorID = -1;

    for (int ronda = 0; ronda < NUM_RONDAS; ++ronda) {
        std::cout << "\n===== Ronda " << ronda + 1 << " =====" << std::endl;

        for (int i = 0; i < NUM_JUGADORES; ++i) {
            jugadores[i].cartasEnMano = 0;
            jugadores[i].haPasado = false;
        }

        crearBaraja(baraja);
        barajar(baraja, NUM_CARTAS);
        repartirCartas(baraja, jugadores, NUM_JUGADORES, CARTAS_POR_JUGADOR);

        if (ronda > 0) {
            std::cout << "\n--- Preparación de la ronda (Intercambio de Cartas) ---" << std::endl;

            Jugador* magnatePtr = nullptr;
            Jugador* ricoPtr = nullptr;
            Jugador* pobrePtr = nullptr;
            Jugador* mendigoPtr = nullptr;

            for (int i = 0; i < NUM_JUGADORES; ++i) {
                if (jugadores[i].id == magnateRondaAnteriorID) magnatePtr = &jugadores[i];
                if (jugadores[i].id == ricoRondaAnteriorID) ricoPtr = &jugadores[i];
                if (jugadores[i].id == pobreRondaAnteriorID) pobrePtr = &jugadores[i];
                if (jugadores[i].id == mendigoRondaAnteriorID) mendigoPtr = &jugadores[i];
            }

            if (magnatePtr && mendigoPtr) {
                intercambiarCartas(magnatePtr, mendigoPtr, 2);
            } else {
                std::cout << "Advertencia: No se pudo encontrar Magnate o Mendigo para el intercambio." << std::endl;
            }

            if (ricoPtr && pobrePtr) {
                intercambiarCartas(ricoPtr, pobrePtr, 1);
            } else {
                std::cout << "Advertencia: No se pudo encontrar Rico o Pobre para el intercambio." << std::endl;
            }
        }

        int primerJugadorIndex;
        if (ronda == 0) {
            primerJugadorIndex = PrimerJugador(jugadores);
            if (primerJugadorIndex == -1) {
                std::cout << "Error: 3 de diamantes no encontrado. Iniciando con Jugador 1." << std::endl;
                primerJugadorIndex = 0;
            }
        } else {
            int foundMendigoIndex = -1;
            for(int i = 0; i < NUM_JUGADORES; ++i) {
                if (jugadores[i].id == mendigoRondaAnteriorID) {
                    foundMendigoIndex = i;
                    break;
                }
            }
            if (foundMendigoIndex != -1) {
                primerJugadorIndex = foundMendigoIndex;
                std::cout << "El Mendigo de la ronda anterior (Jugador " << mendigoRondaAnteriorID << ") inicia la ronda." << std::endl;
            } else {
                primerJugadorIndex = 0;
                std::cout << "No se encontró el Mendigo de la ronda anterior. Iniciando con Jugador 1." << std::endl;
            }
        }

        for (int i = 0; i < NUM_JUGADORES; ++i) {
            std::cout << "\n--- Mano inicial del Jugador " << jugadores[i].id << " ("
                      << jugadores[i].cartasEnMano << " cartas) ---" << std::endl;
            ordenarcartasburbuja(jugadores[i].mano, jugadores[i].cartasEnMano);
            for (int j = 0; j < jugadores[i].cartasEnMano; ++j) {
                std::cout << "[" << j << "] ";
                imprimirCarta(jugadores[i].mano[j]);
                std::cout << std::endl;
            }
        }

        jugarRonda(jugadores, NUM_JUGADORES, CARTAS_POR_JUGADOR, primerJugadorIndex, ordenSalidaRonda);

        magnateRondaAnteriorID = ordenSalidaRonda[0];
        ricoRondaAnteriorID = ordenSalidaRonda[1];
        pobreRondaAnteriorID = ordenSalidaRonda[2];

        int mendigoDeEstaRonda = -1;
        for (int i = 0; i < NUM_JUGADORES; ++i) {
            bool encontradoEnOrdenSalida = false;
            for (int k = 0; k < (NUM_JUGADORES -1); ++k) {
                if (jugadores[i].id == ordenSalidaRonda[k]) {
                    encontradoEnOrdenSalida = true;
                    break;
                }
            }
            if (!encontradoEnOrdenSalida) {
                mendigoDeEstaRonda = jugadores[i].id;
                break;
            }
        }

        if (mendigoDeEstaRonda != -1) {
            mendigoRondaAnteriorID = mendigoDeEstaRonda;
        } else {
            std::cout << "ERROR: No se pudo determinar el mendigo de esta ronda." << std::endl;
            mendigoRondaAnteriorID = -1;
        }
    }

    std::cout << "\n===== ¡Fin del Juego! =====" << std::endl;
    int ganadorJuegoID = -1;
    int puntajeMax = -1;
    for (int i = 0; i < NUM_JUGADORES; ++i) {
        std::cout << "Jugador " << jugadores[i].id << " Puntaje Final: " << jugadores[i].puntaje << std::endl;
        if (jugadores[i].puntaje > puntajeMax) {
            puntajeMax = jugadores[i].puntaje;
            ganadorJuegoID = jugadores[i].id;
        } else if (jugadores[i].puntaje == puntajeMax) {
        }
    }
    if (ganadorJuegoID != -1) {
        std::cout << "El ganador final es el Jugador " << ganadorJuegoID << " con " << puntajeMax << " puntos." << std::endl;
    } else {
        std::cout << "No se pudo determinar un ganador final." << std::endl;
    }

    return 0;
}
