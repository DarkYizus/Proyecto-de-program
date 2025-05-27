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

    std::cout << "\nDEBUG: El Jugador " << jugadores[primerJugadorIndex].id << " inicia la ronda." << std::endl;
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
    int ordenSalidaRonda[NUM_JUGADORES]; // Para almacenar el orden de salida de cada ronda

    // Inicializar jugadores (puntajes, etc.)
    for (int i = 0; i < NUM_JUGADORES; ++i) {
        jugadores[i].id = i + 1;
        jugadores[i].cartasEnMano = 0;
        jugadores[i].puntaje = 0;
        jugadores[i].haPasado = false;
    }

    // Variables para almacenar los rangos de la ronda anterior (para intercambios y inicio de ronda)
    int magnateRondaAnteriorID = -1;
    int ricoRondaAnteriorID = -1;
    int pobreRondaAnteriorID = -1;
    int mendigoRondaAnteriorID = -1; // Quién empieza la siguiente ronda

    for (int ronda = 0; ronda < NUM_RONDAS; ++ronda) {
        std::cout << "\n===== Ronda " << ronda + 1 << " =====" << std::endl;

        // Resetear manos y estado de "pasado" de jugadores para la nueva ronda
        for (int i = 0; i < NUM_JUGADORES; ++i) {
            jugadores[i].cartasEnMano = 0;
            jugadores[i].haPasado = false; // Importante resetear esto
        }

        // Crear, barajar y repartir cartas
        crearBaraja(baraja);
        barajar(baraja, NUM_CARTAS);
        repartirCartas(baraja, jugadores, NUM_JUGADORES, CARTAS_POR_JUGADOR);

        // --- Preparación de la ronda (intercambio de cartas) ---
        if (ronda > 0) { // A partir de la segunda ronda
            std::cout << "\n--- Preparación de la ronda (Intercambio de Cartas) ---" << std::endl;
            // Aquí iría la lógica para buscar los jugadores por sus IDs de la ronda anterior
            // y realizar el intercambio de cartas.
            std::cout << "Lógica de intercambio de cartas pendiente de implementación." << std::endl;
            // Ejemplo:
            // Jugador* magnatePtr = nullptr; // Buscar jugador por ID
            // ...
            // magnatePtr->mano[idx1] = cartaDelMendigo;
        }

        // Determinar el primer jugador de la ronda
        int primerJugadorIndex;
        if (ronda == 0) { // Primera ronda: el que tiene el 3 de diamantes
            primerJugadorIndex = PrimerJugador(jugadores);
            if (primerJugadorIndex == -1) {
                std::cout << "Error: 3 de diamantes no encontrado. Iniciando con Jugador 1." << std::endl;
                primerJugadorIndex = 0; // Fallback
            }
        } else { // Rondas siguientes: el Mendigo de la ronda anterior
            // Necesitamos encontrar el índice del jugador que fue el mendigo de la ronda anterior
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
                // Esto debería pasar si no se pudo determinar el mendigo anterior (solo en la primera ronda)
                // o si hay un error
                primerJugadorIndex = 0;
                std::cout << "No se encontró el Mendigo de la ronda anterior. Iniciando con Jugador 1." << std::endl;
            }
        }

        // Mostrar manos iniciales de la ronda (después de repartir y posibles intercambios)
        for (int i = 0; i < NUM_JUGADORES; ++i) {
            std::cout << "\n--- Mano inicial del Jugador " << jugadores[i].id << " ("
                      << jugadores[i].cartasEnMano << " cartas) ---" << std::endl;
            ordenarcartasburbuja(jugadores[i].mano, jugadores[i].cartasEnMano); // Ordenar antes de mostrar
            for (int j = 0; j < jugadores[i].cartasEnMano; ++j) {
                std::cout << "[" << j << "] "; // Mostrar índice para que el jugador elija
                imprimirCarta(jugadores[i].mano[j]);
                std::cout << std::endl;
            }
        }

        // Jugar la ronda, pasando el array para el orden de salida
        jugarRonda(jugadores, NUM_JUGADORES, CARTAS_POR_JUGADOR, primerJugadorIndex, ordenSalidaRonda);

        // --- Actualizar los rangos para la próxima ronda ---
        // El mendigo es el único jugador que queda con cartas.
        int mendigoDeEstaRonda = -1;
        for (int i = 0; i < NUM_JUGADORES; ++i) {
            // Un jugador es el mendigo si no está en el ordenSalidaRonda
            bool encontradoEnOrdenSalida = false;
            for (int k = 0; k < (NUM_JUGADORES -1); ++k) { // ordenSalidaRonda solo tiene 3 jugadores
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
            // Esto no debería suceder si la lógica de salida es correcta
            std::cout << "ERROR: No se pudo determinar el mendigo de esta ronda." << std::endl;
            mendigoRondaAnteriorID = -1; // Fallback
        }
    }

    // Determinar el ganador final del juego
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
             
  






