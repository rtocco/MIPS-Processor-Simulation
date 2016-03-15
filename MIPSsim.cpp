
/* On my honor, I have neither given nor received unauthorized aid on this assigment */

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <queue>
#include "MIPSsim.h"

int main(){ // main method
	initInstructions(); // initialize
	initRegisters();
	initMemory();

	sim.open("simulation.txt"); // output file
	printState(0);
	int step = 1;
	do{                  // execute simulation
		transition();
		printState(step);
		step++;
	}while(finished() == false);
	
	sim.close();
	return 0;
}

void initInstructions(){  // initialize Instructions, store in INM queue
	std::ifstream file("instructions.txt");
	std::string line;
	while(getline(file, line)){
		for(int i = 0; i < line.length(); i++){
			if(line.at(i) == '<' || line.at(i) == '>'){
				line.erase(i, i+1);
			}
		}
		std::string opcode, dr, fso, sso;
		std::stringstream ss(line);
		getline(ss, opcode, ',');
		getline(ss, dr, ',');
		getline(ss, fso, ',');
		getline(ss, sso);
		Instruction *i = new Instruction(opcode, dr, fso, sso);
		INM.push(i);
	}
}

void initRegisters(){  // Initialize Registers, store in RGF
	std::ifstream file("registers.txt");
	std::string line;
	while(getline(file, line)){
		for(int i = 0; i < line.length(); i++){
			if(line.at(i) == '<'){ line.erase(i, i+2); }
			if(line.at(i) == '>'){ line.erase(i, i+1); }
		}
		std::string reg, value;
		std::stringstream ss(line);
		getline(ss, reg, ',');
		int rnum = atoi(reg.c_str());
		getline(ss, value, ',');
		int vnum = atoi(value.c_str());
		RGF[rnum].reg = reg;
		RGF[rnum].val = vnum;
	}
}

void initMemory(){  // Initialize Data Memory, store in DAM
	std::ifstream file("datamemory.txt");
	std::string line;
	while(getline(file, line)){
		for(int i = 0; i < line.length(); i++){
			if(line.at(i) == '<' || line.at(i) == '>'){ line.erase(i, i+1); }
		}
		std::string loc, value;
		std::stringstream ss(line);
		getline(ss, loc, ',');
		int rnum = atoi(loc.c_str());
		getline(ss, value, ',');
		int vnum = atoi(value.c_str());
		DAM[rnum].loc = rnum;
		DAM[rnum].val = vnum;
	}
}

void printState(int step){           // print the current state of the program
	sim << "STEP " << step << ":\n";
	printINM();
	sim << "\n";
	printINB();
	sim << "\n";
	printAIB();
	sim << "\n";
	printSIB();
	sim << "\n";
	printPRB();
	sim << "\n";
	printADB();
	sim << "\n";
	printREB();
	sim << "\n";
	printRGF();
	sim << "\n";
	printDAM();
	sim << "\n\n";
}

void printINM(){                     // print contents of INM
	Instruction *ins;
	std::queue <Instruction*> tempINM = INM;
	sim << "INM:";
	while(tempINM.empty() == false){
		ins = tempINM.front();
		sim << "<" << ins->opcode << "," << ins->dr << "," << ins->fso << "," << ins->sso << ">";
		tempINM.pop();
		if(tempINM.empty() == false){ sim << ","; }
	}
}

void printINB(){                     // print contents of INB
	DecodedInstruction *in;
	sim << "INB:";
	if(INB.empty() == false){
		in = INB.front();
		sim << "<" << in->opcode << "," << in->dr << "," << in->fso << "," << in->sso << ">";
	}
}

void printAIB(){                     // print contents of AIB
	DecodedInstruction *in;
	sim << "AIB:";
	if(AIB.empty() == false){
		in = AIB.front();
		sim << "<" << in->opcode << "," << in->dr << "," << in->fso << "," << in->sso << ">";
	}
}

void printSIB(){                     // print contents of SIB
	DecodedInstruction *in;
	sim << "SIB:";
	if(SIB.empty() == false){
		in = SIB.front();
		sim << "<" << in->opcode << "," << in->dr << "," << in->fso << "," << in->sso << ">";
	}
}

void printPRB(){                     // print contents of PRB
	DecodedInstruction *in;
	sim << "PRB:";
	if(PRB.empty() == false){
		in = PRB.front();
		sim << "<" << in->opcode << "," << in->dr << "," << in->fso << "," << in->sso << ">";
	}
}

void printADB(){                     // print contents of ADB
	Register *inr;
	sim << "ADB:";
	if(ADB.empty() == false){
		inr = ADB.front();
		sim << "<" << inr->reg << "," << inr->val << ">";
	}
}

void printREB(){                     // print contents of REB
	Register *inr;
	sim << "REB:";
	std::queue <Register*> tempREB = REB;
	while(tempREB.empty() == false){
		inr = tempREB.front();
		sim << "<" << inr->reg << "," << inr->val << ">";
		tempREB.pop();
		if(tempREB.empty() == false){ sim << ","; }
	}
}

