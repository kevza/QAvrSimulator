#include "core.h"
#include "opcode_masks.h"
#include <iostream>
#include <ctime>
/**
 *@brief A Cpu simulator for Avr based Microprocessors
 *      built as a project to study software simulation of micros
 *      and to gain a greater understanding of the inner workings
 *      of Avr chips
 *@author Kevin Luke - kevzawinning@gmail.com
 *@date 8/11/2012
 */
//Enumeration of SREG flags 
enum {C,Z,N,V,S,H,T,I};

#define BIT(X) (1 << X)



Avr_Core::Avr_Core(Avr_Flash *f, Avr_Memory *mem, Avr_Registers *regI){
	//Load Flash
    this->flash = f;
	reg = regI;
	
	this->sreg = reg->sreg;
    reg->pc = 0;
	//Setup Memory
	reg->setRam(mem->getRam());
	reg->setRamEnd(mem->getRamEnd());
	this->mem = mem;

    reg->setStackPH(mem->getRam() + 0x20 + 0x3e);
    reg->setStackPL(mem->getRam() + 0x20 + 0x3d);

	//Initialize Sreg to zero
	for (int i = 0 ; i < 8;i++){
		reg->setSREG(i, 0);
	}
}

Avr_Core::Avr_Core(){

}

Avr_Core::~Avr_Core(){

	//delete reg;
}

void Avr_Core::setMemory(Avr_Memory *mem){
    this->mem = mem;
}

void Avr_Core::setFlash(Avr_Flash *flash){
    this->flash = flash;
}

void Avr_Core::setRegisters(Avr_Registers *reg){
    //Set local pointer for sreg debuggin
    this->sreg = reg->sreg;
    //Set the Registers
    this->reg = reg;
    if (this->reg != NULL){
        this->reg->setRam(mem->getRam());
        this->reg->setRamEnd(mem->getRamEnd());
    }else{
        //Some form of error handling
        exit(1);
    }
}


void Avr_Core::run(){
    isThreadStopped = false;
    int interrupt = 0;

    while (!isThreadStopped){
        //Lock the thread
        this->decodeInstruction();
        foreach (Avr_Hardware_Interface *h, hardware){
            interrupt = h->update(this->cCount);
        }
        this->interrupt(interrupt);
    }
}

/**
 * @brief Avr_Core::step Step through instructions
 */
void Avr_Core::step(){
    bool state = this->debug;
    this->debug = true;
    debugQueue.push(this->decodeInstruction());
    int inter = -1;
    foreach (Avr_Hardware_Interface *h, hardware){
            inter = h->update(this->cCount);
    }
    this->interrupt(inter);
    this->debug = state;
}

/**
 * @brief Avr_Core::stringFormat Creates a string with format
 * @param fmt
 * @return The formatted string
 */
std::string Avr_Core::debugFormat(const char *fmt, ...)
{
    //Bail out if not in debug mode
    if (!this->debug){
        return "";
    }
    char *ret;
    va_list ap;
    va_start(ap, fmt);
    if (!vasprintf(&ret, fmt, ap)){
        qDebug() << "Form debug string for queue failed";
    };
    va_end(ap);

    std::string str(ret);
    free(ret);

    return str;
}

/**
*@brief Returns fist 4 bits in an instruction (Direct Address Mode)
*/
inline uint8_t Avr_Core::getRegister4BitR(uint16_t inst){
	return inst & 0x0f;
}

#define GET_REGISTER_4_BIT_R (inst & 0x0f)

/**
*@brief Returns fist 4 + 9 bits in an instruction (Direct Address Mode)
*/
inline uint8_t Avr_Core::getRegister5BitR(uint16_t inst){
    return (inst & 0x0f) | ((inst & 0x0200) >> 5);
}

#define GET_REGISTER_5_BIT_R ((inst & 0x0f) | ((inst & 0x0200) >> 5))

/**
*@brief Returns bits 4-7 from an instruction (Direct Address Mode)
*/
inline uint8_t Avr_Core::getRegister4BitD(uint16_t inst){
	return (inst >> 4) & 0x0f;
}

#define GET_REGISTER_4_BIT_D ((inst >> 4) & 0x0f)

/**
*@brief Returns bits 4-7 + 8from an instruction (Direct Address Mode)
*/
inline uint8_t Avr_Core::getRegister5BitD(uint16_t inst){
	return ((inst >> 4) & 0x1f);
}

#define GET_REGISTER_5_BIT_D ((inst >> 4) & 0x1f)
/**
*@brief Returns K from and opcode of format ooooKKKKddddKKKK
*/
inline uint8_t Avr_Core::getK6Bit(uint16_t inst){
	return ((inst & 0xc0) >> 2) | (inst & 0xf); 
}

#define GET_K_6_BIT ((inst & 0xc0) >> 2) | (inst & 0xf)

inline uint8_t Avr_Core::getK8Bit(uint16_t inst){
    return ((inst >> 4) & 0xf0) | (inst & 0xf);
}

#define GET_K_8_BIT (((inst >> 4) & 0xf0) | (inst & 0xf))

/**
*@brief Returns K from and opcode of format ooooooKKKKKKKsss
*/
inline int8_t Avr_Core::getK7Bit(uint16_t inst){
	uint8_t res;
	res = (inst & 0x3f8) >> 3;
    if (res & BIT(6)){
        res = ~((~res + 1) & 0x7f) + 1;
        return res;
	}
    return res;
}

/**
*@brief Returns A from and opcode of format ooooKKKKddddKKKK
*/
inline uint8_t Avr_Core::getA6Bit(uint16_t inst){
	return ((inst >> 5) & 0x30) | (inst & 0xf);  
}

#define GET_A_6_BIT (((inst >> 5) & 0x30) | (inst & 0xf))


inline uint8_t Avr_Core::getA5Bit(uint16_t inst){
	return ((inst >> 3) & 0x1f);  
}

#define GET_A_5_BIT ((inst >> 3) & 0x1f)




/**
*@brief Sets the Carry Flags (H,C) for the supplied bit
*/
inline bool Avr_Core::setCarryFlag(uint8_t bit){
    return ((reg->ram[Rr] & BIT(bit)) & ((~R) & BIT(bit)))
        | ((reg->ram[Rd] & BIT(bit)) & ((~R) & BIT(bit)))
        | ((reg->ram[Rd] & BIT(bit)) & (reg->ram[Rr] & BIT(bit)));
}

