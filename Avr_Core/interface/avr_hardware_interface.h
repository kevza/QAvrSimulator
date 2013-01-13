#ifndef AVR_HARDWARE_INTERFACE_H
#define AVR_HARDWARE_INTERFACE_H
#include <QStringList>
#include <QtPlugin>
#include <stdint.h>

class Avr_Hardware_Interface
{
    public:
        virtual ~Avr_Hardware_Interface(){}
        //Get Counts and Lists of regs/ints
         /**
         * @brief getRegisterCount Returns the count of registers that need to be setup
         * @return
         */
        virtual int getRegisterCount() = 0;
        /**
         * @brief getInterruptCount Returns the count of interrupt vectors that need to be set
         * @return
         */
        virtual int getInterruptCount() = 0;
        /**
         * @brief getRegisters Returns a list of Relevant Registers
         * @return
         */
        virtual QStringList getRegisters() = 0;
        /**
         * @brief getInterrupts Returns a list of relevant Interrupts
         * @return
         */
        virtual QStringList getInterrupts() = 0;
        //Bind Registers
         /**
         * @brief bindRegister Binds a pointer to a register
         * @param n The register to connect
         * @param ptr The pointer to connect
         */
        virtual void bindRegister(QString reg, uint8_t *ptr) = 0;
         /**
         * @brief update Runs an update cycle for the hardware
         * @param cycles Number of cycles run by last instruction
         * @return An interrupt vector if any
         */
        virtual int update(int cycles) = 0;
};

Q_DECLARE_INTERFACE(Avr_Hardware_Interface, "com.kevin_luke.Avr_Core_Builder.HardwareUnit/1.0")


#endif // AVR_HARDWARE_INTERFACE_H


