#include <iostream>
using namespace std;
#include <sstream>
#include <fstream>
#include <string>
int PC = 0;

int nextPC = 0;

vector<string> instructions;

vector<int> rf(32,0);

vector<int> D_mem(32,0);

string currInstructions;

int branch_target = 0;

bool is_branch_taken = false;

int rs1G = 0;
int rs2G = 0;
int rdG = 0;
int ImmG = 0;

int op1 = 0;
int op2 = 0;

string alu_ctrl;
int ctrl_sig = 0;

int alu_zero = 0;

string opcodeG;
string Funct7;
string Funct3;

int RegWrite = 0;
int branch = 0;
int aluSrc = 0;
int MemWrite = 0;
int MemToReg = 0;
int MemRead = 0;
int AluOp = 0;

string intToHex(int num) {
    stringstream ss;
    ss << hex << num;
    return ss.str();
}
string op(string binary) {
    opcodeG = binary.substr(25,7);
    return binary.substr(25,7);
}
string getFunct3(string binary) {
    Funct3 = binary.substr(17, 3);
    return binary.substr(17, 3);
}
string getFunct7(string binary) {
    Funct7 = binary.substr(0, 7);
    return binary.substr(0, 7);
}
int getRs1(string binary) {
    int rs1 = stoi(binary.substr(12, 5), nullptr, 2);
    return rs1;
}
int getRs2(string binary) {
    int rs2 = stoi(binary.substr(7, 5), nullptr, 2);
    return rs2;
}
int zerotofour(string binary) {
    int rd = stoi(binary.substr(7, 5), nullptr, 2);
    return rd;
}
int getRd(string binary) {
    int rd = stoi(binary.substr(20, 5), nullptr, 2);
    return rd;
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
        return (value + 1) * -1;
    }
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

void decode(string instruction) {
    string opcode = op(instruction);
    string funct3 = getFunct3(instruction);
    string funct7 = getFunct7(instruction);
    if (opcode == "0110011") {
        cout << "Instruction Type: R\n";
        if (funct3 == "000") {
            if (funct7 == "0000000") {
                cout << "Operation: add\n";

                rs1G = getRs1(instruction);
                rs2G = getRs2(instruction);
                rdG = getRd(instruction);
                op1 = rf[rs1G];
                op2 = rf[rs2G];
                alu_ctrl = "0010";
                
            } else if(funct7 == "0100000"){
                cout << "Operation: sub\n";

                rs1G = getRs1(instruction);
                rs2G = getRs2(instruction);
                rdG = getRd(instruction);
                op1 = rf[rs1G];
                op2 = rf[rs2G];
                alu_ctrl = "0110";
            }
        } else if (funct3 == "111") {
            if(funct7 == "0000000") {
                cout << "Operation: and\n";
                
            }
        } else if (funct3 == "110") {
            if(funct7 == "0000000") {
                cout << "Operation: or\n";
                
            }
        } else if (funct3 == "001") {
            if(funct7 == "0000000") {
                cout << "Operation: sll\n";
                
            }
        } else if (funct3 == "010") {
            if(funct7 == "0000000") {
                cout << "Operation: slt\n";
                
            }
        } else if (funct3 == "101") {
            if(funct7 == "0100000") {
                cout << "Operation: sra\n";
                
            } else if (funct7 == "0000000") {
                cout << "Operation: srl\n";
                
            }
        } else if (funct3 == "100") {
            if(funct7 == "0000000") {
                cout << "Operation: xor\n";
                
            }
        } else if (funct3 == "011") {
            if(funct7 == "0000000") {
                cout << "Operation: sltu\n";
            }
        }
    } else if (opcode == "0000011") {
        cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            cout << "Operation: lb\n";
        } else if (funct3 == "010") {
            cout << "Operation: lw\n";
        } else if (funct3 == "001") {
            cout << "Operation: lh\n";
        }
    } else if(opcode == "0010011") {
        cout << "Instruction Type: I\n";
        if(funct3 == "011") {
            cout << "Operation: sltiu\n";
           
        } else if(funct3 == "000") {
            cout << "Operation: addi\n";
           
        } else if(funct3 == "111") {
            cout << "Operation: andi\n";
           
        } else if(funct3 == "110") {
            cout << "Operation: ori\n";
           
        } else if(funct3 == "001") {
            if(funct7 == "0000000") {
                cout << "Operation: slli\n";
            } 
        } else if(funct3 == "010") {
            cout << "Operation: slti\n";
           
        } else if(funct3 == "101") {
            if(funct7 == "0100000") {
                cout << "Operation: srai\n";
            } else if (funct7 == "0000000") {
                cout << "Operation: srli\n";
            }
        } else if(funct3 == "100") {
            cout << "Operation: xori\n";
           
        }

    } else if (opcode == "1100111") {
        cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            cout << "Operation: jalr\n";

            rs1G = getRs1(instruction);
            rdG = getRd(instruction);
            ImmG = todecimalForI((instruction));
            op1 = rf[rs1G];
            op2 = ImmG;
            alu_ctrl = "0010";
        }

    } else if (opcode == "0100011") {
        cout << "Instruction Type: S\n";
        if(funct3 == "000") {
            cout << "Operation: sb\n";
        } else if(funct3 == "001") {
            cout << "Operation: sh\n";
        } else if(funct3 == "010") {
            cout << "Operation: sw\n";

            rs1G = getRs1(instruction);
            rs2G = getRs2(instruction);
            ImmG = getSTypeImm(instruction);
            op1 = rf[rs1G];
            op2 = ImmG;
            alu_ctrl = "0010";
        }
    }
    else if (opcode == "1100011") {
        cout << "Instruction Type: SB\n";
        if(funct3 == "000") {
            cout << "Operation: beq\n";
        } else if(funct3 == "001") {
            cout << "Operation: bne\n";
        } else if(funct3 == "100") {
            cout << "Operation: blt\n";
        } else if(funct3 == "101") {
            cout << "Operation: bge\n";
        }
    } else if (opcode == "1101111") {
        cout << "Instruction Type: UJ\n";
        cout << "Instruction: jal\n";

        rdG = getRd(instruction);
        ImmG = getUJTypeImm(instruction);
        op1 = PC;
        op2 = ImmG;
        alu_ctrl = "0010";
    }
    else {
        cout << "not valid\n";
    }
    controlUnit();
}

