#include "CPU.h"

instruction::instruction(bitset<32> fetch)
{

	instr = fetch;
	
}

CPU::CPU() 
{
	
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
}

bitset<32> CPU::Fetch(bitset<8> *instmem) {
	

	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction
	
	ifid.pc = PC;
	PC += 4;//increment PC
	return instr;

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
	//put all data into next stage register
	idex.pc = ifid.pc;
	idex.readRs1 = registerFile[rs1];
	idex.readRs2 = registerFile[rs2];
	idex.rd = (ins >> 7) & 0x1f;
	idex.readImm = int32_t(ins) >> 20;
	//Using opcode to decide the operation
	if (RTYPE == opcode) {
		if (0x0 == func3) {
			if (0x0 == func7) {
				idex.operation = Op::add;//put the operation data to next stage
			}
			else if (0x20 == func7) {
				idex.operation = Op::sub;
			}
			else {
				idex.operation = Op::error;
				return false;
			}
		}
		else if (0x4 == func3) { idex.operation = Op::Xor; }
		else if (0x5 == func3) { idex.operation = Op::sra; }
		else {
			idex.operation = Op::error;
			return false;
		}
		r_counter++;//count the r-type instructions
		ins_counter++;// count all instructions
	}
	else if (ITYPE == opcode) {
		if (0x0 == func3) {
			idex.operation = Op::addi;
		}
		else if (0x7 == func3) {
			idex.operation = Op::andi;
		}
		else {
			idex.operation = Op::error;
			return false;
		}
		ins_counter++;//count all instructions
	}
	else if (LOADWORD == opcode && 0x2 == func3) {
		idex.operation = Op::lw;
		ins_counter++;//count all instructions
	}
	else if (STOREWORD == opcode && 0x2 == func3) {
		int32_t imm11_5 = (int32_t)(ins & 0xfe000000);//generate imm based on the instruction arrangement
		int32_t imm4_0 = (int32_t)(ins & 0xf80) << 13;//second segment
		idex.readImm = (imm11_5 + imm4_0) >> 20;//make sure is 12 bits
		idex.operation = Op::sw;
		ins_counter++;//count all instructions
	}
	else if (BRANCH == opcode) {
		uint32_t imm12 = (int32_t)(ins & 0x80000000) >> 20;//process the most significant bit separately
		uint32_t imm11 = (int32_t)(ins & 0x80) << 3;//the 11th bit also neeb be treated separately
		uint32_t imm10_5 = (int32_t)(ins & 0x7e000000) >> 21;//10_5 segment
		uint32_t imm4_1 = (int32_t)(ins & 0xf00) >> 8;//4_1 segment
		idex.readImm = (imm12 + imm11 + imm10_5 + imm4_1)<< 1;//first bit always zero
		idex.operation = Op::blt;
		ins_counter++;
	}
	else if (JALR == opcode) {
		idex.readImm = (int32_t)(ins & 0xfff00000) >> 20;
		idex.operation = Op::jalr;
		ins_counter++;
	}

	else if (ZERO == opcode) {
		idex.operation = Op::ze;
	}
	else {
		idex.operation = Op::error;
		return false;
	}
return true;
}

void CPU::execute() {
	//put useful data in to next stage
	exmem.pc = idex.pc;//this is PC+4
	exmem.ALUzero = 0;// clear last instruction data
	exmem.pcPlusImm = idex.readImm+idex.pc;// because we incrmented PC at fetch stage, PC-4 is the original PC
	exmem.operation = idex.operation;
	exmem.readData2 = idex.readRs2;
	exmem.rd = idex.rd;
	switch (idex.operation) {
		// ALU compute
		case Op::add:
			exmem.ALUresult = idex.readRs1 + idex.readRs2;
			break;
		case Op::sub:
			exmem.ALUresult = idex.readRs1 - idex.readRs2;
			break;
		case Op::sra:
			exmem.ALUresult = idex.readRs1 >> idex.readRs2;
		case Op::Xor:
			exmem.ALUresult = idex.readRs1 ^ idex.readRs2;
			break;
		case Op::addi:
			exmem.ALUresult = idex.readRs1 + idex.readImm;
			break;
		case Op::andi:
			exmem.ALUresult = idex.readRs1 & idex.readImm;
			break;
		case Op::lw:
		case Op::sw:
			exmem.ALUresult = idex.readRs1 + idex.readImm;
			break;
		case Op::blt:
			exmem.ALUzero = (idex.readRs1 < idex.readRs2) ? 1 : 0;//compare
			break;
		case Op::jalr:
			exmem.ALUresult = (idex.readRs1 + idex.readImm) ;//& 0xfffffffe
			exmem.rd = idex.rd;
			break;
		case Op::ze:
			exmem.ALUresult = ZERO;
		case Op::error:
			break;
	}
	
}

void CPU::memory() {
	//put useful data to next stage
	memwb.pc = exmem.pc;
	memwb.rd = exmem.rd;
	//cout << memwb.rd << endl;
	memwb.ALUresult = exmem.ALUresult;
	memwb.operation = exmem.operation;
	//use four data to mimic the real register
	int32_t lByte;
	int32_t sByte;
	if (exmem.ALUzero != 0) {
		PC = exmem.pcPlusImm;
	}
	if (memwb.operation==Op::jalr) {
		PC = exmem.ALUresult;
	}
	if (exmem.operation==Op::lw) {
		lByte = dmemory[exmem.ALUresult];
		memwb.memData = lByte;
	}
	else if (exmem.operation==Op::sw) {	
		sByte = (exmem.readData2 ) ;
		dmemory[exmem.ALUresult] = sByte;
	
	}
	
}

		
void CPU::writeback() {
	//no rd and no sw operation means no write
	if (ZERO==memwb.rd||Op::sw==memwb.operation) {
		return;
	}
	//load word
	if (memwb.operation==Op::lw) {
		registerFile[memwb.rd] = memwb.memData;
	}
	//jalr
	else if(memwb.operation==Op::jalr){
		registerFile[memwb.rd] = memwb.pc+4;
		
	}
	//all other write back aluresult
	else if (Op::error!=memwb.operation) {
		registerFile[memwb.rd] = memwb.ALUresult;
	}
}


unsigned long CPU::readPC()
{
	return PC;
}
