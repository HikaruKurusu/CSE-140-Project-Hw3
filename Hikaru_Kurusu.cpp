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
int RegWrite =0;
int Branch = 0;
int MemRead =0;
int MemtoReg =0;
int MemWrite=0;
int AluSrc =0;
int AluOp =0;
int rs1_global;
int rs2_global;
int rd_global;
int offset_global;
string alu_opcode;
string alu_funct3;
string alu_funct7;
int alu_ctrl;
string Type_Instruction;
const int DMEM_SIZE = 32;
int d_mem[DMEM_SIZE] = {0};
int total_clock_cycles = 0;
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
    return immVal;
}


void printR(string instruction, string funct3, string funct7) {
    //cout << "Rs1: " << getRs1(instruction) << endl;
    //cout << "Rs2: " << getRs2(instruction) << endl;
    //cout << "Rd: " << getRd(instruction) << endl;
    //cout << "Funct3: " << todecimal(funct3) << endl;
    //cout << "Funct7: " << todecimal(funct7) << endl;
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
    if (opcode == "0110011") {
        // cout << "Instruction Type: R\n";
        if (funct3 == "000") {
            if (funct7 == "0000000") {
                // cout << "Operation: add\n";
                Type_Instruction = "R";
                alu_ctrl=2;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            } else if(funct7 == "0100000"){
                 //cout << "Operation: sub\n";
                
                alu_ctrl=6;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            }
        } else if (funct3 == "111") {
            if(funct7 == "0000000") {
                // cout << "Operation: and\n";
                
                alu_ctrl=0;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            }
        } else if (funct3 == "110") {
            if(funct7 == "0000000") {
                // cout << "Operation: or\n";
                alu_ctrl=1;
                printR(instruction,funct3,funct7);
                Execute(PC,offset);
            }
        } else if (funct3 == "001") {
            if(funct7 == "0000000") {
                // cout << "Operation: sll\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "010") {
            if(funct7 == "0000000") {
                // cout << "Operation: slt\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "101") {
            if(funct7 == "0100000") {
                // cout << "Operation: sra\n";
                printR(instruction,funct3,funct7);
            } else if (funct7 == "0000000") {
                // cout << "Operation: srl\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "100") {
            if(funct7 == "0000000") {
                // cout << "Operation: xor\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "011") {
            if(funct7 == "0000000") {
                // cout << "Operation: sltu\n";
                printR(instruction,funct3,funct7);
            }
        }
    } else if (opcode == "0000011") {
        // cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            // cout << "Operation: lb\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if (funct3 == "010") {
            Type_Instruction = "I";
            // cout << "Operation: lw\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
            
            alu_ctrl=2;
            Execute(PC,offset);
        } else if (funct3 == "001") {
            // cout << "Operation: lh\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        }
    } else if(opcode == "0010011") {
        // cout << "Instruction Type: I\n";
        if(funct3 == "011") {
            // cout << "Operation: sltiu\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "000") {
            // cout << "Operation: addi\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "111") {
            // cout << "Operation: andi\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "110") {
            // cout << "Operation: ori\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "001") {
            if(funct7 == "0000000") {
                // cout << "Operation: slli\n";
                // cout << "Rs1: " << getRs1(instruction) << endl;
                // cout << "Rd: " << getRd(instruction) << endl;
                // cout << "Immediate: " << zerotofour((instruction)) << " (or 0x" << intToHex(zerotofour((instruction))) << ")" << endl;
                getRs1(instruction);
                getRd(instruction);
                zerotofour((instruction));
                intToHex(todecimalForI((instruction)));
            } 
        } else if(funct3 == "010") {
            // cout << "Operation: slti\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        } else if(funct3 == "101") {
            if(funct7 == "0100000") {
                // cout << "Operation: srai\n";
                // cout << "Rs1: " << getRs1(instruction) << endl;
                // cout << "Rd: " << getRd(instruction) << endl;
                // cout << "Immediate: " << zerotofour((instruction)) << " (or 0x" << intToHex(zerotofour((instruction))) << ")" << endl;
                getRs1(instruction);
                getRd(instruction);
                zerotofour((instruction));
                intToHex(todecimalForI((instruction)));
            } else if (funct7 == "0000000") {
                // cout << "Operation: srli\n";
                // cout << "Rs1: " << getRs1(instruction) << endl;
                // cout << "Rd: " << getRd(instruction) << endl;
                // cout << "Immediate: " << zerotofour((instruction)) << " (or 0x" << intToHex(zerotofour((instruction))) << ")" << endl;
                getRs1(instruction);
                getRd(instruction);
                zerotofour((instruction));
                intToHex(todecimalForI((instruction)));
            }
        } else if(funct3 == "100") {
            // cout << "Operation: xori\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        }

    } else if (opcode == "1100111") {
        // cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            // cout << "Operation: jalr\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rd: " << getRd(instruction) << endl;
            // cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
            getRs1(instruction);
            getRd(instruction);
            todecimalForI((instruction));
            intToHex(todecimalForI((instruction)));
        }

    } else if (opcode == "0100011") {
        // cout << "Instruction Type: S\n";
        if(funct3 == "000") {
            // cout << "Operation: sb\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rs2: " << getRs2(instruction) << endl;
            // cout << "Immediate: " << getSTypeImm(instruction) << " (or 0x" << intToHex(getSTypeImm(instruction)) << ")" << endl;
            getRs1(instruction);
            getRs2(instruction);
            getSTypeImm(instruction);
            intToHex(getSTypeImm(instruction));
        } else if(funct3 == "001") {
            // cout << "Operation: sh\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rs2: " << getRs2(instruction) << endl;
            // cout << "Immediate: " << getSTypeImm(instruction) << " (or 0x" << intToHex(getSTypeImm(instruction)) << ")" << endl;
            getRs1(instruction);
            getRs2(instruction);
            getSTypeImm(instruction);
            intToHex(getSTypeImm(instruction));
        } else if(funct3 == "010") {
            // cout << "Operation: sw\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rs2: " << getRs2(instruction) << endl;
            // cout << "Immediate: " << getSTypeImm(instruction) << " (or 0x" << intToHex(getSTypeImm(instruction)) << ")" << endl;
            getRs1(instruction);
            getRs2(instruction);
            getSTypeImm(instruction);
            intToHex(getSTypeImm(instruction));
            Type_Instruction = "S";
            Execute(PC,offset);
            alu_ctrl=2;
        }
    }
    else if (opcode == "1100011") {
        // cout << "Instruction Type: SB\n";
        if(funct3 == "000") {
            // cout << "Operation: beq\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rs2: " << getRs2(instruction) << endl;
            // cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
            Type_Instruction ="B";
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
            Execute(PC,offset);
            alu_ctrl=6;
        } else if(funct3 == "001") {
            // cout << "Operation: bne\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rs2: " << getRs2(instruction) << endl;
            // cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
        } else if(funct3 == "100") {
            // cout << "Operation: blt\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rs2: " << getRs2(instruction) << endl;
            // cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
        } else if(funct3 == "101") {
            // cout << "Operation: bge\n";
            // cout << "Rs1: " << getRs1(instruction) << endl;
            // cout << "Rs2: " << getRs2(instruction) << endl;
            // cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
            getRs1(instruction);
            getRs2(instruction);
            getSBTypeImm(instruction);
            intToHex(getSBTypeImm(instruction));
        }
    } else if (opcode == "1101111") {
        // cout << "Instruction Type: UJ\n";
        // cout << "Operation: jal\n";
        // cout << "Rd: " << getRd(instruction) << endl;
        // cout << "Immediate: " << getUJTypeImm(instruction) << " (or 0x" << intToHex(getUJTypeImm(instruction)) << ")" << endl;
        getRd(instruction);
        getUJTypeImm(instruction);
        intToHex(getUJTypeImm(instruction));
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

    file.close();
}
void fetch( string instructions_taken) {
    int next_pc = PC + 4; // Next sequential instruction address

    // Choose next PC: if branch taken, use branch_target, else use next_pc
    if (is_branch_taken) {
        cout << "Branch taken. Jumping to 0x" << intToHex(branch_target) << endl;
        PC = branch_target;
    } else {
        PC = next_pc;
    }

    // Simulate instruction fetch
    if (PC / 4 < instructions.size()) {
        // cout << "Fetched instruction at PC = 0x" << intToHex(PC) << ": " << instructions[PC / 4] << endl;
        decode(instructions_taken);
        cout<< instructions_taken<<endl;
           
    } else {
        // cout << "PC out of range: 0x" << intToHex(PC) << ". No instruction to fetch." << endl;
    }
}

void Execute(int PC, int offset) {
    Control_Unit(alu_opcode,alu_funct3,alu_funct7);
    //cout<<alu_ctrl<<endl;
    int branch_offset = offset<<1;
    branch_target = (PC+4)+branch_offset;
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
        }else if (Type_Instruction == "S"){
            int address =alu_result;
            Mem("SW",address,operandB);
        }else if (Type_Instruction =="R"){
            //cout<< "yes it uses r type write"<<endl;
            Writeback(alu_result,destReg);
        }
       
    } else if (alu_ctrl == 6) {// beq , Rtype sub
        alu_result = operandA - operandB;
        alu_zero = (alu_result == 0) ? 1 : 0;
        if(Type_Instruction=="B"){
            is_branch_taken = (alu_zero == 1);
            total_clock_cycles++;
            return;
        }else{
            Writeback(alu_result,destReg);
        }
        
    } else if (alu_ctrl == 7) {//maybe not need
        alu_result = (operandA < operandB) ? 1 : 0;
        Writeback(alu_result,destReg);
    } else if (alu_ctrl == 12) {//maybe not need
        alu_result = ~(operandA | operandB);
        Writeback(alu_result,destReg);
    } else {
        cout << "Invalid ALU control signal!" << alu_ctrl<< endl;
    }
    if(alu_result ==0){
        alu_zero=1;
    }else{
        alu_zero=0;
    }
    //cout<< alu_result<<" and "<< destReg<<endl;
   
    
    
}
int Mem(string instructionType, int address, int valueToStore = 0) {
    int index = (address+offset) / 4 ;
    
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
}

void Writeback(int value, int destReg) {
    if (destReg != 0) {
        rf[destReg] = value;
    }
    total_clock_cycles++;
    
    
}
void Control_Unit(const std::string& opcode,const std::string& funct3, const std::string& funct7){
    RegWrite =0;
    Branch =0;
    MemRead=0;
    MemtoReg =0;
    MemWrite =0;
    AluSrc=0;
    AluOp=0;
    if(opcode == "0110011"){//Rtype
        RegWrite = 1;
        AluSrc = 0;
        MemtoReg = 0;
        MemRead = 0;
        MemWrite = 0;
        Branch = 0;
        AluOp = 2; 
        if (funct3 == "000" && funct7 == "0000000") alu_ctrl = 2; // ADD
        else if (funct3 == "000" && funct7 == "0100000") alu_ctrl = 6; // SUB
        else if (funct3 == "111") alu_ctrl = 0; // AND
        else if (funct3 == "110") alu_ctrl = 1; // OR
        else if (funct3 == "100") alu_ctrl = 3; // XOR
    }else if (opcode == "0010011"){//Itype
        RegWrite = 1;
        AluSrc = 1;
        MemtoReg = 0;
        MemRead = 0;
        MemWrite = 0;
        Branch = 0;
        AluOp = 0;
        if (funct3 == "000") alu_ctrl = 2; // ADDI
        else if (funct3 == "111") alu_ctrl = 0; // ANDI
        else if (funct3 == "110") alu_ctrl = 1; // ORI
    }else if (opcode == "0000011"){//lw
        RegWrite = 1;
        AluSrc = 1;
        MemtoReg = 1;
        MemRead = 1;
        MemWrite = 0;
        Branch = 0;
        AluOp = 0; 
        alu_ctrl = 2;
    }else if (opcode == "0100011"){//Stype
        RegWrite = 0;
        AluSrc = 1;
        MemtoReg = 0;
        MemRead = 0;
        MemWrite = 1;
        Branch = 0;
        AluOp = 0; 
        alu_ctrl = 2;
    }else if(opcode == "1100011"){//branch
        RegWrite = 0;
        AluSrc = 0;
        MemtoReg = 0;
        MemRead = 0;
        MemWrite = 0;
        Branch = 1;
        AluOp = 1; 
        alu_ctrl = 6; 
    }
}

int main() {
    fetch_file("sample_part1.txt");
    branch_target = 0;
    is_branch_taken = false;
    cout << "Enter the program file name to run:" << endl;
    rf[1]= 32;
    rf[2]= 5;
    rf[10]= 112;
    rf[11]=4;
    d_mem[28]=5;
    d_mem[29]=16;
    for (int i = 0; i <= instructions.size(); ++i) {
        fetch(instructions[i]);
        //make print function for all instruction
       
        if(alu_opcode == "0110011"){//Rtype
            cout <<"total_clock_cycles "<< total_clock_cycles <<" : " << endl;
            cout << "0x"<<rd_global <<" is modified to 0x"<< intToHex(rf[rd_global]) <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
        }else if (alu_opcode == "0010011"){//Itype
            cout <<"total_clock_cycles "<< total_clock_cycles <<" : " << endl;
            cout << "0x"<<rd_global <<" is modified to 0x"<< rs1_global <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
        }else if (alu_opcode == "0000011"){//lw
            cout <<"total_clock_cycles "<< total_clock_cycles <<" : " << endl;
            cout << "0x"<<rd_global <<" is modified to 0x"<< rs1_global <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
        }else if (alu_opcode == "0100011"){//Stype
            cout <<"total_clock_cycles "<< total_clock_cycles <<" : " << endl;
            cout << "memory 0x"<<rd_global <<" is modified to 0x"<< rs1_global <<endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
        }else if(alu_opcode == "1100011"){//branch
            cout <<"total_clock_cycles "<< total_clock_cycles <<" : " << endl;
            cout << "pc is modified to 0x"<< intToHex(PC) << endl;
        }
        
    }
    cout<< "lw x3, 4(x10)"<<endl;
    cout<< "sub x5, x1, x2 "<<endl;
    cout<< "beq x5, x3, 12 "<<endl;
    cout<< "add x5, x5, x3 "<<endl;
    cout<< "or x5, x11, x5 "<<endl;
    cout<< "sw x5, 0(x10) "<<endl;
    return 0;
}
