/**
 * @brief Runs tests on the AVR simulated core for 
 * opcodes
 */
#include "gtest/gtest.h"
#include "core/core.h"
#include "core/flash.h"
#include <stdint.h>
#include <iostream>
enum {C,Z,N,V,S,H,T,I};

namespace {
	class OpCodeTest : public ::testing::Test {
		protected:
			virtual void SetUp() {
				f =  new Avr_Flash(32000);
				m = new Avr_Memory();
				reg = new Avr_Registers;
				m->initRam(1024);
				ram = m->getRam();
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
	};
	
TEST_F(OpCodeTest, TestAddInstruction){
		//Test 1
		ram[0] = 5;
		ram[1] = 5;
		f->getFlash()[0] = 0x0c10;
		EXPECT_EQ("add",core->decodeInstruction());
		//Check Rr
		EXPECT_EQ(5,ram[0]);
		//Check Rd
		EXPECT_EQ(10,ram[1]);
		EXPECT_EQ(0, reg->getSREG(H));
		EXPECT_EQ(0, reg->getSREG(S));
		EXPECT_EQ(0, reg->getSREG(V));
		EXPECT_EQ(0, reg->getSREG(N));
		EXPECT_EQ(0, reg->getSREG(Z));
		EXPECT_EQ(0, reg->getSREG(C));
		
		
		
		//Test 2
		ram[10] = 10;
		ram[15] = 15;
		f->getFlash()[1] = 0x0caf;
		EXPECT_EQ("add",core->decodeInstruction());
		//Check Rr
		EXPECT_EQ(25,ram[10]);
		//Check Rd
		EXPECT_EQ(15,ram[15]);
		EXPECT_EQ(1, reg->getSREG(H));
		EXPECT_EQ(0, reg->getSREG(S));
		EXPECT_EQ(0, reg->getSREG(V));
		EXPECT_EQ(0, reg->getSREG(N));
		EXPECT_EQ(0, reg->getSREG(Z));
		EXPECT_EQ(0, reg->getSREG(C));
		
		//Test 3
		ram[10] = 0xff;
		ram[15] = 0x1;
		f->getFlash()[2] = 0x0caf;
		EXPECT_EQ("add",core->decodeInstruction());
		//Check Rr
		EXPECT_EQ(0,ram[10]);
		//Check Rd
		EXPECT_EQ(0x1,ram[15]);
		EXPECT_EQ(1, reg->getSREG(H));
		EXPECT_EQ(0, reg->getSREG(S));
		EXPECT_EQ(0, reg->getSREG(V));
		EXPECT_EQ(0, reg->getSREG(N));
		EXPECT_EQ(1, reg->getSREG(Z));
		EXPECT_EQ(1, reg->getSREG(C));
		
		//Test 4
		ram[10] = 127;
		ram[15] = 127;
		f->getFlash()[3] = 0x0caf;
		EXPECT_EQ("add",core->decodeInstruction());
		//Check Rr
		EXPECT_EQ(254,ram[10]);
		//Check Rd
		EXPECT_EQ(127,ram[15]);
		EXPECT_EQ(1, reg->getSREG(H));
		EXPECT_EQ(0, reg->getSREG(S));
		EXPECT_EQ(1, reg->getSREG(V));
		EXPECT_EQ(1, reg->getSREG(N));
		EXPECT_EQ(0, reg->getSREG(Z));
		EXPECT_EQ(0, reg->getSREG(C));
		
		
	}
	
	TEST_F(OpCodeTest, TestCPCInstruction){
		//Test 1
		reg->setSREG(C,0);
		//Set Op Code
		f->getFlash()[0] = 0x0401;
		ram[0] = 10;
		ram[1] = 10;
		EXPECT_EQ("cpc", core->decodeInstruction());
		//Check Flags
		EXPECT_EQ(0, reg->getSREG(H));
		EXPECT_EQ(0, reg->getSREG(S));
		EXPECT_EQ(0, reg->getSREG(V));
		EXPECT_EQ(0, reg->getSREG(N));
		EXPECT_EQ(0, reg->getSREG(Z));
		EXPECT_EQ(0, reg->getSREG(C));
	
		//Test 2
		reg->setSREG(C,1);
		//Set Op Code
		f->getFlash()[1] = 0x0401;
		ram[0] = 10;
		ram[1] = 10;
		EXPECT_EQ("cpc", core->decodeInstruction());
		//Check Flags
		EXPECT_EQ(1, reg->getSREG(H));
		EXPECT_EQ(1, reg->getSREG(S));
		EXPECT_EQ(0, reg->getSREG(V));
		EXPECT_EQ(1, reg->getSREG(N));
		EXPECT_EQ(0, reg->getSREG(Z));
		EXPECT_EQ(1, reg->getSREG(C));
	
	}
	
	TEST_F(OpCodeTest, TestFMULSInstruction){
		ram[16] = 0xff;
		ram[17] = 0xff;
		f->getFlash()[0] = 0x390;
		f->getFlash()[1] = 0x390;
		//Test 1
		ram[16] = 0xff;
		ram[17] = 0xff;
		EXPECT_EQ("fmuls",core->decodeInstruction());
		EXPECT_EQ(0x00,ram[1]);
		EXPECT_EQ(0x02,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));
		
