#include <iostream>
using namespace std;
#include <sstream>
#include <fstream>
#include <string>
int PC = 0;
std::vector<std::string> instructions;
std::vector<int> rf(32,0);
int branch_target = 0;
bool is_branch_taken = false;
int operandA = 0;
int operandB = 0;
int alu_result = 0;
int alu_zero=0;
int offset = 0;
int destReg=0;
int rs1_global;
int rs2_global;
int rd_global;
int imm_value;
int offset_global;
int UJ_imm;
string alu_opcode;
string alu_funct3;
string alu_funct7;
int alu_ctrl;
string Type_Instruction;
const int DMEM_SIZE = 32;
int d_mem[DMEM_SIZE] = {0};
int TotalClockCycles = 0;

void Execute( int PC, int offset); //calling function here so i dont have to move it 
int Mem(string instructionType, int address, int valueToStore ); 
void Control_Unit(const std::string& opcode,const std::string& funct3, const std::string& funct7);
void Writeback(int value, int destReg);

string intToHex(int num) {
    stringstream ss;
    ss << hex << num;
    return ss.str();
}
string op(string binary) {
    alu_opcode=binary.substr(25,7);
    return binary.substr(25,7);
}
string getFunct3(string binary) {
    alu_funct3=binary.substr(17, 3);
    return binary.substr(17, 3);
}
string getFunct7(string binary) {
    alu_funct7=binary.substr(0, 7);
    return binary.substr(0, 7);
}
string getRs1(string binary) {
    int rs1_num = stoi(binary.substr(12, 5), nullptr, 2);
    rs1_global = rs1_num;

    operandA = rf[rs1_num];

    return "x" + to_string(rs1_num);
}
string getRs2(string binary) {
    int rs2_num = stoi(binary.substr(7, 5), nullptr, 2);
    rs2_global = rs2_num;
    operandB = rf[rs2_num];
    
    return "x" + to_string(rs2_num);
}
int zerotofour(string binary) {
    int rd_num = stoi(binary.substr(7, 5), nullptr, 2);
    destReg = rd_num;
    rd_global = rd_num;
    return rd_num;
}
string getRd(string binary) {
    int rd_num = stoi(binary.substr(20, 5), nullptr, 2);
    destReg = rd_num;   
    rd_global = rd_num;
    return "x" + to_string(rd_num);
}
int todecimal(string binary) {
    return stoi(binary, nullptr, 2);
}
int todecimalForI(string binary) {
    string imm = binary.substr(0, 12);
    if(binary[0] == '1') {
        string inverted = imm;
        for (char& c : inverted) {
            c = (c == '1') ? '0' : '1';
        }
        imm = inverted;
        int value = stoi(imm, nullptr, 2);
        offset = stoi(imm, nullptr, 2); //added
        return (value + 1) * -1;
    }
    offset_global = stoi(imm, nullptr, 2);
    offset = stoi(imm, nullptr, 2);//added
    return stoi(imm, nullptr, 2);
}
int getSTypeImm(string binary) {
    string imm1 = binary.substr(0, 7);
    string imm2 = binary.substr(20, 5);
    string imm = imm1 + imm2;
    string inverted = imm1;
    if(binary[0] == '1'){
        for (char& c : inverted) {
            c = (c == '1') ? '0' : '1';
        }
        imm1 = inverted;
        string inverted2 = imm2;
        for (char& c : inverted2) {
            c = (c == '1') ? '0' : '1';
        }
        imm2 = inverted2;
        imm = imm1 + imm2;
        int value2 = stoi(imm, nullptr, 2);
        return (value2 + 1) * -1;
    }
    imm_value = stoi(imm, nullptr, 2);
    int value2 = stoi(imm, nullptr, 2);
    return value2;
}
int getSBTypeImm(string binary) {
    string imm12 = binary.substr(0, 1);
    string imm11 = binary.substr(24, 1);
    string imm10 = binary.substr(1, 6);
    string imm4 = binary.substr(20, 4);
    string allcombined = imm12 + imm11 + imm10 + imm4 + "0";
    int decimalValue = stoi(allcombined, nullptr, 2);
    if(binary[0] == '1') {
        string inverted = imm12;
        for (char& c : inverted) {
            c = (c == '1') ? '0' : '1';
        }
        imm12 = inverted;
        string inverted2 = imm11;
        for (char& c : inverted2) {
            c = (c == '1') ? '0' : '1';
        }
        imm11 = inverted2;
        string inverted3 = imm10;
        for (char& c : inverted3) {
            c = (c == '1') ? '0' : '1';
        }
        imm10 = inverted3;
        string inverted4 = imm4;
        for (char& c : inverted4) {
            c = (c == '1') ? '0' : '1';
        }
        imm4 = inverted4;
        string imm = imm12 + imm11 + imm10 + imm4 + "0";
        int value5 = stoi(imm, nullptr, 2);
        return (value5 + 2) * -1;
    }
    return decimalValue;
}

