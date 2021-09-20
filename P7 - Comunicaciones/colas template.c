#define TAM_COLA 100
static char cola[TAM_COLA];
static int icabeza = 0; //índice para añadir
static int icola = 0; //índice para leer

__attribute__((vector(32), interrupt(IPL3SOFT),
        nomips16))
void InterrupcionUART1(void) {
    if (IFS1bits.U1RXIF == 1) { // Ha interrumpido el receptor
        if ((icabeza + 1 == icola) ||
                (icabeza + 1 == TAM_COLA && icola == 0)) {
            // La cola está llena
        } else {
            cola[icabeza] = U1RXREG; // Lee carácter de la UART
            icabeza++;
            if (icabeza == TAM_COLA) {
                icabeza = 0;
            }
            128 Comunicación serie asíncrona
        }
        IFS1bits.U1RXIF = 0; // Y para terminar se borra el flag
    }

    char getcUART(void) {
        char c;
        if (icola != icabeza) { // Hay datos nuevos
            c = cola[icola];
            icola++;
            if (icola == TAM_COLA) {
                icola = 0;
            }
        } else { // no ha llegado nada
            c = ’\0’;
        }
        return c;
    }






#define TAM_COLA 100
    static char cola_rx[TAM_COLA]; // cola de recepción
    static int icab_rx = 0; // índice para añadir
    static int icol_rx = 0; // índice para leer
    static char cola_tx[TAM_COLA]; // cola de transmisión
    static int icab_tx = 0; // índice para añadir
    static int icol_tx = 0; // índice para leer

    __attribute__((vector(32), interrupt(IPL3SOFT),
            nomips16))
            void InterrupcionUART1(void) {
        if (IFS1bits.U1RXIF == 1) { // Ha interrumpido el receptor
            if ((icab_rx + 1 == icol_rx) ||
                    (icab_rx + 1 == TAM_COLA && icol_rx == 0)) {
                // La cola está llena
            } else {
                cola_rx[icab_rx] = U1RXREG; // Lee carácter de la UART
                icab_rx++;
                if (icab_rx == TAM_COLA) {
                    icab_rx = 0;
                }
            }
            IFS1bits.U1RXIF = 0; // Y para terminar se borra el flag
        }
        if (IFS1bits.U1TXIF == 1) { // Ha interrumpido el transmisor
            // Se extrae un carácter de la cola y se envía
            if (icol_tx != icab_tx) { // Hay datos nuevos
                U1TXREG = cola_tx[icol_tx];
                icol_tx++;
                if (icol_tx == TAM_COLA) {
                    icol_tx = 0;
                }
            } else { // Se ha vaciado la cola.
                IEC1bits.U1TXIE = 0; // Para evitar bucle sin fin
            }
            IFS1bits.U1TXIF = 0; // Y para terminar se borra el flag
        }
    }

    void putsUART(char *ps) {
        while (*ps != ’\0’) {
            if ((icab_tx + 1 == icol_tx) ||
                    (icab_tx + 1 == TAM_COLA && icol_tx == 0)) {
                // La cola está llena. Se aborta el envío de
                // los caracteres que restan
                break;
            } else {
                cola_tx[icab_tx] = *ps; // Copia el carácter en la cola
                ps++; // Apunto al siguiente carácter de la cadena
                icab_tx++;
                if (icab_tx == TAM_COLA) {
                    icab_tx = 0;
                }
            }
            ps++;
        }
        // Se habilitan las interrupciones del transmisor para
        // comenzar a enviar
        IEC1bits.U1TXIE = 1;
    }
