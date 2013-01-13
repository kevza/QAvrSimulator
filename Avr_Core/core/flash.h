/**
 * @brief Classes for the AVR Flash memory simulation
 * @author Kevin Luke kevzawinning@gmail.com
 * @date 11/11/2012
 **/ 
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>


#ifndef AVR_FLASH_H
#define AVR_FLASH_H

/**
 * @brief Loads an AVR ihex file and pre-processes it into memory for 
 * the Avr Processor Core
 */ 
using namespace std;

class Avr_Flash{
	public:
		/**
		 * @brief Creates and instance of the Avr_Flash class
		 * 			of the argument size in bytes
		 * @param size The number of bytes for the flash memory size
		 */
		Avr_Flash(int size);
		
		/**
		 * @brief Clean up
		 */ 
		~Avr_Flash();
		
		/**
		 * @brief Loads a hex file into the flash memory
		 * @param hexFile The file path 
		 **/ 
		bool loadHex(string hexFile);
		
		/**
		 * @brief Returns a pointer to the flash memory array
		 **/
		uint16_t *getFlash();
		
		/**
		 * @brief Convert a Hex decimal character to an integer
		 */ 
 		uint16_t hexToDec(char c);
	
	private:
		/**
		 * @brief a pointer to the flash memory array
		 **/
		uint16_t *flash;
		int maxSize;
		
	
};

#endif //AVR_FLASH_H
