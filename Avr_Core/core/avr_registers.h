#ifndef AVR_REGISTERS_H
#define AVR_REGISTERS_H

#include <stdint.h>
#include <string>

class Avr_Registers{
	public:
		Avr_Registers();
		~Avr_Registers();

		//
		void setRam(uint8_t *r);
		void setRamEnd(int e);
        uint8_t ramG(int l);
        void ramS(int l, uint8_t s);
        uint8_t ioG(int l);
        void ioS(int l, uint8_t s);
		
        //Setup Pointers for key register SP/PC/SREG
		void setSREGP(uint8_t *r);
        void setRAMPXP(uint8_t *p);
        void setRAMPYP(uint8_t *p);
        void setRAMPZP(uint8_t *p);
        void setRAMPDP(uint8_t *p);
        void setEINDP(uint8_t *p);
        //Setup Stack Pointer Address
        void setStackPH(uint8_t *hb);
        void setStackPL(uint8_t *lb);
        //Get Stack Pointer or Update Value
		uint16_t sp(int t);

		
		//Get and set X Y Z Registers
		uint16_t getX();
		uint16_t getY();
		uint16_t getZ();
		
		void setX(uint16_t X);
		void setY(uint16_t Y);
		void setZ(uint16_t Z);
		//
		uint8_t getRampx();
		uint8_t getRampy();
		uint8_t getRampz();
		uint8_t getRampd();
		uint8_t getEind();
		
		//Status register and program counter
		uint8_t * sreg;
		//Setters and getters for sreg
		bool getSREG(uint8_t bit);
		void setSREG(uint8_t bit, bool res);
		
        int pc;
		
		//Ram and IO areas <- Will probably look at changing the structure of this
		uint8_t* ram;
		uint8_t* io;
        int ramEnd;

        //Records of reads and writes for hardware use
        int lastRead;
        int lastWrite;
		
	private:
		uint8_t* rampx;
		uint8_t* rampy;
		uint8_t* rampz;
		
		uint8_t* rampd;
		uint8_t* eind;

        uint8_t* spH;
        uint8_t* spL;
		
		
};
#endif //AVR_REGISTERS_H