// Load all instructions from the file (do this once)
void loadFile(const string& filename) {
    ifstream file(filename);
    string line;
    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "'." << std::endl;
        return;
    }
    while (getline(file, line)) {
        instructions.push_back(line);
    }
    file.close();
}
void fetch() {
    currInstructions = instructions[PC/4];
    PC+=4;
    nextPC = PC;
}

void execute() {
    if(alu_ctrl == "0010") {
        ctrl_sig = op1 + op2;
    } else if(alu_ctrl == "0010") {
        ctrl_sig = op1 - op2;
    }
    if(ctrl_sig == 0) {
        alu_zero = 1;
        branch_target = PC + ImmG;
    } else {
        alu_zero = 0;
    }
}
void controlUnit() {
    if(opcodeG == "0110011") { // type R
        RegWrite = 1;
        branch = 0;
        aluSrc = 0;
        MemWrite = 0;
        MemToReg = 0;
        MemRead = 0;
        AluOp = 2; 
    } else if(opcodeG == "000010") { // lw
        RegWrite = 1;
        branch = 0;
        aluSrc = 1;
        MemWrite = 0;
        MemToReg = 1;
        MemRead = 1;
        AluOp = 0;
    } else if(opcodeG == "0100011") { // sw
        RegWrite = 0;
        branch = 0;
        aluSrc = 1;
        MemWrite = 1;
        MemToReg = 0;
        MemRead = 0;
        AluOp = 0;
    } else if(opcodeG == "1100111") { // jalr
        RegWrite = 1;
        branch = 0;
        aluSrc = 1;
        MemWrite = 0;
        MemToReg = 0;
        MemRead = 0;
        AluOp = 0;
    } else if(opcodeG == "1101111") { // jal
        RegWrite = 1;
        branch = 0;
        aluSrc = 0;
        MemWrite = 0;
        MemToReg = 0;
        MemRead = 0;
        AluOp = 0;
    }
    //ALU COntrol RIGHT HERE
    if(AluOp == 0) {
        alu_ctrl = "0010";
    } else if(AluOp == 1) {
        alu_ctrl = "0110";
    } else if(AluOp == 2) {
        if(Funct3 == "000") {
            if(Funct7.substr(1, 1) == "0") {
                alu_ctrl = "0010";
            } else {
                alu_ctrl = "0110";
            }
        } else if(Funct3 == "001") {
            alu_ctrl = "1001";
        } else if(Funct3 == "010") {
            alu_ctrl = "0111";
        } else if(Funct3 == "100") {
            alu_ctrl = "0100";
        } else if(Funct3 == "011") {
            alu_ctrl = "1111";
        } else if(Funct3 == "101") {
            if(Funct7.substr(1, 1) == "0") {
                alu_ctrl = "1010";
            } else {
                alu_ctrl = "1011";
            }
        } else if(Funct3 == "110") {
            alu_ctrl = "0001";
        } else if(Funct3 == "111") {
            alu_ctrl = "0000";
        }
    } else if(AluOp == 3) {
        if(Funct3 == "000") {
            if(Funct7.substr(1, 1) == "0") {
                alu_ctrl = "0010";
            } else {
                alu_ctrl = "0110";
            }
        } else if(Funct3 == "001") {
            alu_ctrl = "1001";
        } else if(Funct3 == "010") {
            alu_ctrl = "0111";
        } else if(Funct3 == "100") {
            alu_ctrl = "0100";
        } else if(Funct3 == "011") {
            alu_ctrl = "1111";
        } else if(Funct3 == "101") {
            if(Funct7.substr(1, 1) == "0") {
                alu_ctrl = "1010";
            } else {
                alu_ctrl = "1011";
            }
        } else if(Funct3 == "110") {
            alu_ctrl = "0001";
        } else if(Funct3 == "111") {
            alu_ctrl = "0000";
        }
    }

}
int main() {
    loadFile("sample_part2.txt");
    int i = 0;
    while(instructions.size() > i) {
        fetch();
        decode(currInstructions);

        i++;
    }
    
    return 0;
}