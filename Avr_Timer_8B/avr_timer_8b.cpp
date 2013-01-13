#include "avr_timer_8b.h"

#include <QApplication>

Avr_Timer_8b::Avr_Timer_8b(){


}

/**
 * @brief Avr_Timer_8b::getRegisterCount
 * @return The number of registers to set up
 */
int Avr_Timer_8b::getRegisterCount(){

    return 0;
}

/**
 * @brief Avr_Timer_8b::getRegisters
 * @return a QList of Register Names
 */
QStringList Avr_Timer_8b::getRegisters(){

    return QStringList() << "nothing to see here";
}

/**
 * @brief Avr_Timer_8b::getInterruptCount
 * @return The number of Interrupts
 */
int Avr_Timer_8b::getInterruptCount(){

    return 0;
}

/**
 * @brief Avr_Timer_8b::getInterrupts
 * @return A QList of interrupt names
 */
QStringList Avr_Timer_8b::getInterrupts(){

    return QStringList() << "nothing to see here";
}

/**
 * @brief Avr_Timer_8b::bindRegister Binds a memory location to a register
 * @param n The Number of the Register
 * @param ptr The memory location to point to
 */
void Avr_Timer_8b::bindRegister(int n, uint8_t *ptr){


}

/**
 * @brief Avr_Timer_8b::update Runs an update cycle on the hard ware
 * @param cycles The number of cycles since the last update
 * @return Any relevant Interrupt Vector
 */
int Avr_Timer_8b::update(int cycles){
    return -1;
}

Q_EXPORT_PLUGIN2(avr_timer_8b ,Avr_Timer_8b)
