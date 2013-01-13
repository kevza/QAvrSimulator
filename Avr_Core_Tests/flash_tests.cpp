/**
 * @brief Runs tests on the virtual flash memory
 */
#include "gtest/gtest.h" 
#include "flash.h"
#include <stdint.h>
enum {C,Z,N,V,S,H,T,I};

namespace {
	class FlashTest : public ::testing::Test {
		 protected:
			  virtual void SetUp() {


			  }

	  // virtual void TearDown() {}
	};

	/**
	 * @brief Run tests on file loading
	 **/
	TEST_F(FlashTest, LoadTest){
			//Create a flash 32000 bytes long
			Avr_Flash f(32000);
			
			EXPECT_EQ(true, f.loadHex("TestHex/CoreTest.hex"));
			uint16_t *flash = f.getFlash();
			//Test the first few instructions in Test.hex
			EXPECT_EQ(0x940c,flash[0]);
			EXPECT_EQ(0x005c,flash[1]);
			EXPECT_EQ(0x940c,flash[2]);

	}
	
	/**
	 * @brief Test hex to dec
	 */
	 TEST_F(FlashTest,HexToDec){
		 Avr_Flash f(1);
		 EXPECT_EQ(10,f.hexToDec('A'));
		 EXPECT_EQ(11,f.hexToDec('B'));
		 EXPECT_EQ(12,f.hexToDec('C'));
		 EXPECT_EQ(13,f.hexToDec('D'));
		 EXPECT_EQ(14,f.hexToDec('E'));
		 EXPECT_EQ(15,f.hexToDec('F'));
		 for (int i = 0 ; i < 10; i++){
			 EXPECT_EQ(i,f.hexToDec(char(i + '0')));
		 }
	 } 

}