/**
*@brief Sets the Carry Flags (H,C) for the supplied bit
*/
inline bool Avr_Core::setBorrowFlag(uint8_t bit){
    return ((reg->ram[Rr] & (1 << bit)) & (R & (1 << bit)))
        | (((~reg->ram[Rd]) & (1 << bit)) & (R & (1 << bit)))
        | (((~reg->ram[Rd]) & (1 << bit)) & (reg->ram[Rr] & (1 << bit)));
}

/**
*@brief Sets the Sub Carry Flags (H,C) for the supplied bit
*/
inline bool Avr_Core::setSubBorrow(uint8_t bit){
    return (((~reg->ram[Rd]) & BIT(bit)) & (reg->ram[Rr] & BIT(bit))) |
            ((reg->ram[Rr] & BIT(bit)) & (R & BIT(bit))) |
            ((R & BIT(bit)) & ((~reg->ram[Rd]) & BIT(bit)));
}

/**
*@brief Sets the Sub Carry Flags (H,C) for the supplied bit
*/
inline bool Avr_Core::setSubBorrowK(uint8_t bit){
    return (((~reg->ram[Rd]) & BIT(bit)) & (K & BIT(bit))) |
            ((K & BIT(bit)) & (R & BIT(bit))) |
            ((R & BIT(bit)) & ((~reg->ram[Rd]) & BIT(bit)));
}

/**
*@brief Sets the Flags for Subs Twos complement overflow
*/
inline bool Avr_Core::setSubTCOverflow(){
    return ((reg->ram[Rd] & BIT(7)) & ((~reg->ram[Rr]) & BIT(7)) & ((~R) & BIT(7))) |
            (((~reg->ram[Rd]) & BIT(7)) & (reg->ram[Rr] & BIT(7)) & (R & BIT(7)));
}

/**
*@brief Sets the Flags for Subs Twos complement overflow
*/
inline bool Avr_Core::setSubTCOverflowK(){
    return ((reg->ram[Rd] & BIT(7)) & ((~K) & BIT(7)) & ((~R) & BIT(7))) |
            (((~reg->ram[Rd]) & BIT(7)) & (K & BIT(7)) & (R & BIT(7)));
}

/**
*@brief Sets the overflow flag
*/
bool Avr_Core::setOverflow(){
    return 	((reg->ram[Rd] &BIT(7)) &
            (reg->ram[Rr] & BIT(7)) &
            ((~R) & BIT(7))) |
            (((~reg->ram[Rd]) & BIT(7)) &
             ((~reg->ram[Rr]) & BIT(7)) &
			(R & 1 << 7));
}

/**
*@brief Returns a Value for q 
* 00q0-qq00-0000-0qqq
*/
#define GET_Q ((inst & 0x2000) >> 8) | ((inst & 0x0c00) >> 7) | (inst & 0x7)

/**
*@brief Checks if an instruction is a two word instruction
*/
inline bool Avr_Core::isTwoWord(uint16_t inst){
	if ((inst & 0xf000) == 0x9000){
		switch (inst & 0xf800){ 
			case LDS: case STS:
				return true;
			break;
		}
		switch (inst & 0xfe0e){
			case JMP:case CALL:
				return true;
			break;
		}
	}
	return false;
}

/**
 * @brief Avr_Core::interrupt Sets the program counter to an interrupt vector address
 *        and pushes old program counter to the stack
 * @param vector
 */
void Avr_Core::interrupt(int vector){
    //Changes the Program Counter and pushed old pc to the stack;
    if (vector == -1 || !reg->getSREG(I)){
        //No interrupt Return
        return;
    }

    //Push the program counter to the stack
    if (mem->getRamEnd() < 65536){
        //push pc to stack
        reg->ram[reg->sp(0)] = reg->pc & 0x00ff;
        reg->ram[reg->sp(-1)] =  reg->pc >> 8;
        reg->sp(-1);
    }else{
        //push pc to stack
        reg->ram[reg->sp(0)] = reg->pc & 0x0000ff;
        reg->ram[reg->sp(-1)] =  (reg->pc && 0x00ff00) >> 8;
        reg->ram[reg->sp(-1)] =  reg->pc >> 16;
        reg->sp(-1);
    }
    //Sets Program counter to interrupt vector
    reg->pc = vector;
}

/**
 * @brief Decodes an instruction and runs the requested operation
 **/ 
