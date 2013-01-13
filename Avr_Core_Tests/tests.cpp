#include "gtest/gtest.h"

enum {C,Z,N,V,S,H,T,I};

//#include "flash_tests.cpp"
//#include "core_tests.cpp"
//#include "opcode_tests.cpp"
//#include "opcode_tests2.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
