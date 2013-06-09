#include "avr_io.h"
#include <QDebug>
Avr_IO::Avr_IO(){
    //Registers
    regMap["PINB"] = 0;
    regMap["DDRB"] = 1;
    regMap["PORTB"] = 2;

    regMap["PINC"] = 3;
    regMap["DDRC"] = 4;
    regMap["PORTC"] = 5;

    regMap["PIND"] = 6;
    regMap["DDRD"] = 7;
    regMap["PORTD"] = 8;


    inputs["PINB0"] = &pinb[0];
    inputs["PINB1"] = &pinb[1];
    inputs["PINB2"] = &pinb[2];
    inputs["PINB3"] = &pinb[3];
    inputs["PINB4"] = &pinb[4];
    inputs["PINB5"] = &pinb[5];
    inputs["PINB6"] = &pinb[6];
    inputs["PINB7"] = &pinb[7];

    inputs["PINC0"] = &pinc[0];
    inputs["PINC1"] = &pinc[1];
    inputs["PINC2"] = &pinc[2];
    inputs["PINC3"] = &pinc[3];
    inputs["PINC4"] = &pinc[4];
    inputs["PINC5"] = &pinc[5];
    inputs["PINC6"] = &pinc[6];
    inputs["PINC7"] = &pinc[7];

    inputs["PIND0"] = &pind[0];
    inputs["PIND1"] = &pind[1];
    inputs["PIND2"] = &pind[2];
    inputs["PIND3"] = &pind[3];
    inputs["PIND4"] = &pind[4];
    inputs["PIND5"] = &pind[5];
    inputs["PIND6"] = &pind[6];
    inputs["PIND7"] = &pind[7];


    //Set all inputs to off position
    QList <uint8_t*> inpPtrs = inputs.values();
    for (int i = 0 ; i < inputs.size();i++){
        uint8_t *t = inpPtrs.at(i);
        *t = 255;
    }

    outputs["PORTB0"] = 0;
    outputs["PORTB1"] = 0;
    outputs["PORTB2"] = 0;
    outputs["PORTB3"] = 0;
    outputs["PORTB4"] = 0;
    outputs["PORTB5"] = 0;
    outputs["PORTB6"] = 0;
    outputs["PORTB7"] = 0;

    outputs["PORTC0"] = 0;
    outputs["PORTC1"] = 0;
    outputs["PORTC2"] = 0;
    outputs["PORTC3"] = 0;
    outputs["PORTC4"] = 0;
    outputs["PORTC5"] = 0;
    outputs["PORTC6"] = 0;
    outputs["PORTC7"] = 0;

    outputs["PORTD0"] = 0;
    outputs["PORTD1"] = 0;
    outputs["PORTD2"] = 0;
    outputs["PORTD3"] = 0;
    outputs["PORTD4"] = 0;
    outputs["PORTD5"] = 0;
    outputs["PORTD6"] = 0;
    outputs["PORTD7"] = 0;
}

QString Avr_IO::getPluginName(){
    return "AVRIO";
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
    static int count = 0;

    //UPDATE REGISTERS
    for (int i = 0; i < 8 ; i++){
        //Port B
        uint8_t shift = (1 << i);
        if (*reg[1] & shift){
            //UPDATE OUTPUT
            if (*reg[2] & shift){
                portb[i] += 1;
                portb[8] |= shift;
            }
        }else{
            //UPDATE INPUT
            if (pinb[i] == 1){
                *reg[0] |= shift;
            }else{
                *reg[0] &= ~shift;
            }
            //portb[8] &= ~shift;
        }

        //Port C
        if (*reg[4] & (1 << i)){
            //UPDATE OUTPUT
            if (*reg[5] & shift){
                portc[i] += 1;
                portc[8] |= shift;
            }

        }else{
            //UPDATE INPUT
            //UPDATE INPUT
            if (pinc[i] == 1){
                *reg[3] |= shift;
            }else{
                *reg[3] &= ~(shift);
            }
            //portc[8] &= ~shift;
        }
        //Port D
        if (*reg[7] & (1 << i)){
            //UPDATE OUTPUT
            //UPDATE OUTPUT
            if (*reg[8] & shift){
                portd[i] += 1;
                portd[8] |= shift;
            }
        }else{
            //UPDATE INPUT
            //UPDATE INPUT
            if (pind[i] == 1){
                *reg[6] |= shift;
            }else{
                *reg[6] &= ~shift;
            }
            //portd[8] &= ~shift;
        }

        /*
        outputs["PORTBD"] = portb[8];
        outputs["PORTCD"] = portc[8];
        outputs["PORTDD"] = portd[8];
        */
    }
    count += 1;
    if (count == 255){
        count = 0;
        updateOut();
    }

    return -1;
}

QMap <QString,uint8_t*> Avr_IO::getInputs(){
    return inputs;
}

QMap <QString, uint8_t> Avr_IO::getOutputs(){
    return outputs;
}


void Avr_IO::updateOut(){
    outputs["PORTB0"] = portb[0];
    outputs["PORTB1"] = portb[1];
    outputs["PORTB2"] = portb[2];
    outputs["PORTB3"] = portb[3];
    outputs["PORTB4"] = portb[4];
    outputs["PORTB5"] = portb[5];
    outputs["PORTB6"] = portb[6];
    outputs["PORTB7"] = portb[7];

    outputs["PORTC0"] = portc[0];
    outputs["PORTC1"] = portc[1];
    outputs["PORTC2"] = portc[2];
    outputs["PORTC3"] = portc[3];
    outputs["PORTC4"] = portc[4];
    outputs["PORTC5"] = portc[5];
    outputs["PORTC6"] = portc[6];
    outputs["PORTC7"] = portc[7];


    outputs["PORTD0"] = portd[0];
    outputs["PORTD1"] = portd[1];
    outputs["PORTD2"] = portd[2];
    outputs["PORTD3"] = portd[3];
    outputs["PORTD4"] = portd[4];
    outputs["PORTD5"] = portd[5];
    outputs["PORTD6"] = portd[6];
    outputs["PORTD7"] = portd[7];

    //Reset all ports for next round
    for (int i = 0 ; i < 8 ; i++){
        portb[i] = 0;
        portc[i] = 0;
        portd[i] = 0;
    }
}

Q_EXPORT_PLUGIN2(avr_io ,Avr_IO)
