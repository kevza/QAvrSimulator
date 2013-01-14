#include "avr_timer_16b.h"

#include <QApplication>

Avr_Timer_16b::Avr_Timer_16b(){
    registers["TIFR0"] = 0;
    registers["TIFR1"] = 1;
    registers["TIMSK0"] = 2;
    registers["TIMSK1"] = 3;
    registers["TCCR1A"] = 4;
    registers["TTCR1B"] = 5;
    registers["TTCR1C"] = 6;
    registers["TCNT1L"] = 7;
    registers["TCNT1H"] = 8;
    registers["ICR1L"] = 9;
    registers["ICR1H"] = 10;
    registers["OCR1AL"] = 11;
    registers["OCR1AH"] = 12;
    registers["OCR1BL"] = 13;
    registers["OCR1BH"] = 14;
    registers["OCR1CL"] = 15;
    registers["OCR1CH"] = 16;


}

/**
 * @brief Avr_Timer_16b::getRegisterCount
 * @return The number of registers to set up
 */
int Avr_Timer_16b::getRegisterCount(){

    return 0;
}

/**
 * @brief Avr_Timer_16b::getRegisters
 * @return a QList of Register Names
 */
QStringList Avr_Timer_16b::getRegisters(){

    return QStringList() << "nothing to see here";
}

/**
 * @brief Avr_Timer_16b::getInterruptCount
 * @return The number of Interrupts
 */
int Avr_Timer_16b::getInterruptCount(){

    return 0;
}

/**
 * @brief Avr_Timer_16b::getInterrupts
 * @return A QList of interrupt names
 */
QStringList Avr_Timer_16b::getInterrupts(){

    return QStringList() << "nothing to see here";
}

/**
 * @brief Avr_Timer_16b::bindRegister Binds a memory location to a register
 * @param n The Number of the Register
 * @param ptr The memory location to point to
 */
void Avr_Timer_16b::bindRegister(QString reg, uint8_t *ptr){
    this->reg[registers[reg]] = ptr;
}

/**
 * @brief Avr_Timer_16b::update Runs an update cycle on the hard ware
 * @param cycles The number of cycles since the last update
 * @return Any relevant Interrupt Vector
 */
int Avr_Timer_16b::update(int cycles){
    return -1;
}

Q_EXPORT_PLUGIN2(avr_timer_16b ,Avr_Timer_16b)