int getUJTypeImm(string binary) {
    string imm20 = binary.substr(0, 1);
    string imm10 = binary.substr(1, 10);
    string imm11 = binary.substr(11, 1);
    string imm19 = binary.substr(12, 8);
    string imm = imm20 + imm19 + imm11 + imm10 + "0";
    if(binary[0] == '1') {
        string inverted = imm;
        for (char& c : inverted) {
            c = (c == '1') ? '0' : '1';
        }
        int value44 = stoi(inverted, nullptr, 2);
        return (value44 + 1) * -1;
    }
    int immVal = stoi(imm, nullptr, 2);
    UJ_imm = immVal;
    return immVal;
}


void printR(string instruction, string funct3, string funct7) {
    getRs1(instruction);
    getRs2(instruction);
    getRd(instruction);
    todecimal(funct3);
    todecimal(funct7);

}
void decode(string instruction) {
    string opcode = op(instruction);
    string funct3 = getFunct3(instruction);
    string funct7 = getFunct7(instruction);
    if (opcode == "0110011") { //R type
        if (funct3 == "000") {
            if (funct7 == "0000000") {//add
                Type_Instruction = "R";
                alu_ctrl=2;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            } else if(funct7 == "0100000"){ //sub                
                alu_ctrl=6;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            }
        } else if (funct3 == "111") {
            if(funct7 == "0000000") { //and
                alu_ctrl=0;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            }
        } else if (funct3 == "110") {
            if(funct7 == "0000000") { //or
                alu_ctrl=1;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            }
        } else if (funct3 == "001") {
            if(funct7 == "0000000") { //sll
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "010") {
            if(funct7 == "0000000") {//slt
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "101") {
            if(funct7 == "0100000") {//sra
                printR(instruction,funct3,funct7);
            } else if (funct7 == "0000000") {//srl
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "100") {
            if(funct7 == "0000000") {//xor
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "011") {
            if(funct7 == "0000000") {//sltu
                printR(instruction,funct3,funct7);
            }
        }
    } else if (opcode == "0000011") {// type I
        if(funct3 == "000") {//lb
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if (funct3 == "010") { //lw
            Type_Instruction = "I";
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
            alu_ctrl=2;
            Execute(PC,offset);
        } else if (funct3 == "001") {//lh
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        }
    } else if(opcode == "0010011") { //type I
        if(funct3 == "011") {//sltiu
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "000") { //addi
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "111") {//andi
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "110") {//ori
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "001") {
            if(funct7 == "0000000") { //slli
                getRs1(instruction);
                getRd(instruction);
                zerotofour((instruction));
                intToHex(todecimalForI((instruction)));
            } 
        } else if(funct3 == "010") {//slti
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "101") {
            if(funct7 == "0100000") {//srai
                getRs1(instruction);
                getRd(instruction);
                zerotofour((instruction));
                intToHex(todecimalForI((instruction)));
            } else if (funct7 == "0000000") {//srli
                getRs1(instruction);
                getRd(instruction);
                zerotofour((instruction));
                intToHex(todecimalForI((instruction)));
            }
        } else if(funct3 == "100") {//xori
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        }

    } else if (opcode == "1100111") { //type I
        if(funct3 == "000") {//jalr
            Type_Instruction == "I_J";
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
            TotalClockCycles++;
        }

    } else if (opcode == "0100011") { //Type: S
        if(funct3 == "000") { //sb
            getRs1(instruction);
            getRs2(instruction);
            getSTypeImm(instruction);
            intToHex(getSTypeImm(instruction));
        } else if(funct3 == "001") {//sh
            getRs1(instruction);
            getRs2(instruction);
            getSTypeImm(instruction);
            intToHex(getSTypeImm(instruction));
        } else if(funct3 == "010") {//sw
            getRs1(instruction);
            getRs2(instruction);
            getSTypeImm(instruction);
            intToHex(getSTypeImm(instruction));
            Type_Instruction = "S";
            Execute(PC,offset);
            alu_ctrl=2;
        }
    }
    else if (opcode == "1100011") { // type SB
        if(funct3 == "000") {//beq
            Type_Instruction ="B";
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
            Execute(PC,offset);
            alu_ctrl=6;
        } else if(funct3 == "001") {// bne
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
        } else if(funct3 == "100") {//blt
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
        } else if(funct3 == "101") {//bge
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
        }
    } else if (opcode == "1101111") { //Instruction Type: UJ jal
        Type_Instruction = "UJ";
        getRd(instruction);
        getUJTypeImm(instruction);
        intToHex(getUJTypeImm(instruction));
        TotalClockCycles++;
    }
    else {
        cout << "not valid\n";
    }
}

// Load all instructions from the file (do this once)
void fetch_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "'." << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        instructions.push_back(line);
    }
    cout << "Enter the program file name to run:" << endl;
    cout<<endl;
    cout << filename << endl;
    cout<<endl;
    file.close();
}
void fetch( string instructions_taken) {
    int next_pc = PC + 4; // Next sequential instruction address
    // Choose next PC: if branch taken, use branch_target, else use next_pc
    if (is_branch_taken) {
        //cout << "Branch taken. Jumping to 0x" << intToHex(branch_target) << endl;
        //PC = branch_target;
    } else {
        PC = next_pc;
    }
    // Simulate instruction fetch
    //if (PC / 4 < instructions.size()) {
        // cout << "Fetched instruction at PC = 0x" << intToHex(PC) << ": " << instructions[PC / 4] << endl;
    decode(instructions_taken);
    
    cout<< instructions_taken<<endl;
           
    //} else {
        // cout << "PC out of range: 0x" << intToHex(PC) << ". No instruction to fetch." << endl;
    //}
}
void Execute(int PC, int offset) {
    Control_Unit(alu_opcode,alu_funct3,alu_funct7);
    //cout<<alu_ctrl<<endl;
    // int branch_offset = offset<<1;
    // branch_target = (PC+4)+branch_offset;
    int alu_result = 0;
    if (alu_ctrl == 0) {//R-Type and
        alu_result = operandA & operandB;
        Writeback(alu_result,destReg);
    } else if (alu_ctrl == 1) {//R-type or
        alu_result = operandA | operandB;
        Writeback(alu_result,destReg);
    } else if (alu_ctrl == 2) {// lw, sw , R-type add
        alu_result = operandA + operandB;
       
        if(Type_Instruction == "I"){
            //cout<<"yes it goes to IIIIIII"<<endl;
            int address =alu_result;
            int loaded_data =Mem("LW",address,1);
            Writeback(loaded_data,destReg);
        } else if (Type_Instruction == "S"){
            alu_result = operandA + imm_value;
            int address =alu_result;
            TotalClockCycles++;
            Mem("SW",address,operandB);
        } else if (Type_Instruction =="R"){
            //cout<< "yes it uses r type write"<<endl;
            Writeback(alu_result,destReg);
        } else if(Type_Instruction == "UJ"){
            Writeback(alu_result,destReg);
        } else if(Type_Instruction == "I_J"){
            // alu_result = operandA + imm_value;
            // int address =alu_result;
            // Mem("I_J",address,operandB);
        }
       
    } else if (alu_ctrl == 6) {// beq , Rtype sub
        alu_result = operandA - operandB;
        alu_zero = (alu_result == 0) ? 1 : 0;
        if(Type_Instruction=="B"){
            is_branch_taken = (alu_zero == 1);
            TotalClockCycles++;
            return;
        }else{
            Writeback(alu_result,destReg);
        }
        
    } else {
        cout << "Invalid ALU control signal!" << alu_ctrl  << endl;
    }
    if(alu_result ==0){
        alu_zero=1;
    }else{
        alu_zero=0;
    } 
}
int Mem(string instructionType, int address, int valueToStore = 0) {
    int index = (address+offset_global) / 4 ;
    
    if (index < 0 || index >= 32) {
        cerr << "Memory access error: Address out of bounds\n";
        return -1;
    }
    if (instructionType == "SW") {
        d_mem[index] = valueToStore;
        return 0;
    }
    else if (instructionType == "LW") {
        return d_mem[index];
    }
    else {
        cerr << "Unknown instruction type in Mem()\n";
        return -1;
    }
    offset_global=0;
}

void Writeback(int value, int destReg) {
    if (destReg != 0) {
        rf[destReg] = value;
    }
    TotalClockCycles++;
    
}
void Control_Unit(const std::string& opcode,const std::string& funct3, const std::string& funct7){
    if(opcode == "0110011"){//Rtype
        if (funct3 == "000" && funct7 == "0000000") alu_ctrl = 2; // ADD
        else if (funct3 == "000" && funct7 == "0100000") alu_ctrl = 6; // SUB
        else if (funct3 == "111") alu_ctrl = 0; // AND
        else if (funct3 == "110") alu_ctrl = 1; // OR
        else if (funct3 == "100") alu_ctrl = 3; // XOR
    }else if (opcode == "0010011"){//Itype
        if (funct3 == "000") alu_ctrl = 2; // ADDI
        else if (funct3 == "111") alu_ctrl = 0; // ANDI
        else if (funct3 == "110") alu_ctrl = 1; // ORI
    }else if (opcode == "0000011" || opcode == "0100011"){//lw and sw
        alu_ctrl = 2;
    }else if(opcode == "1100011"){//branch
        alu_ctrl = 6; 
    } else if(opcode == "1100111"){//jalr
        alu_ctrl = 2; 
    } else if(opcode == "1101111"){//jal
        alu_ctrl = 2; 
    }
}

int main() {
    fetch_file("sample_part2.txt");
    branch_target = 0;
    is_branch_taken = false;
    // This is for sample 1 
    // rf[1]= 32;
    // rf[2]= 5;
    // rf[10]= 112;
    // rf[11]=4;
    // d_mem[28]=5;
    // d_mem[29]=16;
    // This is for sample 2 
    rf[8]=32;
    rf[10]=5;
    rf[11]=2;
    rf[12]=10;
    rf[13]=15;
    while (PC/4< instructions.size()) {
        std::string instr = instructions[PC / 4];
        fetch(instr);
        if(alu_opcode == "0110011"){//Rtype
            cout <<"TotalClockCycles "<< TotalClockCycles <<" : " << endl;
            cout << "x"<<rd_global <<" is modified to 0x"<< intToHex(rf[rd_global]) <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
            cout<<endl;
        }else if (alu_opcode == "0010011"){//Itype
            cout <<"TotalClockCycles "<< TotalClockCycles <<" : " << endl;
            cout << "x"<<rd_global <<" is modified to 0x"<< rs1_global <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
            cout<<endl;
        }else if (alu_opcode == "0000011"){//lw
            cout <<"TotalClockCycles "<< TotalClockCycles <<" : " << endl;
            cout << "x"<<rd_global <<" is modified to 0x"<< rs1_global <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
            cout<<endl;
        }else if (alu_opcode == "0100011"){//Stype
            cout <<"TotalClockCycles "<< TotalClockCycles <<" : " << endl;
            cout << "memory x"<<intToHex(rf[rs1_global])<<" is modified to 0x"<< intToHex(rf[rs2_global]) <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
            cout<<endl;
        }else if(alu_opcode == "1100011"){//branch
            cout <<"TotalClockCycles "<< TotalClockCycles <<" : " << endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
            cout<<endl;
        } else if(alu_opcode == "1101111"){//jal
            cout <<"TotalClockCycles "<< TotalClockCycles <<" : " << endl;
            cout << "x1"<<" is modified to 0x"<< intToHex(PC)<<endl;
            cout << "pc is modified to 0x"<< UJ_imm << endl;
            cout<<endl;
        } else if(alu_opcode == "1100111"){//jalr
            cout <<"TotalClockCycles "<< TotalClockCycles <<" : " << endl;
            cout << "x1"<<" is modified to 0x"<< rf[rd_global] <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
            cout<<endl;
        }
        //jal x1, 8
        //jal x1, 16
        //add x10,x11,x12
        //sub x30, x13, x10
        //jalr x1,0(x1)
        //sw x30, 0(x8)
        // cout<<"x30:"<< rf[30]<<endl;
    }
   
    return 0;
}
