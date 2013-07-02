#ifndef AVR_IO_H
#define AVR_IO_H

#include "../Avr_Core/interface/avr_hardware_interface.h"
#include <QMap>
#include <QApplication>
#include <iostream>
#include <cstdio>


#define REG_COUNT 9

class Avr_IO: public QObject, public Avr_Hardware_Interface
{
    Q_OBJECT
    Q_INTERFACES(Avr_Hardware_Interface)
    public:
        Avr_IO();

        virtual QString getPluginName();

        /**
        * @brief getRegisterCount Returns the count of registers that need to be setup
        * @return
        */
       virtual int getRegisterCount();
       /**
        * @brief getInterruptCount Returns the count of interrupt vectors that need to be set
        * @return
        */
       virtual int getInterruptCount() ;
       /**
        * @brief getRegisters Returns a list of Relevant Registers
        * @return
        */
       virtual QStringList getRegisters() ;
       /**
        * @brief getInterrupts Returns a list of relevant Interrupts
        * @return
        */
       virtual QStringList getInterrupts();
       //Bind Registers
        /**
        * @brief bindRegister Binds a pointer to a register
        * @param n The register to connect
        * @param ptr The pointer to connect
        */
       virtual void bindRegister(QString  reg, uint8_t *ptr);
        /**
        * @brief update Runs an update cycle for the hardware
        * @param cycles Number of cycles run by last instruction
        * @return An interrupt vector if any
        */
       virtual int update(int cycles);

        /**
         * @brief getInputs Returns a QMap of pointers to any required inputs
         * @return a QMap of pointers to inputs
         */
        virtual QMap <QString, uint8_t*> getInputs();

        /**
         * @brief getOutputs
         * @return a QMap of pointers to outputs
         */
        virtual QMap <QString, uint8_t>  getOutputs();

        /**
          * @brief Attaches a pointer to the full register set.
          */
        virtual void attachRegister(Avr_Registers *regPtr){}

        void updateOut();

    private:
        uint8_t* reg[9];
        QMap <QString,int> regMap;
        QMap <QString,uint8_t*> inputs;
        QMap <QString,uint8_t> outputs;
        uint8_t pinb[8], pinc[8],pind[8];
        uint8_t portb[9],portc[9],portd[9];
};

#endif // AVR_IO_H
