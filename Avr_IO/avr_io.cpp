#include "avr_io.h"

Avr_IO::Avr_IO(){
    regMap["PINB"] = 0;
    regMap["DDRB"] = 1;
    regMap["PORTB"] = 2;

    regMap["PINC"] = 3;
    regMap["DDRC"] = 4;
    regMap["PORTC"] = 5;

    regMap["PIND"] = 6;
    regMap["DDRD"] = 7;
    regMap["PORTD"] = 8;


}

/**
 * @brief Avr_IO::getRegisterCount
 * @return The number of registers to set up
 */
int Avr_IO::getRegisterCount(){
    return 9;
}

/**
 * @brief Avr_IO::getRegisters
 * @return a QList of Register Names
 */
QStringList Avr_IO::getRegisters(){
    return QStringList() <<"PINB" << "DDRB"  << "PORTB"
                         << "PINC" << "DDRC" << "PORTC"
                         << "PIND"<< "DDRD"  << "PORTD";
}

/**
 * @brief Avr_IO::getInterruptCount
 * @return The number of Interrupts
 */
int Avr_IO::getInterruptCount(){
    return 0;
}

/**
 * @brief Avr_IO::getInterrupts
 * @return A QList of interrupt names
 */
QStringList Avr_IO::getInterrupts(){

    return QStringList();
}

/**
 * @brief Avr_IO::bindRegister Binds a memory location to a register
 * @param n The Number of the Register
 * @param ptr The memory location to point to
 */
void Avr_IO::bindRegister(QString reg, uint8_t *ptr){
    if (regMap.contains(reg)){
        this->reg[regMap[reg]] = ptr;
    }else{
        std::cout << "Register Doesn't exists :" << reg.toStdString() << "\n";
        exit(99);
    }
}

/**
 * @brief Avr_IO::update Runs an update cycle on the hard ware
 * @param cycles The number of cycles since the last update
 * @return Any relevant Interrupt Vector
 */
int Avr_IO::update(int cycles){
    static int lock = 0;
    if (*reg[4] == 0)
        lock = 0;

    if (*reg[4] > 0 && !lock){

        fflush(stdout);
        for (int i = 0 ; i < 8 ; i++){
            if (*reg[5] & (1 << i)){
                std::cout << "* ";
            }else{
                std::cout << "  ";
            }
        }
        std::cout << "\n";
        lock = 1;
    }

    return -1;
}

Q_EXPORT_PLUGIN2(avr_io ,Avr_IO)
