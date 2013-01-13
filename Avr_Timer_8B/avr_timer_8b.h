#ifndef Avr_Timer_8b_H
#define Avr_Timer_8b_H

#include "interface/avr_hardware_interface.h"


class Avr_Timer_8b: public QObject, public Avr_Hardware_Interface
{
    Q_OBJECT
    Q_INTERFACES(Avr_Hardware_Interface)
    public:
        Avr_Timer_8b();

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
       virtual void bindRegister(int n, uint8_t *ptr);
        /**
        * @brief update Runs an update cycle for the hardware
        * @param cycles Number of cycles run by last instruction
        * @return An interrupt vector if any
        */
       virtual int update(int cycles);
};

#endif // Avr_Timer_8b_H
