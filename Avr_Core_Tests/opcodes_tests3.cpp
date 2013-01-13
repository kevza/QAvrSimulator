/**
 * @brief Runs tests on the AVR simulated core for 
 * opcodes (0x1001 section)
 */
#include "gtest/gtest.h"
#include "core/core.h"
#include "core/flash.h"
#include <stdint.h>
#include <iostream>
#include <time.h>

enum {C,Z,N,V,S,H,T,I};


namespace {
	class OpCodeTest3 : public ::testing::Test {
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

	
	//Clear Flags Tests
	TEST_F(OpCodeTest3, TestBCLRInstruction){
		//Set the SREG
		for (int i = 0; i < 8; i++){
			reg->setSREG(i,1);
			f->getFlash()[i] = 0x9488 | (i << 4); 
		}
		//Unset the SREG for by opcode
		for (int i = 0; i < 8 ; i++){
			EXPECT_EQ("bclr",core->decodeInstruction());
			EXPECT_EQ(0,reg->getSREG(i));
		}
	
	}
	//Set Flags Tests
	TEST_F(OpCodeTest3, TestBSETInstruction){
		//Unset the SREG
		for (int i = 0; i < 8; i++){
			reg->setSREG(i,0);
			f->getFlash()[i] = 0x9408 | (i << 4); 
		}
		//Set the SREG for by opcode
		for (int i = 0; i < 8 ; i++){
			EXPECT_EQ("bset",core->decodeInstruction());
			EXPECT_EQ(1,reg->getSREG(i));
		}
	
	}
	
	//Test ST 
	TEST_F(OpCodeTest3, TestSTInstruction){
		f->loadHex("TestHex/st_test.hex");
		//Run Load Data Instructions
		for (int i = 0 ; i < 6;i++){
			EXPECT_EQ("ldi", core->decodeInstruction());
		}
		//Load r16 (1) to (X = 0x76) 
		EXPECT_EQ("st",core->decodeInstruction());
		EXPECT_EQ(1,ram[0x276]);
		//Load r17 (2) to (X = 0x75) --0x76
		EXPECT_EQ("st",core->decodeInstruction());
		EXPECT_EQ(2,ram[0x275]);
		//Load r18 (3) to (X = 0x75) 0x75++
		EXPECT_EQ("st",core->decodeInstruction());
		EXPECT_EQ(3,ram[0x275]);
		
		//Load r19 (4) to X = 0x76
		EXPECT_EQ("st",core->decodeInstruction());
		EXPECT_EQ(4,ram[0x276]);
	}
	
	TEST_F(OpCodeTest3, TestSTSLInstruction){
		f->loadHex("TestHex/sts_test.hex");
		//Run Load Data Instructions
		for (int i = 0 ; i < 3;i++){
			EXPECT_EQ("ldi", core->decodeInstruction());
		}
		EXPECT_EQ("stsl",core->decodeInstruction());
		EXPECT_EQ(1,ram[512]);
		EXPECT_EQ("stsl",core->decodeInstruction());
		EXPECT_EQ(2,ram[513]);
		EXPECT_EQ("stsl",core->decodeInstruction());
		EXPECT_EQ(3,ram[514]);
	}
	
	TEST_F(OpCodeTest3, TestSWAPLInstruction){
		f->loadHex("TestHex/swap_test.hex");
		//Run Load Data Instructions
		for (int i = 0 ; i < 2;i++){
			EXPECT_EQ("ldi", core->decodeInstruction());
		}
		EXPECT_EQ("swap",core->decodeInstruction());
		EXPECT_EQ(0x21,ram[16]);
		EXPECT_EQ("swap",core->decodeInstruction());
		EXPECT_EQ(0x4,ram[17]);
	}

	TEST_F(OpCodeTest3,TestPOP_PUSHInstruction){
		f->loadHex("TestHex/pop_test.hex");
		//Load Setup Count to Register r31
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[31];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ(0xff, this->ram[0x3d + 32]);
		EXPECT_EQ(0x3, this->ram[0x3e + 32]);
		
		EXPECT_EQ("push",core->decodeInstruction());
		
		EXPECT_EQ("push",core->decodeInstruction());
		
		EXPECT_EQ("pop",core->decodeInstruction());
		
		EXPECT_EQ(2,this->ram[17]);
		
		EXPECT_EQ("pop",core->decodeInstruction());
		
		EXPECT_EQ(1, this->ram[18]);
		
	}
	
