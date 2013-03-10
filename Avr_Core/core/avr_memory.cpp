/**
*@brief This class defines the various types of 
* memory on the simulated AVR Chip
*@author Kevin Luke kevzawinning@gmail.com
*@date 15/11/2012
*/
#include "avr_memory.h"
#include <cstdio>
Avr_Memory::Avr_Memory(){


}

/**
*@brief Initialize the system ram
*/
void Avr_Memory::initRam(int size){
	this->ram = new uint8_t[size];
	for (int i = 0 ; i < size; i++){
		this->ram[i] = 0;
	}
	printf("Ram Size : %d",size);
	fflush(stdout);
	this->ramEnd = size;
}

/**
*@brief Initialize the system Epprom
*/
void Avr_Memory::initEprom(int size){
	this->eeprom = new uint8_t[size];

}

/**
*@brief Get pointer to Ram Memory
*/
uint8_t* Avr_Memory::getRam(){
	return this->ram;
}

/**
*@brief Return Ram Size
*/
int Avr_Memory::getRamEnd(){
	return this->ramEnd;
}
