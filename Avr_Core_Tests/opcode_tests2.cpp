/**
 * @brief Runs tests on the AVR simulated core for 
 * opcodes (0x1000 section)
 */
#include "gtest/gtest.h"
#include "core/core.h"
#include "core/flash.h"
#include <stdint.h>
#include <iostream>

enum {C,Z,N,V,S,H,T,I};

namespace {
	class OpCodeTest2 : public ::testing::Test {
		protected:
			virtual void SetUp() {
				f =  new Avr_Flash(32000);
				m = new Avr_Memory();
				reg = new Avr_Registers;
				m->initRam(1024);
				ram = m->getRam();
				io = ram + 0x20;
				reg->setSREGP(ram + 0x5f);
				core = new Avr_Core(f,m,reg);
			}
			
			virtual void TearDown() {
				delete core;
				delete reg;
				delete f;
				delete m;
			}
			Avr_Registers* reg;
			Avr_Core* core;
			Avr_Flash* f;	
			Avr_Memory* m;
			uint8_t *ram; 
			uint8_t *io;
	};
	
	TEST_F(OpCodeTest2, TestCPSEInstruction){
		ram[0] = 0x1;
		ram[1] = 0x1;
		//Args for 0x390 fmuls opcode
		ram[16] = 0xff;
		ram[17] = 0xff;
		
		f->getFlash()[0] = 0x1010;
		f->getFlash()[1] = 0x390;
		f->getFlash()[2] = 0x1010;
		f->getFlash()[3] = 0x390;
		
		EXPECT_EQ("cpse",core->decodeInstruction());
		ram[1] = 0x2;
		EXPECT_EQ("cpse",core->decodeInstruction());
		EXPECT_EQ("fmuls",core->decodeInstruction());
	}
	
	TEST_F(OpCodeTest2,TestCPInstruction){
		ram[0] = 0x1;
		ram[1] = 0xff;
	
		f->getFlash()[0] = 0x1410;
		f->getFlash()[1] = 0x1410;
		//Test 1
		EXPECT_EQ("cp",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 0, reg->getSREG(C));
		
		//Test 2
		ram[0] = 0xff;
		ram[1] = 0x1;
		EXPECT_EQ("cp",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ( 1, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 1, reg->getSREG(C));
	
	}
	
		TEST_F(OpCodeTest2,TestSUBInstruction){
		ram[0] = 0x1;
		ram[1] = 0xff;
	
		f->getFlash()[0] = 0x1810;
		f->getFlash()[1] = 0x1810;
		//Test 1
		EXPECT_EQ("sub",core->decodeInstruction());
		EXPECT_EQ(0xfe,ram[1]);
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 0, reg->getSREG(C));
		
		//Test 2
		ram[0] = 0xff;
		ram[1] = 0x1;
		EXPECT_EQ("sub",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0x2,ram[1]);
		EXPECT_EQ( 1, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 1, reg->getSREG(C));
	
	}
	
		TEST_F(OpCodeTest2, TestADCInstruction){
		ram[0] = 0x1; // Rr
		ram[1] = 0xff;	//Rd
		//Setup Instruction
		f->getFlash()[0] = 0x1c10;
		f->getFlash()[1] = 0x1c10;
		reg->setSREG(C, 1);
		//Result tests
		EXPECT_EQ("adc",core->decodeInstruction());
		EXPECT_EQ(0x1,ram[1]);
		//Flag Tests
		EXPECT_EQ( 1, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 1, reg->getSREG(C));
	
	
		ram[0] = 0x1;
		ram[1] = 0xff;
		reg->setSREG(C,0);
		EXPECT_EQ("adc",core->decodeInstruction());
		EXPECT_EQ(0x0,ram[1]);
		//Flag Tests
		EXPECT_EQ( 1, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 1, reg->getSREG(Z));
		EXPECT_EQ( 1, reg->getSREG(C));
	}
	
	TEST_F(OpCodeTest2, TestCPIInstruction){
		ram[16] = 0xff; // Rd K = 0x1
		//Setup Instruction
		f->getFlash()[0] = 0x3001;
		f->getFlash()[1] = 0x3f0f;
		//Result tests
		EXPECT_EQ("cpi",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 0, reg->getSREG(C));
	
	
		ram[16] = 0x1; //Rd \\ K = 0xff
		reg->setSREG(C,0);
		EXPECT_EQ("cpi",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ( 1, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 1, reg->getSREG(C));
	}
	
	TEST_F(OpCodeTest2, TestANDInstruction){
		ram[0] = 0xff; // Rr
		ram[1] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x2010;
		f->getFlash()[1] = 0x2010;
		//Result tests
		EXPECT_EQ("and",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0x01,ram[1]);
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
	

		ram[1] = 0xf0;

		EXPECT_EQ("and",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0xf0,ram[1]);
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
	}
	
	TEST_F(OpCodeTest2, TestEORInstruction){
		ram[0] = 0xff; // Rr
		ram[1] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x2410;
		f->getFlash()[1] = 0x2410;
		//Result tests
		EXPECT_EQ("eor",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0xfe,ram[1]);
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
	

		ram[1] = 0xff;

		EXPECT_EQ("eor",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0x0,ram[1]);
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 1, reg->getSREG(Z));
	}
		
