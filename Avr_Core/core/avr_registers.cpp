#include "avr_registers.h"
#include <iostream>

/**
*@brief Constructor
*/
Avr_Registers::Avr_Registers(){
    this->rampd = NULL;
    this->rampx = NULL;
    this->rampy = NULL;
    this->rampz = NULL;
    this->eind = NULL;
    this->pc = 0;
}

/**
*@brief Destructor
*/
Avr_Registers::~Avr_Registers(){
	
}

/**
*@brief Sets a pointer the the Avr Ram
*/
void Avr_Registers::setRam(uint8_t *r){
	this->ram = r;
	this->io = r + 0x20;
}

/**
*@brief Sets the SREG Address
*/
void Avr_Registers::setSREGP(uint8_t *r){
	this->sreg = r;
}

//Setup Pointers for key register SP/PC/SREG

void Avr_Registers::setRAMPXP(uint8_t *p){
    this->rampx = p;
}

void Avr_Registers::setRAMPYP(uint8_t *p){
    this->rampy = p;
}

void Avr_Registers::setRAMPZP(uint8_t *p){
    this->rampz = p;
}

void Avr_Registers::setRAMPDP(uint8_t *p){
    this->rampd = p;
}

void Avr_Registers::setEINDP(uint8_t *p){
    this->eind = p;
}

//Setup Stack Pointer Address HB
void Avr_Registers::setStackPH(uint8_t *hb){
    this->spH = hb;
}

//Setup Stack Pointer Address LB
void Avr_Registers::setStackPL(uint8_t *lb){
    this->spL = lb;
}


/**
*@brief Returns if a bit in the SReg is set
*/
bool Avr_Registers::getSREG(uint8_t bit){
	if (*this->sreg & (1 << bit)){
		return true;
	}
	return false;
}

/**
*@brief Sets a bit in the SREG
*/
void Avr_Registers::setSREG(uint8_t bit, bool res){
	if (res){
		*this->sreg |= (1 << bit);
	}else{
		*this->sreg &= ~(1 << bit);
	}
}


void Avr_Registers::setRamEnd(int e){
	this->ramEnd = e;
}

/**
*@brief Returns the current Stack Pointer + any changes
*/
uint16_t Avr_Registers::sp(int t){ 
    uint8_t lowByte = *this->spL;
    uint8_t highByte = *this->spH;
	
	uint16_t sp_t = (highByte << 8) + lowByte + t;
	
	if (t != 0){
        *this->spL = sp_t & 0xff;
        *this->spH = (sp_t & 0xff00) >> 8;
	}

	return sp_t;
}


/**
*@brief Returns X register value
*/
uint16_t  Avr_Registers::getX(){
	return (this->ram[27]<< 8) | this->ram[26];
}

/**
*@brief Returns X register value
*/
uint16_t Avr_Registers::getY(){
	return (this->ram[29]<< 8) | this->ram[28];
}

/**
*@brief Returns X register value
*/
uint16_t Avr_Registers::getZ(){
	return (this->ram[31]<< 8) | this->ram[30];
}

/**
*@brief Set X
*/
void Avr_Registers::setX(uint16_t X){
	this->ram[27] = (X & 0xff00) >> 8; 
	this->ram[26] = X & 0xff;
}

/**
*@brief Set Y
*/
void Avr_Registers::setY(uint16_t Y){
	this->ram[29] = (Y & 0xff00) >> 8; 
	this->ram[28] = Y & 0xff;
}

/**
*@brief Set Z
*/
void Avr_Registers::setZ(uint16_t Z){
	this->ram[31] = (Z & 0xff00) >> 8; 
	this->ram[30] = Z & 0xff;
}

uint8_t Avr_Registers::getRampx(){
    if (this->rampx){
        return *this->rampx;
    }else{
        return 0;
    }
}
uint8_t Avr_Registers::getRampy(){
    if (this->rampy){
        return *this->rampy;
    }else{
        return 0;
    }
}
uint8_t Avr_Registers::getRampz(){
    if (this->rampz){
        return *this->rampz;
    }else{
        return 0;
    }
}
uint8_t Avr_Registers::getRampd(){
    if (this->rampd){
        return *this->rampd;
    }else{
        return 0;
    }
}
uint8_t Avr_Registers::getEind(){
    if (this->eind){
        return *this->eind;
    }else{
        return 0;
    }
}
