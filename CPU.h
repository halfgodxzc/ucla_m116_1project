#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;


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
		andi,
		xor,
		addi,
		sra,
		lw,
		sw,
		blt,
		jalr
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
	CPU();
	unsigned long readPC();
	void fetch();
	void decode();
	void execute();
	void memory();
	void writeback();
	void clockTick();
	bool isFinished();
	void printStats();
};




