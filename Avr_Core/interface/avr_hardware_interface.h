#ifndef AVR_HARDWARE_INTERFACE_H
#define AVR_HARDWARE_INTERFACE_H
#include <QStringList>
#include <QMap>
#include <QtPlugin>
#include <stdint.h>
#include "../core/avr_registers.h"

class Avr_Hardware_Interface
{
    public:
        virtual ~Avr_Hardware_Interface(){}


        virtual QString getPluginName() = 0;
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

         /**
         * @brief bindRegister Binds a pointer to a register
         * @param n The register to connect
         * @param ptr The pointer to connect
         */
        virtual void bindRegister(QString reg, uint8_t *ptr) = 0;

        /**
        * @brief bindRegister Binds a register pointer location to a
        * register
        * @param reg The register to connect
        * @para loc The location of the register
        *
        virtual void bindRegister(QString res, int loc){}
        */

        /**
          * @brief attachRegister Attach a pointer to the cores register set
          * @param registers The Register object
          */
        virtual void attachRegister(Avr_Registers *regPtr) = 0;

         /**
         * @brief update Runs an update cycle for the hardware
         * @param cycles Number of cycles run by last instruction
         * @return An interrupt vector if any
         */
        virtual int update(int cycles) = 0;

        /**
         * @brief getInputs Returns a QMap of pointers to any required inputs
         * @return a QMap of pointers to inputs
         */
        virtual QMap <QString, uint8_t*> getInputs() = 0;

        /**
         * @brief getOutputs
         * @return a QMap of pointers to outputs
         */
        virtual QMap <QString, uint8_t>  getOutputs() = 0;

        /**
          * @brief Allows for the passing of general settings in
          *         text form
          * @param settings A QMap containing the settings and their keys
          */
        virtual void passSetting(QMap <QString, QString> setting){}

        /**
        * @brief Runs a full reset on the
        *   simulated hardware
        */
        virtual void reset(){};
};

Q_DECLARE_INTERFACE(Avr_Hardware_Interface, "com.kevin_luke.Avr_Core_Builder.HardwareUnit/1.0")


#endif // AVR_HARDWARE_INTERFACE_H