	TEST_F(OpCodeTest3,TestLPMInstruction){
		f->loadHex("TestHex/lpm_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_EQ("ldi",core->decodeInstruction());
		}
		//Set Constants in flash
		f->getFlash()[1022] = 10;
		f->getFlash()[1023] = 8;
		
		//EXPECT_EQ(1022,this->core->getZ());
		EXPECT_EQ("lpm",core->decodeInstruction());
		EXPECT_EQ(10, this->ram[0]);
		
		EXPECT_EQ("lpm",core->decodeInstruction());
		EXPECT_EQ(10, this->ram[1]);
		
		EXPECT_EQ("lpm+",core->decodeInstruction());
		EXPECT_EQ(10, this->ram[2]);
		//EXPECT_EQ(1023,this->core->getZ());
		
		EXPECT_EQ("lpm",core->decodeInstruction());
		EXPECT_EQ(8, this->ram[0]);
	}
	
	TEST_F(OpCodeTest3,TestLDSInstruction){
		f->loadHex("TestHex/lds_test.hex");
		
		//Load Setup Count to Register r31
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ("lds",core->decodeInstruction());
		EXPECT_EQ(0x12,this->ram[0]);
	}
	
	TEST_F(OpCodeTest3,TestLDInstruction){
		f->loadHex("TestHex/ld_test.hex");
		
		//Load Setup Count to Register r31
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ("ld",core->decodeInstruction());
		EXPECT_EQ(0x12,this->ram[0]);
		EXPECT_EQ("ld",core->decodeInstruction());
		EXPECT_EQ(0x13,this->ram[1]);
		EXPECT_EQ("ld",core->decodeInstruction());
		EXPECT_EQ(0x13,this->ram[2]);
		EXPECT_EQ("ld",core->decodeInstruction());
		EXPECT_EQ(0x12,this->ram[3]);
	}
	
