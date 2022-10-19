#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <bitset>

using namespace std;
#define ZERO 0x0  // Five in a row indicates the program has completed
#define RTYPE 0x33
#define ITYPE 0x13
#define LOADWORD 0x3
#define STOREWORD 0x23

class instruction {
public:
	bitset<32> instr;//instruction
	instruction(bitset<32> fetch); // constructor

};

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 
	vector<uint8_t> insMem;
	vector<uint8_t> dataMem;
	int32_t registerFile[32];
	uint32_t pc;

	enum class Op {
		error,
		ze,
		add,
		sub,
		Or,
		And,
		addi,
		ori,
		andi,
		lw,
		sw,
	};

	struct IFID {
		uint32_t pc = 0;
		uint32_t instruction = 0;
	}ifidCurr,ifidNext;

	struct IDEX {
		uint32_t pc = 0;
		int32_t readRs1 = 0;
		int32_t readRs2 = 0;
		uint32_t rd = 0;
		int32_t readImm = 0;
		Op operation = Op::ze;
	}idexCurr,idexNext;

	struct EXMEM {
		uint32_t pcPlusImm = 0;
		int32_t ALUresult = 0;
		int32_t writeRs2 = 0;
		int32_t ALUzero = 0;
		uint32_t rd = 0;
		Op operation = Op::ze;

	}exmemCurr,exmemNext;

	struct MEMWB {
		uint32_t memData = 0;
		int32_t ALUresult = 0;
		uint32_t rd = 0;
		Op operation = Op::ze;
	}memwbCurr,memwbNext;

public:
	explicit CPU();
	unsigned long readPC();
	bitset<32> Fetch(bitset<8>* instmem);
	bool Decode(instruction* instr);
	void execute();
	void memory();
	void writeback();
	void clockTick();
	bool isFinished();
	void printStats();
};