	TEST_F(OpCodeTest2, TestMOVInstruction){
		ram[0] = 0xff; // Rr
		ram[1] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x2c10;
		f->getFlash()[1] = 0x2c10;
		//Result tests
		EXPECT_EQ("mov",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0xff,ram[1]);

		ram[0] = 0xf0;

		EXPECT_EQ("mov",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0xf0,ram[1]);
	}
	
	TEST_F(OpCodeTest2, TestORInstruction){
		ram[0] = 0xff; // Rr
		ram[1] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x2810;
		f->getFlash()[1] = 0x2810;
		//Result tests
		EXPECT_EQ("or",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0xff,ram[1]);
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
	

		ram[1] = 0xff;

		EXPECT_EQ("or",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0xff,ram[1]);
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
	}
	
		TEST_F(OpCodeTest2, TestSBCIInstruction){
		ram[16] = 0xff; // Rr
		ram[17] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x4001;
		f->getFlash()[1] = 0x4f1f;
		reg->setSREG(C,1);
		//Result tests
		EXPECT_EQ("sbci",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0,reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 0, reg->getSREG(C));
		
		reg->setSREG(C, 0);
		EXPECT_EQ("sbci",core->decodeInstruction());
		
		//Flag Tests
		EXPECT_EQ(1,reg->getSREG(H));
		EXPECT_EQ(0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 1, reg->getSREG(C));
	}
	
	TEST_F(OpCodeTest2, TestSUBIInstruction){
		ram[16] = 0xff; // Rr
		ram[17] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x5001;
		f->getFlash()[1] = 0x5f1f;
		//Result tests
		EXPECT_EQ("subi",core->decodeInstruction());
		//Flag Tests
		EXPECT_EQ(0,reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 0, reg->getSREG(C));
		
		EXPECT_EQ("subi",core->decodeInstruction());
		
		//Flag Tests
		EXPECT_EQ(1,reg->getSREG(H));
		EXPECT_EQ(0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 1, reg->getSREG(C));
	}
	
	TEST_F(OpCodeTest2, TestORIInstruction){
		ram[16] = 0xff; // Rr
		ram[17] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x6001;
		f->getFlash()[1] = 0x6f1f;
		//Result tests
		EXPECT_EQ("ori",core->decodeInstruction());
		EXPECT_EQ( 0xff, this->ram[16]);
		
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		
		EXPECT_EQ("ori",core->decodeInstruction());
		EXPECT_EQ( 0xff, this->ram[17]);
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		
	}
	
	TEST_F(OpCodeTest2, TestANDIInstruction){
		ram[16] = 0xff; // Rr
		ram[17] = 0x01; //Rd		
		//Setup Instruction
		f->getFlash()[0] = 0x7001;
		f->getFlash()[1] = 0x7f1f;
		//Result tests
		EXPECT_EQ("andi",core->decodeInstruction());
		EXPECT_EQ( 0x1, this->ram[16]);
		
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		
		EXPECT_EQ("andi",core->decodeInstruction());
		EXPECT_EQ( 0x01, this->ram[17]);
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 0, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		
	}
	
	TEST_F(OpCodeTest2, TestLDIInstruction){	
		//Setup Instruction
		f->getFlash()[0] = 0xef0f;
		f->getFlash()[1] = 0xe414;
		//Result tests
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ( 0xff, this->ram[16]);
		
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ( 0x44, this->ram[17]);		
	}
	
	TEST_F(OpCodeTest2, TestRJMPInstruction){	
		//Setup Instruction
		f->getFlash()[0] = 0xc007;
		f->getFlash()[8] = 0xcff7;
		//Result tests
		EXPECT_EQ("rjmp",core->decodeInstruction());
		EXPECT_EQ("rjmp",core->decodeInstruction());
		EXPECT_EQ("rjmp",core->decodeInstruction());	
	}
	
	TEST_F(OpCodeTest2, TestINInstruction){	
		//Setup Instruction
		io[0] = 0xff;
		io[1] = 0x44;
		
		f->getFlash()[0] = 0xb001;
		f->getFlash()[1] = 0xb010;
		//Result tests
		EXPECT_EQ("in",core->decodeInstruction());
		EXPECT_EQ(0x44,ram[0]);			
		EXPECT_EQ("in",core->decodeInstruction());
		EXPECT_EQ(0xff,ram[1]);
		
	}	
	
	TEST_F(OpCodeTest2, TestOUTInstruction){	
		//Setup Instruction
		ram[0] = 0xff;
		ram[1] = 0x44;
		
		f->getFlash()[0] = 0xb801;
		f->getFlash()[1] = 0xb810;
		//Result tests
		EXPECT_EQ("out",core->decodeInstruction());
		EXPECT_EQ(0xff,io[1]);
		EXPECT_EQ("out",core->decodeInstruction());
		EXPECT_EQ(0x44,io[0]);	
	}
	
}