void printRGF(){                      // print contents of RGF
	sim << "RGF:";
	bool first = true;
	for(int i = 0; i < 16; i++){
		if(RGF[i].reg != "empty"){
			if(first == false){ sim << ","; }
			sim << "<R" << RGF[i].reg << "," << RGF[i].val << ">";
			first = false;
		}
	}
}

void printDAM(){                      // print contents of DAM
	sim << "DAM:";
	bool first = true;
	for(int i = 0; i < 16; i++){
		if(DAM[i].loc != -1){
			if(first == false){ sim << ","; }
			sim << "<" << DAM[i].loc << "," << DAM[i].val << ">";
			first = false;
		}
	}
}

void transition(){ // fire transitions where coins are available
	write();
	store();
	addressCalculation();
	issue2();
	multiply2();
	multiply1();
	addSubtract();
	issue1();
	decodeRead();
}

bool finished(){ // returns true if buffers are empty, false otherwise
	bool fin = true;
	if(INB.empty() == false || AIB.empty() == false || SIB.empty() == false || PRB.empty() == false){
		fin = false;
	}
	if(REB.empty() == false || ADB.empty() == false){
		fin = false;
	}
	return fin;
}

void decodeRead(){ // DECODE and READ
	if(INM.empty() == false){
		std::string opcode = INM.front()->opcode;
		std::string dr = INM.front()->dr;
		INM.front()->fso.erase(0, 1);
		int fso = atoi(INM.front()->fso.c_str());
		fso = RGF[fso].val;
		int sso;
		if(opcode != "ST"){
			INM.front()->sso.erase(0, 1);
			int reg = atoi(INM.front()->sso.c_str());
			sso = RGF[reg].val;
		}
		else{
			sso = atoi(INM.front()->sso.c_str());
		}
		INM.pop();
		DecodedInstruction *in = new DecodedInstruction(opcode, dr, fso, sso);
		INB.push(in);
	}
}

void issue1(){ // ISSUE1
	if(INB.empty() == false){
		if(INB.front()->opcode != "ST"){
			std::string opcode = INB.front()->opcode;
			std::string dr = INB.front()->dr;
			int fso = INB.front()->fso;
			int sso = INB.front()->sso;
			INB.pop();
			DecodedInstruction *in = new DecodedInstruction(opcode, dr, fso, sso);
			AIB.push(in);
		}
	}
}

void addSubtract(){ // ASU
	if(AIB.empty() == false){
		if(AIB.front()->opcode == "ADD"){
			std::string reg = AIB.front()->dr;
			int val = AIB.front()->fso + AIB.front()->sso;
			Register *in = new Register(reg, val);
			REB.push(in);
			AIB.pop();
		}
		else if(AIB.front()->opcode == "SUB"){
			std::string reg = AIB.front()->dr;
			int val = AIB.front()->fso - AIB.front()->sso;
			Register *in = new Register(reg, val);
			REB.push(in);
			AIB.pop();
		}
	}
}

void multiply1(){ // MLU1
	if(AIB.empty() == false){
		if(AIB.front()->opcode == "MUL"){
			std::string opcode = AIB.front()->opcode;
			std::string dr = AIB.front()->dr;
			int fso = AIB.front()->fso;
			int sso = AIB.front()->sso;
			DecodedInstruction *in = new DecodedInstruction(opcode, dr, fso, sso);
			PRB.push(in);
			AIB.pop();
		}
	}
}

void multiply2(){ // MLU2
	if(PRB.empty() == false){
		std::string reg = PRB.front()->dr;
		int val = PRB.front()->fso * PRB.front()->sso;
		Register *in = new Register(reg, val);
		REB.push(in);
		PRB.pop();
	}
}

void issue2(){ // ISSUE2
	if(INB.empty() == false){
		if(INB.front()->opcode == "ST"){
			std::string opcode = INB.front()->opcode;
			std::string dr = INB.front()->dr;
			int fso = INB.front()->fso;
			int sso = INB.front()->sso;
			INB.pop();
			DecodedInstruction *in = new DecodedInstruction(opcode, dr, fso, sso);
			SIB.push(in);
		}
	}
}

void addressCalculation(){ // ADDR
	if(SIB.empty() == false){
		int registerVal = SIB.front()->fso;
		int offset = SIB.front()->sso;
		std::string reg = SIB.front()->dr;
		int val = registerVal + offset;
		Register *in = new Register(reg, val);
		ADB.push(in);
		SIB.pop();
	}
}

void store(){ // STORE
	if(ADB.empty() == false){
		ADB.front()->reg.erase(0,1);
		int reg = atoi(ADB.front()->reg.c_str());
		int storeValue = RGF[reg].val;
		DAM[ADB.front()->val].loc = ADB.front()->val;
		DAM[ADB.front()->val].val = storeValue;
		ADB.pop();
	}
}

void write(){ // WRITE
	if(REB.empty() == false){
		REB.front()->reg.erase(0, 1);
		std::string regName = REB.front()->reg;
		int reg = atoi(REB.front()->reg.c_str());
		RGF[reg].reg = regName;
		RGF[reg].val = REB.front()->val;
		REB.pop();
	}
}