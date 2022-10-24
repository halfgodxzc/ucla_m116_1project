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
#define BRANCH 0x63
#define JALR 0x67

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

	uint32_t pc;

	enum class Op {
		error,
		ze,
		add,
		sub,
		Xor,
		andi,
		addi,
		sra,
		lw,
		sw,
		blt,
		jalr,
	};

	struct IFID {//Ifid pipeline registers, enable me use build-in void fuctions to complete tasks
		uint32_t pc = 0;
		uint32_t instruction = 0;
	}ifid;

	struct IDEX {//Idex pipeline registers, enable me use build-in void fuctions to complete tasks
		uint32_t pc = 0;
		int32_t readRs1 = 0;
		int32_t readRs2 = 0;
		uint32_t rd = 0;
		int32_t readImm = 0;
		Op operation = Op::ze;
	}idex;

	struct EXMEM {//Exmem pipeline registers, enable me use build-in void fuctions to complete tasks
		uint32_t pc = 0;
		uint32_t pcPlusImm = 0;
		int32_t ALUresult = 0;
		int32_t readData2 = 0;
		int32_t ALUzero = 0;
		uint32_t rd = 0;
		Op operation = Op::ze;

	}exmem;

	struct MEMWB {//Memwb pipeline registers, enable me use build-in void fuctions to complete tasks
		uint32_t pc = 0;
		int32_t memData = 0;
		int32_t ALUresult = 0;
		uint32_t rd = 0;
		Op operation = Op::ze;
	}memwb;

public:
	explicit CPU();
	unsigned long readPC();
	int32_t registerFile[32];       // Registers x0-x31
	int r_counter = 0;				//count r-type instructions
	int ins_counter = 0;			//count all instructions

	bitset<32> Fetch(bitset<8>* instmem);
	bool Decode(instruction* instr);
	void execute();
	void memory();
	void writeback();
	
};