	TEST_F(OpCodeTest3,TestRETInstruction){
		f->loadHex("TestHex/ret_test.hex");
		//Jmp to Main
		EXPECT_EQ("jmp",core->decodeInstruction());
		
		//Setup sp
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("out",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("out",core->decodeInstruction());
		
		EXPECT_EQ("call",core->decodeInstruction());
	
		EXPECT_EQ("push",core->decodeInstruction());
		EXPECT_EQ("pop",core->decodeInstruction());
		EXPECT_EQ("ret",core->decodeInstruction());
		
		EXPECT_EQ("ldi",core->decodeInstruction());
	}
	
	TEST_F(OpCodeTest3,TestRCALLInstruction){
		f->loadHex("TestHex/rcall_test.hex");
		//Jmp to Main
		EXPECT_EQ("jmp",core->decodeInstruction());
		
		//Setup sp
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("out",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("out",core->decodeInstruction());
		
		EXPECT_EQ("rcall",core->decodeInstruction());
	
		EXPECT_EQ("push",core->decodeInstruction());
		EXPECT_EQ("pop",core->decodeInstruction());
		EXPECT_EQ("ret",core->decodeInstruction());
		
		EXPECT_EQ("ldi",core->decodeInstruction());
	}
	
	TEST_F(OpCodeTest3,TestSBIInstruction){
		f->loadHex("TestHex/sbi_test.hex");
		EXPECT_EQ("sbi", core->decodeInstruction());
		EXPECT_EQ(16,this->io[0]);	
		
	}	
	TEST_F(OpCodeTest3,TestSBISInstruction){
		f->loadHex("TestHex/sbis_test.hex");
		EXPECT_EQ("sbi", core->decodeInstruction());
		EXPECT_EQ("sbis", core->decodeInstruction());
		EXPECT_EQ("sbis", core->decodeInstruction());
		EXPECT_EQ("ldi", core->decodeInstruction());
		EXPECT_EQ("jmp", core->decodeInstruction());
		
	}	
	
	TEST_F(OpCodeTest3,TestSBICInstruction){
		f->loadHex("TestHex/sbic_test.hex");
		EXPECT_EQ("sbi", core->decodeInstruction());
		EXPECT_EQ("sbic", core->decodeInstruction());
		EXPECT_EQ("ldi", core->decodeInstruction());
		EXPECT_EQ("sbic", core->decodeInstruction());
		EXPECT_EQ("jmp", core->decodeInstruction());
		
	}
	
		
	TEST_F(OpCodeTest3,TestSTZInstruction){
		f->loadHex("TestHex/stz_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("stz",core->decodeInstruction());
		EXPECT_EQ(1 ,this->ram[60]);
		
		EXPECT_EQ("stz",core->decodeInstruction());
		EXPECT_EQ(2 ,this->ram[61]);

		EXPECT_EQ("ldi",core->decodeInstruction());

		EXPECT_EQ("stz",core->decodeInstruction());
		EXPECT_EQ(3 ,this->ram[63]);
		
		EXPECT_EQ("stz",core->decodeInstruction());		
		EXPECT_EQ(4 ,this->ram[62]);
		
		EXPECT_EQ("stz",core->decodeInstruction());
		EXPECT_EQ(5 ,this->ram[64]);

	}	
	TEST_F(OpCodeTest3,TestEIJMPInstruction){
		f->loadHex("TestHex/eijmp_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ("eijmp",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
	}
	
	TEST_F(OpCodeTest3,TestIJMPInstruction){
		f->loadHex("TestHex/ijmp_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ("ijmp",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
	}
	
	TEST_F(OpCodeTest3,TestEICALLInstruction){
		f->loadHex("TestHex/eicall_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ("eical",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("ret",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
	}
	
	TEST_F(OpCodeTest3,TestICALLInstruction){
		f->loadHex("TestHex/icall_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ("icall",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("ret",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
	}
	
	TEST_F(OpCodeTest3,TestCBIInstruction){
		f->loadHex("TestHex/cbi_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		EXPECT_EQ("cbi",core->decodeInstruction());
		EXPECT_EQ(0x7,this->io[0]);

	}
	
	TEST_F(OpCodeTest3,TestADIWInstruction){
		f->loadHex("TestHex/adiw_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("adiw",core->decodeInstruction());
		EXPECT_EQ(0x0,this->ram[30]);
		EXPECT_EQ(0x0,this->ram[31]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(1,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));
	}
	
	TEST_F(OpCodeTest3,TestASRInstruction){
		f->loadHex("TestHex/asr_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("asr",core->decodeInstruction());
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));	
		
				
		EXPECT_EQ("asr",core->decodeInstruction());
		EXPECT_EQ(1,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));		
		
		EXPECT_EQ(0x8,this->ram[16]);
		EXPECT_EQ(-0x2,(int8_t)this->ram[17]);
	}	
	
	TEST_F(OpCodeTest3,TestCOMInstruction){
		f->loadHex("TestHex/com_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("com",core->decodeInstruction());
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));	
		
				
		EXPECT_EQ("com",core->decodeInstruction());
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(1,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(C));		
		
		EXPECT_EQ(0xfa,this->ram[16]);
		EXPECT_EQ(0x00,(int8_t)this->ram[17]);
	}	

	TEST_F(OpCodeTest3,TestDECInstruction){
		f->loadHex("TestHex/dec_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("dec",core->decodeInstruction());
		EXPECT_EQ(0x7f, this->ram[16]);
		EXPECT_EQ(1,reg->getSREG(V));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));

		
				
		EXPECT_EQ("dec",core->decodeInstruction());
		EXPECT_EQ(0x0, this->ram[17]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(1,reg->getSREG(Z));
	
		EXPECT_EQ(0x00,(int8_t)this->ram[17]);
	}	
	
	TEST_F(OpCodeTest3,TestINCInstruction){
		f->loadHex("TestHex/inc_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("inc",core->decodeInstruction());
		EXPECT_EQ(0x80, this->ram[16]);
		EXPECT_EQ(1,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));

		
				
		EXPECT_EQ("inc",core->decodeInstruction());
		EXPECT_EQ(0x2, this->ram[17]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
	}	
	
	TEST_F(OpCodeTest3,TestLSRInstruction){
		f->loadHex("TestHex/lsr_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("lsr",core->decodeInstruction());
		EXPECT_EQ(0x0, this->ram[16]);
		EXPECT_EQ(1,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(C));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(1,reg->getSREG(Z));

		
				
		EXPECT_EQ("lsr",core->decodeInstruction());
		EXPECT_EQ(0x8, this->ram[17]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(0,reg->getSREG(C));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
	}	
	
	TEST_F(OpCodeTest3,TestNEGInstruction){
		f->loadHex("TestHex/neg_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("neg",core->decodeInstruction());
		EXPECT_EQ(-0x5, (int8_t)this->ram[16]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(C));
		EXPECT_EQ(1,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));

					
		EXPECT_EQ("neg",core->decodeInstruction());
		EXPECT_EQ(0x10, (int8_t)this->ram[17]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(C));
		EXPECT_EQ(0,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
	}	

	TEST_F(OpCodeTest3,TestMULInstruction){
		f->loadHex("TestHex/mul_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("mul",core->decodeInstruction());
		EXPECT_EQ(0x94, (uint8_t)this->ram[0]);
		EXPECT_EQ(0x16, (uint8_t)this->ram[1]);
		EXPECT_EQ(0,reg->getSREG(C));
		EXPECT_EQ(0,reg->getSREG(Z));


	}
	
	TEST_F(OpCodeTest3,TestRORInstruction){
		f->loadHex("TestHex/ror_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("ror",core->decodeInstruction());
		EXPECT_EQ(0xff, (uint8_t)this->ram[16]);
		EXPECT_EQ(1,reg->getSREG(C));
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(N));
		
		EXPECT_EQ("ror",core->decodeInstruction());
		EXPECT_EQ(0x80, (uint8_t)this->ram[17]);
		EXPECT_EQ(1,reg->getSREG(C));
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(1,reg->getSREG(N));
	}
	
	
	TEST_F(OpCodeTest3,TestSBIWInstruction){
		f->loadHex("TestHex/sbiw_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("sbiw",core->decodeInstruction());
		EXPECT_EQ(0xfe,this->ram[24]);
		EXPECT_EQ(0xff,this->ram[25]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));
		
		EXPECT_EQ("sbiw",core->decodeInstruction());
		EXPECT_EQ(0xef,this->ram[24]);
		EXPECT_EQ(0xff,this->ram[25]);
		EXPECT_EQ(0,reg->getSREG(V));
		EXPECT_EQ(1,reg->getSREG(N));
		EXPECT_EQ(0,reg->getSREG(Z));
		EXPECT_EQ(0,reg->getSREG(C));
	}
	
	TEST_F(OpCodeTest3,TestBLDInstruction){
		f->loadHex("TestHex/bld_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		reg->setSREG(T,0);
		EXPECT_EQ("bld",core->decodeInstruction());
		EXPECT_EQ(0xf7,this->ram[16]);

		reg->setSREG(T, 1);
		EXPECT_EQ("bld",core->decodeInstruction());
		EXPECT_EQ(0xff,this->ram[17]);
	}
	
	TEST_F(OpCodeTest3,TestBSTInstruction){
		f->loadHex("TestHex/bst_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		
		EXPECT_EQ("bst",core->decodeInstruction());
		EXPECT_EQ(1, reg->getSREG(T));

		
		EXPECT_EQ("bst",core->decodeInstruction());
		EXPECT_EQ(0, reg->getSREG(T));
	}
	
	TEST_F(OpCodeTest3,TestSBRC_SBRS_Instruction){
		f->loadHex("TestHex/sbrc_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		
		EXPECT_EQ("sbrs",core->decodeInstruction());
		EXPECT_EQ("sbrc",core->decodeInstruction());
		EXPECT_EQ("sbrc",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
		EXPECT_EQ("sbrs",core->decodeInstruction());
		EXPECT_EQ("ldi",core->decodeInstruction());
	}
	
	
	TEST_F(OpCodeTest3,TestLDS_STS_Instruction){
		f->loadHex("TestHex/lds_sts_test.hex");
		
		//Load Setup Count to Register r20
		EXPECT_EQ("ldi",core->decodeInstruction());
		for (int i = 0 ; i < this->ram[20];i++){
			EXPECT_NE("---",core->decodeInstruction());
		}
		
		EXPECT_EQ("stsl",core->decodeInstruction());
		EXPECT_EQ(0x12,this->ram[127]);
		
		this->ram[100] = 11;
		EXPECT_EQ("lds",core->decodeInstruction());
		EXPECT_EQ(11,this->ram[17]);
	}
}