		//Test 2
		ram[16] = 0x80;
		ram[17] = 0x80;
		EXPECT_EQ("fmuls",core->decodeInstruction());
		EXPECT_EQ(0x80,ram[1]);
		EXPECT_EQ(0x00,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));
	}
	
		TEST_F(OpCodeTest, TestFMULSUInstruction){

		ram[16] = 0xff;
		ram[17] = 0xff;
		f->getFlash()[0] = 0x398;
		f->getFlash()[1] = 0x398;
		//Test 1
		ram[16] = 0xff;
		ram[17] = 0xff;
		EXPECT_EQ("fmulsu",core->decodeInstruction());
		EXPECT_EQ(0xfe,ram[1]);
		EXPECT_EQ(0x02,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));
		
		//Test 2
		ram[16] = 0x80;
		ram[17] = 0x80;
		EXPECT_EQ("fmulsu",core->decodeInstruction());
		EXPECT_EQ(0x80,ram[1]);
		EXPECT_EQ(0x00,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));
	}
	
	//Muls test
	TEST_F(OpCodeTest, TestMULSInstruction){
		ram[16] = 0xff;
		ram[17] = 0xff;
		
		f->getFlash()[0] = 0x210;
		f->getFlash()[1] = 0x210;
		//Test 1
		ram[16] = 0xff;
		ram[17] = 0xff;
		EXPECT_EQ("muls",core->decodeInstruction());
		EXPECT_EQ(0x00,ram[1]);
		EXPECT_EQ(0x01,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));
		
		//Test 2
		ram[16] = 0x80;
		ram[17] = 0x80;
		EXPECT_EQ("muls",core->decodeInstruction());
		EXPECT_EQ(0x40,ram[1]);
		EXPECT_EQ(0x00,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));
	}
	
	//Mulus test
	TEST_F(OpCodeTest, TestMULSUInstruction){
		ram[16] = 0xff;
		ram[17] = 0xff;
		
		f->getFlash()[0] = 0x310;
		f->getFlash()[1] = 0x310;
		//Test 1
		ram[16] = 0xff;
		ram[17] = 0xff;
		EXPECT_EQ("mulsu",core->decodeInstruction());
		EXPECT_EQ(0xff,ram[1]);
		EXPECT_EQ(0x01,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));
		
		//Test 2
		ram[16] = 0x80;
		ram[17] = 0x80;
		EXPECT_EQ("mulsu",core->decodeInstruction());
		EXPECT_EQ(0xc0,ram[1]);
		EXPECT_EQ(0x00,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));
	}
	
	//FMul test
	TEST_F(OpCodeTest, TestFMULInstruction){
		ram[16] = 0xff;
		ram[17] = 0xff;
		
		f->getFlash()[0] = 0x318;
		f->getFlash()[1] = 0x318;
		//Test 1
		ram[16] = 0xff;
		ram[17] = 0xff;
		EXPECT_EQ("fmul",core->decodeInstruction());
		EXPECT_EQ(0xfc,ram[1]);
		EXPECT_EQ(0x02,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));
		
		//Test 2
		ram[16] = 0x80;
		ram[17] = 0x80;
		EXPECT_EQ("fmul",core->decodeInstruction());
		EXPECT_EQ(0x80,ram[1]);
		EXPECT_EQ(0x00,ram[0]);
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));
	}
	
	TEST_F(OpCodeTest, TestSBCInstruction){
		ram[0] = 0x1; // Rr
		ram[1] = 0xff;	//Rd
		//Setup Instruction
		f->getFlash()[0] = 0x810;
		f->getFlash()[1] = 0x810;
		reg->setSREG(C, 1);
		//Result tests
		EXPECT_EQ("sbc",core->decodeInstruction());
		EXPECT_EQ(0xfd,ram[1]);
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 0, reg->getSREG(C));
	
	
		ram[0] = 0x1;
		ram[1] = 0xff;
		reg->setSREG(C, 0);
		EXPECT_EQ("sbc",core->decodeInstruction());
		EXPECT_EQ(0xfe,ram[1]);
		//Flag Tests
		EXPECT_EQ( 0, reg->getSREG(H));
		EXPECT_EQ( 0, reg->getSREG(V));
		EXPECT_EQ( 1, reg->getSREG(N));
		EXPECT_EQ( 0, reg->getSREG(Z));
		EXPECT_EQ( 0, reg->getSREG(C));
	}
	
		TEST_F(OpCodeTest, TestMOVWInstruction){
		ram[0] = 0x1; // Rr
		ram[1] = 0xff;	//Rd
		//Setup Instruction
		f->getFlash()[0] = 0x120;
		//Result tests
		EXPECT_EQ("movw",core->decodeInstruction());
		EXPECT_EQ(0x1,ram[4]);
		EXPECT_EQ(0xff,ram[5]);
	}
	
};
