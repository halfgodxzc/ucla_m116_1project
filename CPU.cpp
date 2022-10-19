#include "CPU.h"

instruction::instruction(bitset<32> fetch)
{
	//cout << fetch << endl;
	instr = fetch;
	//cout << instr << endl;
}

CPU::CPU()//std::vector<uint8_t>&& iMem, std::vector<uint8_t>&& dMem) 
{
	//insMem(std::move(iMem));
	//dataMem(std::move(dMem));
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
}

bitset<32> CPU::Fetch(bitset<8> *instmem) {
	//if (PC+3<insMem.size()) {

	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction
	//PC += 4;//increment PC
	ifidNext.pc = PC;
	cout << instr << endl;
	return instr;//}
	//else {
	//bitset<32> instr = ZERO;
	//ifidNext.pc = PC;
	
	//return instr;
	//}

}


bool CPU::Decode(instruction* curr)
{
	//get all codes
	uint32_t ins = curr->instr.to_ulong();
	uint32_t opcode = ins & 0x7f;
	uint32_t func3 = (ins >> 12) & 0x7;
	uint32_t func7 = (ins >> 25) & 0x7f;
	uint32_t rs1 = (ins >> 15) & 0x1f;
	uint32_t rs2 = (ins >> 20) & 0x1f;

	idexNext.pc = ifidCurr.pc;
	idexNext.readRs1 = registerFile[rs1];
	idexNext.readRs2 = registerFile[rs2];
	idexNext.rd = (ins >> 7) & 0x1f;
	idexNext.readImm = int32_t(ins) >> 20;

	if (RTYPE == opcode) {
		if (0x0 == func3) {
			if (0x0 == func7) {
				idexNext.operation = Op::add;
			}
			else if (0x20 == func7) {
				idexNext.operation = Op::sub;
			}
			else { idexNext.operation = Op::error; }
		}
		else if (0x6 == func3) { idexNext.operation = Op::Or; }
		else if (0x7 == func3) { idexNext.operation = Op::And; }
		else {
			idexNext.operation = Op::error;
			return false;
		}
	}
	else if (ITYPE==opcode) {
		if (0x0==func3) {
			idexNext.operation = Op::addi;
		}
		else if (0x6==func3) {
			idexNext.operation = Op::ori;
		}
		else if (0x7==func3) {
			idexNext.operation = Op::andi;
		}else{
			idexNext.operation == Op::error;
			return false;
		}
	}
	else if (LOADWORD==opcode&&0x2==func3) {
		idexNext.operation = Op::lw;
	}
	else if (STOREWORD == opcode && 0x2 == func3) {
		int32_t imm11_5 = (int32_t)(ins&0xfe000000);
		int32_t imm4_0 = (int32_t)(ins & 0xf80) << 13;
		idexNext.readImm = (imm11_5 + imm4_0) >> 20;
		idexNext.operation = Op::sw;
	}
	else if (ZERO==opcode) {
		idexNext.operation = Op::ze;
	}
return true;
}

void CPU::execute() {

}

void CPU::memory() {
}

void CPU::writeback() {

}

void CPU::clockTick() {
}

bool CPU::isFinished() {
	return true;
}

void CPU::printStats() {
}

unsigned long CPU::readPC()
{
	return PC;
}
