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
	class SpeedTests : public ::testing::Test {
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

		
	TEST_F(SpeedTests, TestSPEED){
		f->loadHex("TestHex/speed_test.hex");
		//Run Load Data Instructions
		time_t startTime,finishTime;
		startTime = time(NULL);
		for (int i = 0 ; i < (20 * 5839416);i++){
			core->decodeInstruction();
		}
		finishTime = time(NULL);
		
		EXPECT_TRUE(finishTime - startTime <= 30);
	}
}
