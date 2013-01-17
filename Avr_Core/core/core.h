#ifndef AVR_CORE_H
#define AVR_CORE_H

#include <stdint.h>
#include <string>
#include <cstdio>

#include "Avr_Core_global.h"
#include "avr_memory.h"
#include "flash.h"
#include "avr_registers.h"

#include <QThread>
#include <QDebug>
#include <QObject>
#include <QVector>
#include <QMutex>


#include "interface/avr_hardware_interface.h"

/**
 *@brief A Cpu simulator for Avr based Microprocessors
 *		built as a project to study software simulation of micros
 *		and to gain a greater understanding of the inner workings
 *		of Avr chips
 *@author Kevin Luke - kevzawinning@gmail.com
 *@date 8/11/2012
 */

/**
 * Avr Core for Cpu Simulation
 */ 
class Avr_Core : public QThread {
    Q_OBJECT
	public:
		Avr_Core(Avr_Flash *f, Avr_Memory *mem, Avr_Registers *regI);
        Avr_Core();
        virtual ~Avr_Core();

		/**
		 * @brief Decodes an instruction
		 * and updates all registers
		 */
		std::string decodeInstruction();

        /**
         *@Brief Sets Memory
         */
        void setMemory(Avr_Memory *mem);
        /**
         *@Brief Sets Flash
         */
        void setFlash(Avr_Flash *flash);
        /**
         *@Brief Sets Registers
         */
        void setRegisters(Avr_Registers *reg);
        /**
         *@Brief Sets the Processor to an interrupt
         */
        void interrupt(int pc);

        void step();

        //Ready for testing
		uint8_t *sreg;
		int pc;
        int cCount;
        Avr_Memory *mem;
        Avr_Registers *reg;
        Avr_Flash *flash;

        QVector <Avr_Hardware_Interface*> hardware;
        bool isThreadStopped;
        bool debug;
		
    private:
        void run();
        int jmp;


        //Pointers to Data
        uint8_t Rd, Rr, R,K,A, Q, B;
        uint16_t regX,regY,regZ, rampZ,rampX, rampY;
        uint lK, lR;
		//Get Registers from an instruction
		inline uint8_t getRegister4BitD(uint16_t inst);
		inline uint8_t getRegister4BitR(uint16_t inst);
		inline uint8_t getRegister5BitD(uint16_t inst);
		inline uint8_t getRegister5BitR(uint16_t inst);
		inline uint8_t getK8Bit(uint16_t inst);
		inline uint8_t getK6Bit(uint16_t inst);
		inline int8_t getK7Bit(uint16_t inst);
		inline uint8_t getA6Bit(uint16_t inst);
		inline uint8_t getA5Bit(uint16_t inst);

		
		//Set Flags
		inline bool setCarryFlag(uint8_t bit);
		inline bool setBorrowFlag(uint8_t bit);
		inline bool setSubBorrow(uint8_t bit);
		inline bool setSubBorrowK(uint8_t bit);
		inline bool setOverflow();
		inline bool setSubTCOverflow();
		inline bool setSubTCOverflowK();
		//Check instruction
		inline bool isTwoWord(uint16_t inst);	
};

#endif //AVR_CORE_H
