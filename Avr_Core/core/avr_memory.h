#include <stdint.h>
#ifndef AVR_MEMORY_H
#define AVR_MEMORY_H
/**
*@brief This class defines the various types of 
* memory on the simulated AVR Chip
*@author Kevin Luke kevzawinning@gmail.com
*@date 15/11/2012
*/
class Avr_Memory{
	public:
        Avr_Memory();
        ~Avr_Memory();
		void initRam(int size);
		void initEprom(int size);
		uint8_t *getRam();
		int getRamEnd();
		
	private:
		uint8_t *ram;
		int ramEnd;
		//This will need some thought
		//as it needs to be persistent
		uint8_t *eeprom;

};
#endif //AVR_MEMORY_H
