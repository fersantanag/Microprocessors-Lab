void I2C1Inicializa(void) {
    //400 kb/s la que usaremos
    I2C1BRG = 4;
    I2C1CON = 0x8000; // I2C ON, con control de slew rate
    IFS1bits.I2C1MIF = 0; // Borro el flag por si acaso
}

void I2C1GeneraStart(void) {
    I2C1CONbits.SEN = 1; // Genera condición de start en el bus
    while (IFS1bits.I2C1MIF == 0); // Espera el final de la condición
    IFS1bits.I2C1MIF = 0; // Borro el flag
}

void I2C1GeneraReStart(void) {
    I2C1CONbits.RSEN = 1; // Genera condición de repeteaded
    // start en el bus
    while (IFS1bits.I2C1MIF == 0); // Espera el final de la condición
    IFS1bits.I2C1MIF = 0; // Borro el flag
}

void I2C1GeneraStop(void) {
    I2C1CONbits.PEN = 1; // Genera condición de stop en el bus
    while (IFS1bits.I2C1MIF == 0); // Espera el final de la condición
    IFS1bits.I2C1MIF = 0; // Borro el flag
}

uint8_t I2C1LeeByte(int ack) {
    // Lectura
    uint8_t dato;
    I2C1CONbits.RCEN = 1; // Se activa la recepción del dato (reloj)
    while (IFS1bits.I2C1MIF == 0); // Espera el final de la recepción
    IFS1bits.I2C1MIF = 0; // Borro el flag
    dato = I2C1RCV;
    // Envio ACK
    I2C1CONbits.ACKDT = ack & 1; // Bit 5 del I2CxCON
    I2C1CONbits.ACKEN = 1; // Se envía el ACK
    while (IFS1bits.I2C1MIF == 0); // Espera el envío del ACK
    IFS1bits.I2C1MIF = 0; // Borro el flag
    return dato;
}

int I2C1EscribeByte(uint8_t dato) {
    I2C1TRN = dato;
    while (IFS1bits.I2C1MIF == 0); // Espera el final del envío
    IFS1bits.I2C1MIF = 0; // Borro el flag
    return I2C1STATbits.ACKSTAT; //Devuelve el ACK
    //(bit 15 del I2C1STAT)
}

void i2c_writeReg(uint8_t add, uint8_t reg, uint8_t val) {
    I2C1GeneraStart(); // I2C write direction
    
    if (I2C1EscribeByte(add << 1) != 0) { //NACK
        I2C1GeneraStop(); // Abortamos
        return;
    }
    if (I2C1EscribeByte(reg) != 0) { //NACK
        I2C1GeneraStop(); // Abortamos
        return;
    }
    if (I2C1EscribeByte(val) != 0) { //NACK
        I2C1GeneraStop(); // Abortamos
        return;
    }



}

void i2c_read_reg_to_buf(uint8_t add, uint8_t reg, uint8_t *buf, uint8_t size) {
    I2C1GeneraStart(); // I2C write direction

    if (I2C1EscribeByte(add << 1) != 0) { //NACK
        I2C1GeneraStop(); // Abortamos
        return;
    }
    if (I2C1EscribeByte(reg) != 0) { //NACK
        I2C1GeneraStop(); // Abortamos
        return;
    }
    I2C1GeneraReStart();
    
    if (I2C1EscribeByte(add <<1|1) != 0) { //NACK
        I2C1GeneraStop(); // Abortamos
        return;
    }
    
    int tmp=0;
    for(int i=0; i<size; i++ )
    {
        if(i==size-1)
            tmp=1;
            
        buf[i]=I2C1LeeByte(tmp);
    }
    
}

uint8_t i2c_readReg(uint8_t add, uint8_t reg) {
    uint8_t val;
    i2c_read_reg_to_buf(add, reg, &val, 1);
    return val;
}

