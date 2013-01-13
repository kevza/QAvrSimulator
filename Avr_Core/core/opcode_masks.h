/***
 * @brief Opcode bit masks for detection instructions
 * @author Kevin Luke kevzawinning@gmail.com
 * @date 12/11/2012
 */
#ifndef OPCODE_MASK_H
#define OPCODE_MASK_H

//0000
#define NOP 0x0

//Begin 0xfc00 mask
//LSL Also
#define ADD 0x0c00

#define CPC 0x0400
#define SBC 0x0800

//Begin mask 0xff88
#define FMUL 0x0308
#define FMULS 0x0380
#define FMULSU 0x0388
#define MULSU 0x0300

//Begin mask 0xff00
#define MOVW 0x0100
#define MULS 0x0200


//1000
//Mask 0xfc00
#define CPSE 0x1000
#define CP 0x1400
#define SUB 0x1800
#define ADC 0x1c00
#define ROL 0x1c00

//2000
//Mask 0xfc00
#define AND 0x2000
#define TST 0x2000

#define CLR 0x2400
#define EOR 0x2400
#define MOV 0x2c00
#define OR 0x2800


//3000
#define CPI 0x3000

//4000
#define SBCI 0x4000

//5000
#define SUBI 0x5000

//6000
#define ORI 0x6000

//7000
#define ANDI 0x7000

//8000
//Mask 0x0208 //Also used in 0xa000
#define LDYQ 0x0008
#define LDZQ 0x0000
#define STYQ 0x0208


//9000
//Two Word Instructions 0xfe0e Start
#define JMP 0x940c 
#define CALL 0x940e



//One word instructions 0xff00 Start
#define ADIW 0x9600

#define CBI 0x9800

#define SBI 0x9a00

#define SBIC 0x9900

#define SBIS 0x9b00

#define SBIW 0x9700
//End 0xff00 Mask

//Start 0xfe0f Mask
#define ASR 0x9405

#define DEC 0x940a

#define ELPMII 0x9006

#define ELPMIII 0x9007

#define INC 0x9403

#define LAC 0x9206

#define LAS 0x9205

#define LAT 0x9207

#define LD1 0x900c

#define LD2 0x900d

#define LD3 0x900e

#define LDS 0x9000

#define LPM2 0x9004

#define LPM3 0x9005

#define LSR 0x9406

#define NEG 0x9401

#define POP 0x900f

#define PUSH 0x920f

#define ROR 0x9407 

#define STS 0x9800 


#define ST 0x920c
#define ST1 0x920d
#define ST2 0x920e

#define STZ1 0x9201
#define STZ2 0x9202


#define STSL 0x9200

#define SWAP 0x9402

#define XCH 0x9204


#define LDYP 0x9009
#define LDYM 0x900a
#define LDZP 0x9001
#define LDZM 0x9002

#define STYP 0x9209
#define STYM 0x920a

//End 0xfe0f Mask

//Start oxff8f Mask
#define BCLR (inst & 0xff8f) == 0x9488

#define BSET (inst & 0xff8f) ==  0x9408
//End 0xff8f Mask

//Start 0xffff Mask
#define BREAK 0x9598


//Group These
//SRED Address of flag in 2nd byte (-8)
#define CLC 0x9488
#define CLZ 0x9498
#define CLN 0x94a8
#define CLV 0x94b8
#define CLS 0x94c8
#define CLH 0x94d8
#define CLT 0x94e8
#define CLI 0x94f8

//Group these commands
//SREG Address of flag in 2nd byte CZNVSHTI
#define SEC 0x9408
#define SEZ 0x9418
#define SEN 0x9428
#define SEV 0x9438
#define SES 0x9448
#define SEH 0x9458
#define SET 0x9468
#define SEI 0x9478

#define EICAL 0x9519

#define EIJMP 0x9419

#define ICALL 0x9509

#define ELPMI 0x95d8

#define IJMP 0x9409

#define LPM 0x95c8

#define RET 0x9508

#define RETI 0x9518

#define SLEEP 0x9588


//There is some funny business going on here that we need to sort out
#define SPM 0x95e8

#define SPM2 0x95f8

#define WDR 0x95a8 


//end 0xffff Mask

#define COM (inst & 0xfe0f) == 0x9400

#define DES (inst & 0xff0f) == 0x940b 

#define MUL (inst & 0xfc00) == 0x9c00

//a000 and 8000
//mask 0xd208
#define STZ3 0x8200

//a000
//mask 0xf800

#define STS1 0xa800 
#define LDS1 0xa000

//b0000
//mask 0xf800
#define IN 0xb000
#define OUT 0xb800

//c0000
#define RJMP 0xc000

//d000
#define RCALL 0xd000

//e000
#define LDI 0xe000

//f0000
//mask 0xfe08
#define BLD 0xf800


//mask 0xfc00
#define BRBC 0xf400
#define BRBS 0xf000

//mask 0xfc07
#define BRCC 0xf400
#define BRNE 0xf401
#define BRPL 0xf402
#define BRVC 0xf403
#define BRGE 0xf404
#define BRHC 0xf405
// repeated instruction BRSH 0xf400
#define BRTC 0xf406
#define BRID 0xf407

#define BRLO 0xf000
#define BRCS 0xf000
#define BREQ 0xf001
#define BRMI 0xf002
#define BRVS 0xf003
#define BRLT 0xf004
#define BRTS 0xf006
#define BRIE 0xf007

//mask 0xfe08
#define BST 0xfa00
#define SBRC 0xfc00
#define SBRS 0xfe00







#define BRHS 0xf005


#endif //OPCODE_MASK_H
