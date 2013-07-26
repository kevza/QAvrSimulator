#ifndef AVR_UART_H
#define AVR_UART_H

#include "../Avr_Core/interface/avr_hardware_interface.h"
#include <iostream>
#include <cstdio>
#include <QDebug>
#include "serial.h"



class Avr_Uart: public QObject, public Avr_Hardware_Interface
{
    Q_OBJECT
    Q_INTERFACES(Avr_Hardware_Interface)
    public:
        Avr_Uart();
        ~Avr_Uart();

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
       virtual void bindRegister(QString n, uint8_t *ptr);


        /**
         *@brief Attach the full register set
         */
        void attachRegister(Avr_Registers *regPtr){reg = regPtr;}

        /**
        * @brief update Runs an update cycle for the hardware
        * @param cycles Number of cycles run by last instruction
        * @return An interrupt vector if any
        */
       virtual int update(int cycles);

        /**
          *@brief returns the name of the plugin
          */
        virtual QString getPluginName();


        //Unused Virtual void functions
        virtual QMap<QString, unsigned char*> getInputs(){}
        virtual QMap<QString, unsigned char> getOutputs(){}
        void passSetting(QMap<QString,QString> setting);

    private:
        //Registers
        uint8_t *UDR;
        uint8_t *UCSRA;
        uint8_t *UCSRB;
        uint8_t *UCSRC;
        uint8_t *UCSRD;
        uint8_t *UBRRL;
        uint8_t *UBRRH;
        uint8_t lastUDR;

        uint8_t oldUCSRB;
        uint8_t oldUCSRC;
        //flags
        bool readComplete;
        bool writeComplete;
        bool isOpen;

        //Serial Port Comms
        Serial serial;
        QString port;
        QString baud;
        //Register File Main
        Avr_Registers *reg;

        //Functions
        bool openUart();

};

#endif // AVR_UART_H
