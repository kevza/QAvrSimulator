#include "avr_timer_16b.h"

#include <QApplication>
#include <QDebug>

Avr_Timer_16b::Avr_Timer_16b(){
    registers["TIFR1"] = 0;
    registers["TIMSK1"] = 1;
    registers["TCCR1A"] = 2;
    registers["TCCR1B"] = 3;
    registers["TCCR1C"] = 4;
    registers["TCNT1L"] = 5;
    registers["TCNT1H"] = 6;
    registers["ICR1L"] = 7;
    registers["ICR1H"] = 8;
    registers["OCR1AL"] = 9;
    registers["OCR1AH"] = 10;
    registers["OCR1BL"] = 11;
    registers["OCR1BH"] = 12;
    registers["OCR1CL"] = 13;
    registers["OCR1CH"] = 14;
    this->prescaler = 0;
    this->d = 1;
}

QString Avr_Timer_16b::getPluginName(){
    return "TIMER16B";
}

/**
 * @brief Avr_Timer_16b::getRegisterCount
 * @return The number of registers to set up
 */
int Avr_Timer_16b::getRegisterCount(){

    return 15;
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
    qDebug() << "Bind Register : " << reg;
    this->reg[registers[reg]] = ptr;
}

/**
 * @brief Avr_Timer_16b::update Runs an update cycle on the hard ware
 * @param cycles The number of cycles since the last update
 * @return Any relevant Interrupt Vector
 */
int Avr_Timer_16b::update(int cycles){
    //Get count
    count = (*this->reg[6] << 8) | *this->reg[5];
    uint16_t oldCount = count;
    //Simulate the number of cycles to respond to

    switch ((*this->reg[3]) & 0x7){
        case 1:
            //PS=1
            count += cycles;

        break;
        case 2:
            //PS=8
            this->prescaler += cycles;
            this->prescaler += cycles;
            this->prescaler %= 8;
            if (this->prescaler < cycles){
                count += 1;
            }

        break;
        case 3:
            //PS=64
            this->prescaler += cycles;
            this->prescaler += cycles;
            this->prescaler %= 64;
            if (this->prescaler < cycles){
                count += 1;
            }

        break;
        case 4:
            //PS=256
            this->prescaler += cycles;
            this->prescaler %= 256;
            if (this->prescaler < cycles){
                count += 1;
            }

        break;
        case 5:
            //PS=1024
            this->prescaler += cycles;
            this->prescaler %= 1024;
            if (this->prescaler < cycles){
                count += 1;
            }

        break;

    }

    //Set overflow flag
    if (oldCount > count){
        *this->reg[0] |= 1;
    }

    //Set count
    *this->reg[5] = count & 0xff;
    *this->reg[6] = count >> 8;
    return -1;
}


/**
 * @brief getInputs Returns a QMap of pointers to any required inputs
 * @return a QMap of pointers to inputs
 */
QMap <QString, uint8_t*> Avr_Timer_16b::getInputs(){
    return QMap <QString, uint8_t *>();
}

/**
 * @brief getOutputs
 * @return a QMap of pointers to outputs
 */
QMap <QString, uint8_t> Avr_Timer_16b::getOutputs(){
    return QMap <QString, uint8_t >();
}

Q_EXPORT_PLUGIN2(avr_timer_16b ,Avr_Timer_16b)
