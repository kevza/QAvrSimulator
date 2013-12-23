#include "avr_ledmat.h"
#include <QDebug>

Avr_Ledmat::Avr_Ledmat(){
    //Registers
    regMap["DDRB"] = 0;
    regMap["PORTB"] = 1;
    regMap["DDRC"] = 2;
    regMap["PORTC"] = 3;

    regM[0] = 3;
    regM[1] = 1;
    regM[2] = 3;
    regM[3] = 3;
    regM[4] = 3;

    cols[0] = 6;cols[1] = 7; cols[2] = 4;
    cols[3] = 7;cols[4] = 5;

    grid[0]= &outputs["C0R0"];
    grid[1]= &outputs["C1R0"];
    grid[2] = &outputs["C2R0"];
    grid[3] = &outputs["C3R0"];
    grid[4]= &outputs["C4R0"];

    grid[5] = &outputs["C0R1"];
    grid[6]= &outputs["C1R1"];
    grid[7] = &outputs["C2R1"];
    grid[8]= &outputs["C3R1"];
    grid[9] = &outputs["C4R1"];

    grid[10] = &outputs["C0R2"];
    grid[11] = &outputs["C1R2"];
    grid[12]= &outputs["C2R2"];
    grid[13]= &outputs["C3R2"];
    grid[14]= &outputs["C4R2"];

    grid[15]= &outputs["C0R3"];
    grid[16]=&outputs["C1R3"];
    grid[17]= &outputs["C2R3"];
    grid[18]=&outputs["C3R3"];
    grid[19]= &outputs["C4R3"];

    grid[20]=&outputs["C0R4"];
    grid[21]= &outputs["C1R4"];
    grid[22]=&outputs["C2R4"];
    grid[23]= &outputs["C3R4"];
    grid[24]=&outputs["C4R4"];

    grid[25]= &outputs["C0R5"];
    grid[26]= &outputs["C1R5"];
    grid[27]= &outputs["C2R5"];
    grid[28]= &outputs["C3R5"];
    grid[29]= &outputs["C4R5"];

    grid[30]= &outputs["C0R6"] ;
    grid[31]= &outputs["C1R6"];
    grid[32]=&outputs["C2R6"];
    grid[33]= &outputs["C3R6"];
    grid[34]=&outputs["C4R6"] ;
    qDebug() << "Registers Attached!!!!";

    for (int i = 0 ; i < 35 ; i++){
        *this->grid[i] = 0;
    };
}

Avr_Ledmat::~Avr_Ledmat(){

}

QString Avr_Ledmat::getPluginName(){
    return "AVRLEDMAT";
}

/**
 * @brief Avr_Ledmat::getRegisterCount
 * @return The number of registers to set up
 */
int Avr_Ledmat::getRegisterCount(){
    return 4;
}

/**
 * @brief Avr_Ledmat::getRegisters
 * @return a QList of Register Names
 */
QStringList Avr_Ledmat::getRegisters(){
    return QStringList() <<"PORTB" << "DDRB"  << "PORTC" << "DDRC";
}

/**
 * @brief Avr_Ledmat::getInterruptCount
 * @return The number of Interrupts
 */
int Avr_Ledmat::getInterruptCount(){
    return 0;
}

/**
 * @brief Avr_Ledmat::getInterrupts
 * @return A QList of interrupt names
 */
QStringList Avr_Ledmat::getInterrupts(){
    //NO INTERUPPTS
    return QStringList();
}

/**
 * @brief Avr_Ledmat::bindRegister Binds a memory location to a register
 * @param n The Number of the Register
 * @param ptr The memory location to point to
 */
void Avr_Ledmat::bindRegister(QString reg, uint8_t *ptr){
    if (regMap.contains(reg)){
        this->reg[regMap[reg]] = ptr;
        qDebug() << "Connect " << reg  <<"\n";
    }else{
        std::cout << "Register Doesn't exists :" << reg.toStdString() << "\n";
        exit(99);
    }
}

