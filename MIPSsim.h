#ifndef MIPSSIM_H
#define MIPSSIM_H

struct Instruction{ // initial instruction input
	Instruction(std::string opcode, std::string dr, std::string fso, std::string sso){
		this->opcode = opcode;
		this->dr = dr;
		this->fso = fso;
		this->sso = sso;
	}
	std::string opcode, dr, fso, sso;
};
struct DecodedInstruction{ // instructions with numbers to operate on
	DecodedInstruction(std::string opcode, std::string dr, int fso, int sso){
		this->opcode = opcode;
		this->dr = dr;
		this->fso = fso;
		this->sso = sso;
	}
	std::string opcode, dr;
	int fso, sso;
};
struct Register{ // register struct with name and value
	Register(){ reg = "empty"; val = 0; }
	Register(std::string reg, int val){ this->reg = reg; this->val = val; }
	std::string reg;
	int val;
};
struct Memory{ // memory struct with location and value
	Memory(){ loc = -1; val = 0;}
	Memory(int loc, int val){ this->loc = loc; this->val = val; }
	int loc, val;
};

void initInstructions(); // initialization methods
void initRegisters();
void initMemory();

void printState(int step); // referenced by main
void printINM();
void printINB();
void printAIB();
void printSIB();
void printPRB();
void printADB();
void printREB();
void printRGF();
void printDAM();
void transition();
bool finished();

void decodeRead(); // referenced by transition()
void issue1();
void addSubtract();
void multiply1();
void multiply2();
void issue2();
void addressCalculation();
void store();
void write();

std::queue <Instruction*> INM; // data structures for states
Register RGF[16];         // global for simplicity
Memory DAM[16];
std::queue <DecodedInstruction*> INB;
std::queue <DecodedInstruction*> AIB;
std::queue <DecodedInstruction*> SIB;
std::queue <Register*> REB;
std::queue <DecodedInstruction*> PRB;
std::queue <Register*> ADB;
std::ofstream sim;

#endif