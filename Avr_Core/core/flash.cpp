#include "flash.h"
#include <iostream>

/**
 * @brief Creates and instance of the Avr_Flash class
 * 			of the argument size in bytes
 * @param size The number of bytes for the flash memory size
 */
Avr_Flash::Avr_Flash(int size){
	size = size / 2; 
	this->flash = new uint16_t[size];
	this->maxSize = size;
}

/**
 * @brief Clean up
 */
Avr_Flash::~Avr_Flash(){
	delete[] this->flash;
}

/**
 * @brief Loads a hex file into the flash memory
 * @param hexFile The file path 
 * @return Bool Returns true for Success
 **/ 
bool Avr_Flash::loadHex(string hexFile){
	string line;
	ifstream file(hexFile.c_str());
	
    if (!file.is_open()){
		//File failed to open 
		return false;
	}
	
	//Load file into memory
	int flashPointer = 0;
	while (file >> line && flashPointer < maxSize){
		if (line.size() > 11){
			int len = line.size() - 11;
			line = line.substr(9, len);
			//get the whole instruction
			for (int i = 0 ; i < len; i += 4){
				//Convert the bytes to the expected order (AVR HEX
				//is little endian) Convert back to bigendian
				flash[flashPointer]	= hexToDec(line.at(i + 2)) << 12;
				flash[flashPointer] |= hexToDec(line.at(i + 3)) << 8; 	
				flash[flashPointer]	|= hexToDec(line.at(i)) << 4;
				flash[flashPointer] |= hexToDec(line.at(i + 1));
				flashPointer += 1;
			}
		}
	}
	
	flash[flashPointer] = 0;
	file.close();
	return true;
}

/**
 * @brief Returns a pointer to the flash memory array
 * @return flash A pointer to the flash array.
 **/
uint16_t* Avr_Flash::getFlash(){
	return flash;
}

/**
 * @brief Returns and integer for a hex numeral
 * @param c The character to convert
 * @return res The resulting integer
 */ 
uint16_t Avr_Flash::hexToDec(char c){
	uint16_t res;
	if (c - '0' > 9){
		res = c - 'A' + 10;
	}else{
		res = c - '0';
	}
	return res;
}