/**
 * @brief Avr_Ledmat::update Runs an update cycle on the hard ware
 * @param cycles The number of cycles since the last update
 * @return Any relevant Interrupt Vector
 */
int Avr_Ledmat::update(int /*cycles*/){
    for (int i = 0 ; i < 7 ; i++){
        if ((*reg[2] & (1 << 6)) && !(*reg[3] & (1 << 6))){
            //C6 LOW
            if ((*reg[0] & (1 << i)) && !(*reg[1] & (1 << i))){
                //Bi LOW
                *grid[i * 5] = 1;

            }

        }
        if ((*reg[0] & (1 << 7)) && !(*reg[1] & (1 << 7))){
            //B7 LOW
            if ((*reg[0] & (1 << i)) && !(*reg[1] & (1 << i))){
                //Bi LOW
                 *grid[i * 5 + 1] = 1;

            }

        }
        if ((*reg[2] & (1 << 4)) && !(*reg[3] & (1 << 4))){
            //C4 LOW
            if ((*reg[0] & (1 << i)) && !(*reg[1] & (1 << i))){
                //Bi LOW
                 *grid[i * 5 + 2] = 1;

            }

        }
        if ((*reg[2] & (1 << 7)) && !(*reg[3] & (1 << 7))){
            //C7 LOW
            if ((*reg[0] & (1 << i)) && !(*reg[1] & (1 << i))){
                //Bi LOW
                 *grid[i * 5 + 3] = 1;

            }

        }
        if ((*reg[2] & (1 << 5)) && !(*reg[3] & (1 << 5))){
            //C6 LOW
            if ((*reg[0] & (1 << i)) && !(*reg[1] & (1 << i))){
                //Bi LOW

                 *grid[i * 5 + 4] = 1;
            }
        }


    }

    return -1;
}

QMap <QString, uint8_t*> Avr_Ledmat::getInputs(){
    return QMap<QString, uint8_t*>();
}

QMap <QString, uint8_t> Avr_Ledmat::getOutputs(){
    return outputs;
}


void Avr_Ledmat::updateOut(){
    /*
    outputs["C0R0"] = (grid[0]);
    outputs["C1R0"] = (grid[1]);
    outputs["C2R0"] = (grid[2]);
    outputs["C3R0"] = (grid[3]);
    outputs["C4R0"] = (grid[4]);

    outputs["C0R1"] = (grid[5]);
    outputs["C1R1"] = (grid[6] );
    outputs["C2R1"] = (grid[7] );
    outputs["C3R1"] = (grid[8] );
    outputs["C4R1"] = (grid[9] );

    outputs["C0R2"] = (grid[10] );
    outputs["C1R2"] = (grid[11] );
    outputs["C2R2"] = (grid[12] );
    outputs["C3R2"] = (grid[13] );
    outputs["C4R2"] = (grid[14]);

    outputs["C0R3"] = (grid[15] );
    outputs["C1R3"] = (grid[16] );
    outputs["C2R3"] = (grid[17] );
    outputs["C3R3"] = (grid[18] );
    outputs["C4R3"] = (grid[19] );

    outputs["C0R4"] = (grid[20] );
    outputs["C1R4"] = (grid[21] );
    outputs["C2R4"] = (grid[22] );
    outputs["C3R4"] = (grid[23] );
    outputs["C4R4"] = (grid[24] );

    outputs["C0R5"] = (grid[25] );
    outputs["C1R5"] = (grid[26]);
    outputs["C2R5"] = (grid[27] );
    outputs["C3R5"] = (grid[28] );
    outputs["C4R5"] = (grid[29] );

    outputs["C0R6"] = (grid[30] );
    outputs["C1R6"] = (grid[31] );
    outputs["C2R6"] = (grid[32] );
    outputs["C3R6"] = (grid[33]) ;
    outputs["C4R6"] = (grid[34]);
    */
}

void Avr_Ledmat::reset(){
    for (int i = 0 ; i < 35; i++){
        *grid[i] = 0;
    }
}

//Q_EXPORT_PLUGIN2(avr_ledmat ,Avr_Ledmat)
