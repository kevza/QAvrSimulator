#ifndef AVR_TIMER_16B_H
#define AVR_TIMER_16B_H

#include "../Avr_Core/interface/avr_hardware_interface.h"
#include <QMap>
#include <stdint.h>

#define REG_COUNT 15

class Avr_Timer_16b: public QObject, public Avr_Hardware_Interface
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.Avr_Hardware_Interface" FILE "")
    Q_INTERFACES(Avr_Hardware_Interface)
    public:
        Avr_Timer_16b();
        ~Avr_Timer_16b();


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
       virtual void bindRegister(QString reg, uint8_t *ptr);


        /**
         *@brief Attach the full register set
         */
        void attachRegister(Avr_Registers *regPtr){}


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

    private:
        QMap<QString,int> registers;
        uint8_t* reg[REG_COUNT];
        uint16_t count;
        uint16_t oldCount;
        int prescaler;
        int d;
};

#endif // AVR_TIMER_16B_H