std::string Avr_Core::decodeInstruction(){
	//Setup to run instruction

    uint16_t inst = this->flash->getFlash()[reg->pc];

	//Instruction Decoded for debug information
	std::string res = "---";
	
	//Run the Instruction
	switch (inst & 0xf000){
		//0000 
		case 0x0000:
			if (inst == 0x0000){
				res  = "nop";
				reg->pc++;
			}else{
				switch (inst & 0xfc00){
					case ADD:
						//perform operation
						Rd = GET_REGISTER_5_BIT_D;
						Rr = GET_REGISTER_5_BIT_R;
						R = reg->ram[Rr] + reg->ram[Rd];
						//set flags
						reg->setSREG(H, this->setCarryFlag(3));
						
						reg->setSREG(V, this->setOverflow());
                        reg->setSREG(N, R & BIT(7));
						reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
						reg->setSREG(Z, R == 0);
						reg->setSREG(C,this->setCarryFlag(7));
						//Store Result
						reg->ram[Rd] = R;
						reg->pc++;
                        this->cCount = 1;
						res = "add";
					break;
					case CPC:
						Rd = GET_REGISTER_5_BIT_D;
						Rr = GET_REGISTER_5_BIT_R;
						R = reg->ram[Rd] - reg->ram[Rr] - reg->getSREG(C);
						//Set Flags
						reg->setSREG(H, this->setBorrowFlag(3));
						
                        reg->setSREG(V, ((reg->ram[Rd] & BIT(7)) & ((~reg->ram[Rr]) & BIT(7)) & ((~R) & BIT(7)))
                                        | (((~reg->ram[Rd]) & BIT(7)) & (reg->ram[Rr] & BIT(7)) & (R & BIT(7))));
                        reg->setSREG(N, (R & BIT(7)));
						reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
                        reg->setSREG(Z, !R && reg->getSREG(Z));
                        reg->setSREG(C, this->setSubBorrow(7));
						
						reg->pc++;
                        this->cCount = 1;
						res = "cpc";
						
					break;
					case SBC:
						Rd = GET_REGISTER_5_BIT_D;
						Rr = GET_REGISTER_5_BIT_R;
						R = reg->ram[Rd] - reg->ram[Rr] - reg->getSREG(C);
						//Process Flags
						reg->setSREG(H, setSubBorrow(3));
						
						reg->setSREG(V, this->setSubTCOverflow());
 						reg->setSREG(N, R & BIT(7));
 						reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
 						reg->setSREG(Z,!(R) && reg->getSREG(Z));
                        reg->setSREG(C,this->setSubBorrow(7));
						reg->ram[Rd] = R;
						reg->pc++;
                        this->cCount = 1;
						res = "sbc";
					break;
					default:
						int16_t resm;
						switch (inst & 0xff00){
							case MOVW:
								Rd = GET_REGISTER_4_BIT_D * 2;
								Rr = GET_REGISTER_4_BIT_R * 2;
							
								
								reg->ram[Rd] = reg->ram[Rr];
								reg->ram[Rd + 1] = reg->ram[Rr + 1];
								
								reg->pc++;
                                this->cCount = 1;
								res = "movw";
							break;
							case MULS:
								Rd = GET_REGISTER_4_BIT_D + 16;
								Rr = GET_REGISTER_4_BIT_D + 16;
								resm = (int8_t)reg->ram[Rd] * (int8_t)reg->ram[Rr];
								reg->setSREG(C,(resm >> 15) & 1);
								//Save Result
								reg->ram[0] = (uint8_t)(resm & 0xff); 
								reg->ram[1] = (uint8_t)(resm >> 8);
										
								reg->setSREG(Z,resm == 0);
								res = "muls";
                                this->cCount = 2;
								reg->pc++;						
							break;
							default:
								resm = 0;
								Rd = this->getRegister4BitD(inst & 0x0077) + 16;
								Rr = this->getRegister4BitD(inst & 0x0077) + 16;
								switch (inst & 0xff88){
									case FMUL:
										resm = reg->ram[Rd] * reg->ram[Rr];
										reg->setSREG(C,(resm >> 15) & 1);
										resm <<= 1;
                                        this->cCount = 2;
										res = "fmul";
									break;
									case FMULS:
										resm = (int8_t)reg->ram[Rd] * (int8_t)reg->ram[Rr];
										reg->setSREG(C,(resm >> 15) & 1);
										resm <<= 1;
                                        this->cCount = 2;
										res = "fmuls";
									break;
									case FMULSU:
										resm = (int8_t)reg->ram[Rd] * reg->ram[Rr];
										reg->setSREG(C,(resm >> 15) & 1);
										resm <<= 1;
                                        this->cCount = 2;
										res = "fmulsu";
									break;
									case MULSU:
										resm = (int8_t)reg->ram[Rd] * reg->ram[Rr];
										reg->setSREG(C,(resm >> 15) & 1);
                                        this->cCount = 2;
										res = "mulsu";
									break;
								}
								//Save the result of the preceeding instructions
								reg->ram[0] = (uint8_t)(resm & 0xff); 
								reg->ram[1] = (uint8_t)(resm >> 8);	
								reg->setSREG(Z,resm == 0);
								reg->pc++;	
								
						}
					break;
				}
			}
		break;
		//0001
		case 0x1000:
			switch (inst & 0xfc00){
				case CPSE:
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
                    this->cCount = 1;
					if (reg->ram[Rd] == reg->ram[Rr]){
                        //Skip next instruction
                        this->cCount += 1;
						reg->pc++;
						//Skip again if inst is DWORD
                        if(isTwoWord(this->flash->getFlash()[reg->pc])){
                            this->cCount += 1;
							reg->pc++;
						}
					}
					reg->pc++;
					res = "cpse";
				break;
				case CP:
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
					R = reg->ram[Rd] - reg->ram[Rr];
					reg->setSREG(H,this->setSubBorrow(3));
					
					reg->setSREG(V,this->setSubTCOverflow());
					reg->setSREG(N,R & BIT(7));
					reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
					reg->setSREG(Z,R == 0);
                    reg->setSREG(C,this->setSubBorrow(7));
					reg->pc++;
                    this->cCount = 1;
					res = "cp";
				break;
				case SUB:
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
					R = reg->ram[Rd] - reg->ram[Rr];
					reg->setSREG(H,this->setSubBorrow(3));
					reg->setSREG(V,this->setSubTCOverflow());
					reg->setSREG(N,R & BIT(7));
					reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
					reg->setSREG(Z,R == 0);
					reg->setSREG(C,this->setSubBorrow(7));
					reg->ram[Rd] = R;
					reg->pc++;
                    this->cCount = 1;
					res = "sub";
				break;
				case ADC: //also ROL
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
					R = reg->ram[Rd] + reg->ram[Rr] + reg->getSREG(C);

					reg->setSREG(H,this->setCarryFlag(3));
					reg->setSREG(V,this->setOverflow());
                    reg->setSREG(N,R & BIT(7));
					reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
					reg->setSREG(Z,R == 0);
					reg->setSREG(C,this->setCarryFlag(7));
					reg->ram[Rd] = R;
					reg->pc++;
                    this->cCount = 1;
					res = "adc";
				break;

			}
		break;
		
		//0010
		case 0x2000:
			switch (inst & 0xfc00){
				case AND: 
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
					R = reg->ram[Rd] & reg->ram[Rr];
					
					reg->setSREG(V, 0);
                    reg->setSREG(N, R & BIT(7));
					reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
					reg->setSREG(Z, R == 0);
					reg->ram[Rd] = R;
                    this->cCount = 1;
					reg->pc++;
					res = "and";
				break;
				case EOR: //also CLR
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
					R = reg->ram[Rd] ^ reg->ram[Rr];
					reg->setSREG(V, 0);
                    reg->setSREG(N, R & BIT(7));
					reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
					reg->setSREG(Z, R == 0);
					reg->ram[Rd] = R;
					reg->pc++;
                    this->cCount = 1;
					res = "eor";
				break;
				case MOV:
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
					reg->ram[Rd] = reg->ram[Rr];
					reg->pc++;
                    this->cCount = 1;
					res = "mov";
				break;
				case OR:
					Rd = GET_REGISTER_5_BIT_D;
					Rr = GET_REGISTER_5_BIT_R;
					R = reg->ram[Rd] | reg->ram[Rr];
			
					
					reg->setSREG(V, 0);
                    reg->setSREG(N, R & BIT(7));
					reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
					reg->setSREG(Z, R == 0);
					
					reg->ram[Rd] = R;
                    this->cCount = 1;
					reg->pc++;
					res = "or";
				break;
			}
		break;

		//0011
		case CPI:
			//CPI Instruction
			Rd = GET_REGISTER_4_BIT_D + 16;
			K =  GET_K_8_BIT;
			R = reg->ram[Rd] - K;
			reg->setSREG(H, this->setSubBorrowK(3));
			
			reg->setSREG(V, this->setSubTCOverflowK());
			reg->setSREG(N, R & 0x80);
			reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
			reg->setSREG(Z, R == 0);
            reg->setSREG(C, this->setSubBorrowK(7));
			
			res = "cpi";
            this->cCount = 1;
            reg->pc++;
		break;

		//0100
		case SBCI:
			K = GET_K_8_BIT;
			Rd = GET_REGISTER_4_BIT_D + 16;

			R = reg->ram[Rd] - K - reg->getSREG(C);
			
			reg->setSREG(H, this->setSubBorrowK(3));
			
			reg->setSREG(V, this->setSubTCOverflowK());
			reg->setSREG(N, R & BIT(7));
			reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
            reg->setSREG(Z, (R == 0) && reg->getSREG(Z));
			reg->setSREG(C, this->setSubBorrowK(7));
			
			reg->ram[Rd] = R;
			reg->pc++;
            this->cCount = 1;
			res = "sbci";
		break;

		case SUBI:
			K = GET_K_8_BIT;
			Rd = GET_REGISTER_4_BIT_D + 16;
			R = reg->ram[Rd] - K;
			
			reg->setSREG(H, this->setSubBorrowK(3));
			
			reg->setSREG(V, this->setSubTCOverflowK());
			reg->setSREG(N, R & BIT(7));
			reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
			reg->setSREG(Z,(R == 0));
			reg->setSREG(C,this->setSubBorrowK(7));
			
			reg->ram[Rd] = R;
			reg->pc++;
            this->cCount = 1;
			res = "subi";
		break;

		//0110
		case ORI:	//also SBR
			K = GET_K_8_BIT;
			Rd = GET_REGISTER_4_BIT_D + 16;
            R = reg->ram[Rd] | K;

			
			reg->setSREG(V, 0);
			reg->setSREG(N, R & BIT(7));
			reg->setSREG(Z, (R == 0));
            reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
			
			reg->ram[Rd] = R;
			reg->pc++;
            this->cCount = 1;
			res = "ori";
		break;

		//0111
		case ANDI:
			K = GET_K_8_BIT;
			Rd = GET_REGISTER_4_BIT_D + 16;
            R = reg->ram[Rd] & K;
			reg->setSREG(V, 0);
			reg->setSREG(N, R & BIT(7));
            reg->setSREG(Z,(R == 0));
			reg->ram[Rd] = R;
			reg->pc++;
            this->cCount = 1;
			res = "andi";
		break;
    //Still some clashes to sort
    case 0x8000:case 0xa000:
        switch (inst & 0x0208){
            case LDYQ:
                Rd = GET_REGISTER_5_BIT_D;
                Q = GET_Q;
                regY = reg->getY();
                reg->ram[Rd] = reg->ram[regY + Q];
                reg->pc++;
                this->cCount = 2;
                res = "ldyq";
                break;
            case LDZQ:
                Rd = GET_REGISTER_5_BIT_D;
                Q = GET_Q;
                regZ = reg->getZ();
                reg->ram[Rd] = reg->ram[regZ + Q];
                this->cCount = 2;
                reg->pc++;
                res = "ldzq";
                break;
            case STYQ:
                Rd = GET_REGISTER_5_BIT_D;
                Q = GET_Q;
                regY = reg->getY();
                //Copy the Data
                reg->ram[regY + Q] = reg->ram[Rd];
                this->cCount = 2;
                reg->pc++;
                res = "styq";
             break;
             default:
                if (0){//(inst & 0xA800) == LDS1){
                    Rd = GET_REGISTER_4_BIT_D + 16;
                    K = (inst & 0xf);
                    K |= (inst & BIT(9)) >> 5;
                    K |= (inst & BIT(10)) >> 5;
                    K |= ((~inst) & BIT(8)) >> 1;
                    K |= (inst & BIT(8)) >> 2;
                    reg->ram[Rd] = reg->ram[K];
                    reg->pc++;
                    this->cCount = 1;
                    res = "lds1";
                }else if ((inst & STZ3) == STZ3){
                    // STZ3:
                    Q = GET_Q;
                    Rr = GET_REGISTER_5_BIT_D;
                    reg->ram[reg->getZ() + Q] = reg->ram[Rr];
                    reg->pc++;
                    this->cCount = 1;
                    res = "stz";

                }else if ((inst & STS1) == STS1){
                    // STS1:
                    Rd = GET_REGISTER_4_BIT_D + 16;
                    K = (inst & 0xf);
                    K |= (inst & BIT(9)) >> 5;
                    K |= (inst & BIT(10)) >> 5;
                    K |= ((~inst) & BIT(8)) >> 1;
                    K |= (inst & BIT(8)) >> 2;
                    //qDebug() << "STS1" << (int)K;
                    reg->ram[K] = reg->ram[Rd];
                    reg->pc++;
                    this->cCount = 1;
                    res = "sts1";
                }
            break;
        }
        break;
		//1001
		case 0x9000:
			switch (inst & 0xfe0e){
				case JMP:
					//JMP Command
					reg->pc++;
                    K = ((inst & 0x8f0)) >> 3 | (inst & 1);
                    reg->pc = (K << 16) | this->flash->getFlash()[reg->pc];
                    this->cCount = 3;
					res = "jmp";
				break;
                case CALL:
                    K = ((inst & 0x8f0)) >> 3 | (inst & 1);
                    reg->pc+=2;
					if (mem->getRamEnd() < 65536){
                        //push pc to stack
                        this->cCount = 4;
                        reg->ram[reg->sp(0)] = reg->pc & 0xff;
                        reg->ram[reg->sp(-1)] =  (reg->pc >> 8) & 0xff;
						reg->sp(-1);
					}else{
						//push pc to stack			
                        this->cCount = 5;
                        reg->ram[reg->sp(0)] = reg->pc & 0xff;
                        reg->ram[reg->sp(-1)] =  (reg->pc >> 8) & 0xff;
                        reg->ram[reg->sp(-1)] =  (reg->pc >> 16) & 0xff;
						reg->sp(-1);
					}
                    reg->pc -= 1;
                    reg->pc =  (K << 16) |this->flash->getFlash()[reg->pc];
					res = "call";
				break;
				
				default:
					switch (inst & 0xff00){
						case ADIW:
							K = GET_K_6_BIT;
							Rd = ((inst & 0x30) >> 4) * 2 + 24;
                            lK = ((reg->ram[Rd + 1] << 8) | reg->ram[Rd]);
                            lK+= K;
                            reg->setSREG(V, !(reg->ram[Rd+1] & BIT(7)) && (lK  & BIT(15)));
                            reg->setSREG(N, (lK & BIT(15)));
							reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
							reg->setSREG(Z, (lK == 0));
                            reg->setSREG(C, (reg->ram[Rd+1] & BIT(7)) && !(lK & BIT(15)));
							//Return Values to Memory
							reg->ram[Rd + 1] = (lK & 0xff00) >> 8;
							reg->ram[Rd] = lK & 0xff;
							reg->pc++;
                            this->cCount = 2;
							res = "adiw";
						break;
						case CBI:
							A = GET_A_5_BIT;
							B = inst & 0x7;
                            reg->io[A] &= ~(BIT(B));
                            this->cCount = 1;
                            res = "cbi";
                            reg->pc++;
						break;
						case SBI:
							reg->io[((inst & 0xf8) >> 3)] |= (1 << (inst & 0x7));
							reg->pc++;
                            this->cCount = 1;
							res = "sbi";
						break;
						case SBIC:
							if (reg->io[((inst & 0xf8) >> 3)] & (1 << (inst & 0x7))){
                                this->cCount = 1;
								reg->pc++;	
							}else{
                                if (this->isTwoWord(this->flash->getFlash()[reg->pc + 1])){
                                    this->cCount = 3;
                                    reg->pc += 3;
                                }else{
                                    this->cCount = 2;
                                    reg->pc+= 2;
                                }
							}
							res = "sbic";
						break;
						case SBIS:
							if (reg->io[(inst & 0xf8) >> 3] & (1 << (inst & 0x7))){

                                if (this->isTwoWord(this->flash->getFlash()[reg->pc + 1])){
                                    this->cCount = 3;
                                    reg->pc += 3;
                                }else{
                                    this->cCount = 2;
                                    reg->pc+= 2;
                                }
							}else{
                                this->cCount = 1;
								reg->pc++;
							}
							res = "sbis";
						break;
						case SBIW:
							K = GET_K_6_BIT;
							Rd = ((inst & 0x30) >> 4) * 2 + 24;
                            lK = ((reg->ram[Rd + 1] << 8) | reg->ram[Rd]);
                            lK -= K;
							//Set the Flags

                            reg->setSREG(V, (reg->ram[Rd+1] & BIT(7)) & (~(lK >> 8 ) & BIT(7)));
                            reg->setSREG(N, (lK & BIT(15)));
							reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
							reg->setSREG(Z, (lK == 0));
                            reg->setSREG(C,((~reg->ram[Rd+1]) & BIT(7)) & ((lK >> 8) & BIT(7)));
							//Return Values to Memory
							reg->ram[Rd + 1] = (lK & 0xff00) >> 8;
							reg->ram[Rd] = lK & 0xff;	
							reg->pc++;
                            this->cCount = 2;
							res = "sbiw";
						break;
						default:
							switch (inst & 0xfe0f){
								case ASR:
									Rd = GET_REGISTER_5_BIT_D;
									R = (reg->ram[Rd]) >> 1;
									R |= (reg->ram[Rd] & BIT(7));
									
                                    reg->setSREG(C, (reg->ram[Rd] & BIT(1)));
									reg->setSREG(N, (R & BIT(7)));
									reg->setSREG(V, reg->getSREG(N) != reg->getSREG(C));
									reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
									reg->setSREG(Z, (R == 0));
									reg->ram[Rd] = R;
									reg->pc++;
                                    this->cCount = 1;
									res = "asr";
								break;
								case DEC:
									Rd = GET_REGISTER_5_BIT_D;
									R = reg->ram[Rd] - 1;
									//Set Flags
									
									reg->setSREG(V, (reg->ram[Rd] == 0x80));
                                    reg->setSREG(N, (R & BIT(7)));
									reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
									reg->setSREG(Z,(R==0));
									reg->ram[Rd] = R;
									reg->pc++;
                                    this->cCount = 1;
									res = "dec";
								break;
								case ELPMII:
									/*to do*/
								break;
								case ELPMIII:
									/*to do*/
								break;
								case INC:
									Rd = GET_REGISTER_5_BIT_D;
									R = reg->ram[Rd] + 1;
									//Set Flags
									
									reg->setSREG(V, (reg->ram[Rd] == 0x7f));
                                    reg->setSREG(N, (R & BIT(7)));
									reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
									reg->setSREG(Z,(R==0));
									reg->ram[Rd] = R;
									reg->pc++;
                                    this->cCount = 1;
									res = "inc";
								break;
								case LAC:
									Rd = getRegister5BitD(inst);
									regZ = reg->getZ();
									reg->ram[regZ] = reg->ram[Rd] & (0xff - reg->ram[regZ]);
									reg->pc++;
                                    this->cCount = 1;
									res = "lac";
								break;
								case LAS:
									Rd = getRegister5BitD(inst);
									regZ = reg->getZ();
									reg->ram[regZ] = reg->ram[Rd] | reg->ram[regZ];
									reg->ram[Rd] = reg->ram[regZ];
									reg->pc++;
                                    this->cCount = 1;
									res = "las";
								break;
								case LAT:
									Rd = getRegister5BitD(inst);
									regZ =reg->getZ();
									reg->ram[regZ] = reg->ram[Rd] ^ reg->ram[regZ];
									reg->ram[Rd] = reg->ram[regZ];
									reg->pc++;
                                    this->cCount = 1;
									res = "lat";
								break;
								case LD1:case LD2:case LD3:
									//Note there are some variations here for low
									//and high memory devices -- To Do
									Rd = GET_REGISTER_5_BIT_D;
									regX = reg->getX();
									switch (inst & 0x3){
                                        case 0x0:
                                            this->cCount = 1;
                                            reg->ram[Rd] = reg->ram[regX];
                                        break;
										case 0x1:
											reg->ram[Rd] = reg->ram[regX];
											regX++;
                                            this->cCount = 2;
											reg->setX(regX);
                                        break;
										case 0x2:
											regX--;
											reg->setX(regX);
                                            this->cCount = 2;
											reg->ram[Rd] = reg->ram[regX];
										break;

									}

									reg->pc++;
									res = "ld";
								break;
								case LDS:
									//Note there are some variations here for low
									//and high memory devices -- To Do
									Rd = GET_REGISTER_5_BIT_D;
									reg->pc++;
                                    lK  = this->flash->getFlash()[reg->pc];
									reg->ram[Rd] = reg->ram[lK];
									reg->pc++;
                                    this->cCount = 2;
									res = "lds"; 
								break;
								case LPM2:
									Rd = GET_REGISTER_5_BIT_D;
									regZ =reg->getZ();
                                    rampZ = reg->getRampz();
									if (regZ & 0x1){
                                        reg->ram[Rd] = (this->flash->getFlash()[(regZ >> 1) + rampZ] >> 8) & 0xff;
									}else{	
                                        reg->ram[Rd] = this->flash->getFlash()[(regZ >> 1) + rampZ] & 0xff;
                                    }
                                    reg->pc++;
                                    this->cCount = 3;
									res = "lpm";

								break;
								case LPM3:
									Rd = GET_REGISTER_5_BIT_D;
									regZ = reg->getZ();
									rampZ = reg->getRampz();
									if (regZ & 0x1){
                                        reg->ram[Rd] = (this->flash->getFlash()[(regZ >> 1) + rampZ ] >> 8)& 0xff;
									}else{	
                                        reg->ram[Rd] = this->flash->getFlash()[(regZ >> 1) + rampZ] & 0xff;
                                    }
									regZ++; reg->setZ(regZ);
									reg->pc++;
                                    this->cCount = 3;
									res = "lpm+";
								break;
								case LSR:
									Rd = GET_REGISTER_5_BIT_D;
                                    R = reg->ram[Rd] >> 1;
									//Set Flags
									
                                    reg->setSREG(C, reg->ram[Rd] & BIT(0));
									reg->setSREG(N,0);
									reg->setSREG(V, reg->getSREG(N) != reg->getSREG(C));
									reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
									reg->setSREG(Z,R == 0);
									//Set the Value
									reg->ram[Rd] = R;
									reg->pc++;
                                    this->cCount = 1;
									res = "lsr";
								break;
								case NEG:
									Rd = GET_REGISTER_5_BIT_D;
									R = 0x0 - reg->ram[Rd];
									//Set Flags
									reg->setSREG(H, (R & BIT(3)) | (reg->ram[Rd] & BIT(3)));
									
									reg->setSREG(V, R == 0x80);
									reg->setSREG(N, R & BIT(7));
									reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
									reg->setSREG(Z, R == 0);
									reg->setSREG(C, !(R == 0));
									reg->ram[Rd] = R;
									reg->pc++;
                                    this->cCount = 1;
									res = "neg";
								break;
								case POP:
									Rd = GET_REGISTER_5_BIT_D;
									reg->ram[Rd] = reg->ram[reg->sp(1)];
									reg->pc++;
                                    this->cCount = 2;
									res = "pop";
								break;
								case PUSH:
									Rd = GET_REGISTER_5_BIT_D;
									reg->ram[reg->sp(0)] = reg->ram[Rd];
									reg->sp(-1);
									reg->pc++;
                                    this->cCount = 2;
									res = "push";
								break;
								case ROR:
									Rd = GET_REGISTER_5_BIT_D;
                                    //reg->setSREG(C, (reg->ram[Rd] & 0x1));
                                    //Hold tmp copy to set sreg
                                    Rr = reg->ram[Rd];
                                    R = reg->ram[Rd] >> 1;
									if (reg->getSREG(C)){
										R |= BIT(7);
									}
                                    reg->setSREG(C, (Rr & 0x1));
									reg->setSREG(N, reg->getSREG(C));
									reg->setSREG(Z, R == 0);
									reg->setSREG(V, reg->getSREG(N) != reg->getSREG(C)); 
									reg->setSREG(S,reg->getSREG(N) != reg->getSREG(V));
									reg->ram[Rd] = R;
									reg->pc++;
                                    this->cCount = 1;
									res = "ror";
								break;
								case ST: case ST1: case ST2:
									Rr = GET_REGISTER_5_BIT_D;
									regX = reg->getX();
									switch (inst & 0x3){
                                        case 0x0:
                                            this->cCount = 1;
                                            reg->ram[regX] = reg->ram[Rr];
                                            res = "st";
                                        break;
										case (0x1):
                                            this->cCount = 2;
											reg->ram[regX] = reg->ram[Rr];
											regX++;
											reg->setX(regX);
                                            res = "st+";
										break;
										case (0x2):
                                            this->cCount = 2;
											regX--;
											reg->setX(regX);
											reg->ram[regX] = reg->ram[Rr];
                                            res = "-st";
										break;
									}
									reg->pc++;

								break;
								case STZ1:
									Rr = GET_REGISTER_5_BIT_D;
									regZ = reg->getZ();
									reg->ram[regZ] = reg->ram[Rr];
									regZ++; 
									reg->setZ(regZ);
									reg->pc++;
                                    this->cCount = 2;
                                    res = "stz";
								break;
								case STZ2:
									Rr = GET_REGISTER_5_BIT_D;
									regZ = reg->getZ();
									regZ--;
									reg->ram[regZ] = reg->ram[Rr];
									reg->setZ(regZ);
									reg->pc++;
                                    this->cCount = 2;
									res = "stz";
								break;
								
								case STSL:
									Rd = GET_REGISTER_5_BIT_D;
									reg->pc++;
                                    lK = this->flash->getFlash()[reg->pc];
									reg->ram[lK] = reg->ram[Rd];
									reg->pc++;
                                    this->cCount = 2;
									res = "stsl";
								break;
								case SWAP:
									Rd = GET_REGISTER_5_BIT_D;
									K = reg->ram[Rd];
									reg->ram[Rd] = (K << 4) | (K >> 4);
									reg->pc++;
                                    this->cCount = 1;
									res = "swap";
								break;
								case XCH:
									Rd = GET_REGISTER_5_BIT_D;
									K  = reg->ram[Rd];
									reg->ram[Rd] = reg->ram[reg->getZ()];
									reg->ram[reg->getZ()] = K;
									reg->pc++;
                                    this->cCount = 1;
									res = "xch";
								break;
								
								case LDYP:
									Rd = GET_REGISTER_5_BIT_D;
									regY = reg->getY();
                                    reg->ram[Rd] = reg->ram[regY];
									regY++;
									reg->setY(regY);
									reg->pc++;
                                    this->cCount = 2;
									res = "ldyp";
								break;
								case LDYM:
									Rd = GET_REGISTER_5_BIT_D;
									regY = reg->getY();
									regY--;
									reg->ram[Rd] = reg->ram[regY];
									reg->setY(regY);
									reg->pc++;
                                    this->cCount = 2;
									res = "ldym";
								break;
								case LDZP:
									Rd = GET_REGISTER_5_BIT_D;
									regZ = reg->getZ();
									reg->ram[Rd] = reg->ram[regZ];
									regZ++;
                                    reg->setZ(regZ);
									reg->pc++;
                                    this->cCount = 2;
									res = "ldzp";
								break;
									
								case LDZM:
									Rd = GET_REGISTER_5_BIT_D;
									regZ = reg->getZ();
									regZ--;
									reg->ram[Rd] = reg->ram[regZ];
									reg->setZ(regZ);
									reg->pc++;
                                    this->cCount = 2;
									res = "ldzm";
								break;
								
								case STYP:
									Rd = GET_REGISTER_5_BIT_D;
									regY = reg->getY();
									reg->ram[regY] = reg->ram[Rd];
									regY++;
									reg->setY(regY);						
									reg->pc++;
                                    this->cCount = 2;
									res = "styp";
								break;
								case STYM:
									Rd = GET_REGISTER_5_BIT_D;
									regY = reg->getY();
									regY--;
									reg->ram[regY] = reg->ram[Rd];
									reg->setY(regY);
									reg->pc++;
                                    this->cCount = 2;
									res = "stym";
								break;
								default:
									switch (inst & 0xffff){
										case BREAK:
											/*to do*/
										break;
										case EICAL:
											//Push the pc to the stack
											reg->ram[reg->sp(0)] = (reg->pc + 1) && 0xff;
											reg->ram[reg->sp(-1)] = ((reg->pc + 1) >> 8) && 0xff;
											reg->ram[reg->sp(-1)] = ((reg->pc + 1) >> 8) && 0xff;
											reg->sp(-1);
											
											//Set the program counter
											reg->pc = reg->getZ() | (reg->getEind() << 16);
											//Also need to set pc(21:16) to EIND
                                            this->cCount = 4;
											res = "eical";
										break;
										case ICALL:
											reg->pc++;
											if (mem->getRamEnd() < 65536){
												//push pc to stack				
												reg->ram[reg->sp(0)] = reg->pc & 0x00ff;
												reg->ram[reg->sp(-1)] =  reg->pc >> 8;
												reg->sp(-1);
                                                this->cCount = 3;
											}else{
												//push pc to stack			
												reg->ram[reg->sp(0)] = reg->pc & 0x0000ff;
                                                reg->ram[reg->sp(-1)] =  (reg->pc & 0x00ff00) >> 8;
												reg->ram[reg->sp(-1)] =  reg->pc >> 16;
												reg->sp(-1);
                                                this->cCount = 4;
											}
											reg->pc = reg->getZ(); //Eind assumed to be 0
											res = "icall";
										break;
										case EIJMP:
											reg->pc = reg->getZ() | (reg->getEind() << 16);
                                            this->cCount = 2;
											res = "eijmp";
										break;
										case ELPMI:
											/*to do*/
											
											res = "elpmi";
										break;
										case IJMP:
											reg->pc = reg->getZ();
                                            this->cCount = 2;
											res = "ijmp";
										break;
                                        case LPM: //R0 implied
											regZ = reg->getZ();
											rampZ = reg->getRampz();
											if (regZ & 0x1){
                                                reg->ram[0] = (this->flash->getFlash()[(regZ >> 1) + rampZ] >> 8) & 0xff;
											}else{	
                                                reg->ram[0] = this->flash->getFlash()[(regZ >> 1) + rampZ] & 0xff;
                                            }
											reg->pc++;
                                            this->cCount = 3;
											res = "lpm";
										break;
										case RET:case RETI:
                                            if (mem->getRamEnd() < 65536){
                                                reg->pc = reg->ram[reg->sp(1)] << 8;
                                                reg->pc |= reg->ram[reg->sp(1)];
                                                this->cCount = 4;
                                            }else{
                                                reg->pc = reg->ram[reg->sp(1)] << 16;
                                                reg->pc |= reg->ram[reg->sp(1)] << 8;
                                                reg->pc |= reg->ram[reg->sp(1)];
                                                this->cCount = 5;
                                            }
                                            res = "ret";
											if (inst & 0x10){
												reg->setSREG(I,1);
                                                res = "reti";
											}
										break;
										

										case SLEEP:
											/*to do*/
											
											res = "sleep";
										break;
										case SPM:case SPM2:
											/*to do*/
											
											res = "spm";
										break;
										case WDR:
											/*to do*/
											reg->pc++;
                                            this->cCount = 1;
											res = "wdr";
										break;
										default:
											if (COM){
												Rd = GET_REGISTER_5_BIT_D;
												reg->ram[Rd] = 0xff - reg->ram[Rd];
												//Set Flags
												
												reg->setSREG(V,0);
                                                reg->setSREG(N,reg->ram[Rd] & BIT(7));
												reg->setSREG(S, reg->getSREG(N) != reg->getSREG(V));
												reg->setSREG(Z, reg->ram[Rd] == 0);
												reg->setSREG(C, 1);
												reg->pc++;
                                                this->cCount = 1;
												res = "com";
											}else if (DES){
												/*to do*/
                                                this->cCount = 2;
                                                this->pc++;
												res = "des";
											}else if (MUL){
												Rr = GET_REGISTER_5_BIT_R;
												Rd = GET_REGISTER_5_BIT_D;
												lR = (uint16_t)reg->ram[Rr] * (uint16_t)reg->ram[Rd];
												reg->ram[0] = (lR & 0xff);
												reg->ram[1] = (lR & 0xff00) >> 8;												
												reg->setSREG(C, lR & BIT(15));
												reg->setSREG(Z, lR == 0);
                                                this->cCount = 2;
												reg->pc++;
												res = "mul";
											}else if (BCLR){
												//Clear Selected Register
												reg->setSREG((inst ^ 0x9488) >> 4, 0);
												reg->pc++;
                                                this->cCount = 1;
												res = "bclr";
											}else if (BSET){
												//Set Selected Register
												reg->setSREG((inst - 0x9408) >> 4,1);
												reg->pc++;
                                                this->cCount = 1;
												res = "bset";
											}
											
										break;

									}	
								break;

							}
						break;

					}
				
				break;
			}
		break;
		//1011
		case 0xb000:
			switch (inst & 0xf800){
				case IN:
					A = GET_A_6_BIT;
                    Rd = GET_REGISTER_5_BIT_D;
                    //Debug information
                    res = debugFormat("in R%d %d",(int)Rd, (int)A);

					reg->ram[Rd] = reg->io[A];
                    this->cCount = 1;
					reg->pc++;
				break;
				case OUT:
					A = GET_A_6_BIT;
					Rr = GET_REGISTER_5_BIT_D;
                    res = debugFormat("out %d R%d",(int)A, (int)Rr);
					reg->io[A] = reg->ram[Rr];
					res = "out";
                    this->cCount = 1;
					reg->pc++;
				break;
			}
		break;
		
		//1100
		case RJMP:
			jmp = inst & 0xfff;
			if (jmp & 0x800){
				//Negative Number
                jmp = ((~jmp & 0xfff) + 1) & 0xfff;
                reg->pc = reg->pc - jmp + 1;
                //Debug information
                res = debugFormat("rjmp -%d",(int)jmp);
            }else{
				reg->pc = reg->pc + jmp + 1;
                //Debug information
                res = debugFormat("rjmp %d",(int)jmp);

			}
            this->cCount = 2;
		break;

		//1101
		case RCALL:
			//Call a function and push pc to the stack
			reg->pc++;
			if (mem->getRamEnd() < 65536){
				//push pc to stack				
				reg->ram[reg->sp(0)] = reg->pc & 0x00ff;
				reg->ram[reg->sp(-1)] =  reg->pc >> 8;
				reg->sp(-1);
                this->cCount = 3;
			}else{
				//push pc to stack			
				reg->ram[reg->sp(0)] = reg->pc & 0x0000ff;
				reg->ram[reg->sp(-1)] =  (reg->pc && 0x00ff00) >> 8;
				reg->ram[reg->sp(-1)] =  reg->pc >> 16;
				reg->sp(-1);
                this->cCount = 4;
			}
			//Make Jump
			jmp = inst & 0xfff;
			if (jmp & 0x800){
                jmp = ((~jmp & 0xfff) + 1) & 0xfff;
                reg->pc = reg->pc - jmp;
			}else{
                reg->pc = reg->pc + jmp;
			}	
			res = "rcall";
		break;

		//1110
		case LDI://ser
			Rd = GET_REGISTER_4_BIT_D + 16;
			K = GET_K_8_BIT;
			reg->ram[Rd] = K;
			res ="ldi";
            this->cCount = 1;
			reg->pc++;
		break;

		//1111
		case 0xf000:
			if ((inst & 0xfe08) == BLD){
				Rd = GET_REGISTER_5_BIT_D;
				B = inst & 0x7;
				if (reg->getSREG(T)){
					//Set bit B in register
					reg->ram[Rd] = reg->ram[Rd] | BIT(B);
				}else{
					//Unset bit B in register
					reg->ram[Rd] = reg->ram[Rd] & ~(BIT(B));
				}
                this->cCount = 1;
				reg->pc++;
				res = "bld";
			}else if ((inst & 0xfc00) == BRBC){ //Equivelent instruction BRNE
				//Branch if clear
				R = inst & 0x7; 
				jmp = this->getK7Bit(inst);
				if (reg->getSREG(R) == 0){
					reg->pc += jmp + 1;
                    this->cCount = 2;
				}else{	
					reg->pc += 1;
                    this->cCount = 1;
				}
				res = "brbc";
			}else if ((inst & 0xfc00) == BRBS){
				//Branch if set
				R = inst & 0x7;
				jmp = this->getK7Bit(inst);
                if (reg->getSREG(R)){
					reg->pc += jmp + 1;
                    this->cCount = 2;
				}else{
					reg->pc += 1;
                    this->cCount = 1;
				}
				res = "brbs";
			}else{
				switch (inst & 0xfe08){
					case BST:
						Rd = GET_REGISTER_5_BIT_D;
						B = inst & 0x7;
						reg->setSREG(T, reg->ram[Rd] & BIT(B));
						reg->pc++;
                        this->cCount = 1;
						res = "bst";
					break;
					case SBRC:
						Rd = GET_REGISTER_5_BIT_D;
						B = inst & 0x7;
                        this->cCount = 1;
                        if (((~reg->ram[Rd]) & BIT(B))){
                            //Bit in Register Not Set Skip Instruction
							reg->pc++;
                            this->cCount = 2;
                            if (isTwoWord(this->flash->getFlash()[reg->pc])){
								reg->pc++;
                                this->cCount = 3;
							}
						}
						reg->pc++;
						res = "sbrc";
					break;
					case SBRS:
                        Rd = GET_REGISTER_5_BIT_D;
						B = inst & 0x7;
                        this->cCount = 1;
						if (reg->ram[Rd] & BIT(B)){
							//Bit in Register Set Skip Instruction
							reg->pc++;
                            this->cCount = 2;
                            if (isTwoWord(this->flash->getFlash()[reg->pc])){
								reg->pc++;
                                this->cCount = 3;
							}
						}
						reg->pc++;
						res = "sbrs";
					break;
				}
				
			}
		break;

		default:
			//Bad Instruction
            exit(99);
		break;
	}
	
	return res;
}
