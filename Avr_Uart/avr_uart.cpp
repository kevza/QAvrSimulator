#include "avr_uart.h"

#include <QApplication>

Avr_Uart::Avr_Uart(){


}

/**
 * @brief Avr_Uart::getRegisterCount
 * @return The number of registers to set up
 */
int Avr_Uart::getRegisterCount(){

    return 0;
}

/**
 * @brief Avr_Uart::getRegisters
 * @return a QList of Register Names
 */
QStringList Avr_Uart::getRegisters(){

    return QStringList() << "nothing to see here";
}

/**
 * @brief Avr_Uart::getInterruptCount
 * @return The number of Interrupts
 */
int Avr_Uart::getInterruptCount(){

    return 0;
}

/**
 * @brief Avr_Uart::getInterrupts
 * @return A QList of interrupt names
 */
QStringList Avr_Uart::getInterrupts(){

    return QStringList() << "nothing to see here";
}

/**
 * @brief Avr_Uart::bindRegister Binds a memory location to a register
 * @param n The Number of the Register
 * @param ptr The memory location to point to
 */
void Avr_Uart::bindRegister(int n, uint8_t *ptr){


}

/**
 * @brief Avr_Uart::update Runs an update cycle on the hard ware
 * @param cycles The number of cycles since the last update
 * @return Any relevant Interrupt Vector
 */
int Avr_Uart::update(int cycles){
    return -1;
}

Q_EXPORT_PLUGIN2(avr_uart ,Avr_Uart)
