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
int operandA = 10;
int operandB = 5;
int alu_result = 0;
const int DMEM_SIZE = 32;
int d_mem[DMEM_SIZE] = {0};
string intToHex(int num) {
    stringstream ss;
    ss << hex << num;
    return ss.str();
}
string op(string binary) {
    return binary.substr(25,7);
}
string getFunct3(string binary) {
    return binary.substr(17, 3);
}
string getFunct7(string binary) {
    return binary.substr(0, 7);
}
string getRs1(string binary) {
    int rs1_num = stoi(binary.substr(12, 5), nullptr, 2);
    rf[rs1_num]= rs1_num;
    int rs1 = rf[rs1_num];
    return "x" + to_string(rs1);
}
string getRs2(string binary) {
    int rs2_num = stoi(binary.substr(7, 5), nullptr, 2);
    rf[rs2_num]= rs2_num;
    int rs2 =rf[rs2_num];
    return "x" + to_string(rs2);
}
int zerotofour(string binary) {
    int rd_num = stoi(binary.substr(7, 5), nullptr, 2);
    rf[rd_num]= rd_num;
    int rd = rf[rd_num];
    return rd;
}
string getRd(string binary) {
    int rd_num = stoi(binary.substr(20, 5), nullptr, 2);
    rf[rd_num]= rd_num;
    int rd = rf[rd_num];
    return "x" + to_string(rd);
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


void printR(string instruction, string funct3, string funct7) {
    cout << "Rs1: " << getRs1(instruction) << endl;
    cout << "Rs2: " << getRs2(instruction) << endl;
    cout << "Rd: " << getRd(instruction) << endl;
    cout << "Funct3: " << todecimal(funct3) << endl;
    cout << "Funct7: " << todecimal(funct7) << endl;
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
                printR(instruction,funct3,funct7);
            } else if(funct7 == "0100000"){
                cout << "Operation: sub\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "111") {
            if(funct7 == "0000000") {
                cout << "Operation: and\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "110") {
            if(funct7 == "0000000") {
                cout << "Operation: or\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "001") {
            if(funct7 == "0000000") {
                cout << "Operation: sll\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "010") {
            if(funct7 == "0000000") {
                cout << "Operation: slt\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "101") {
            if(funct7 == "0100000") {
                cout << "Operation: sra\n";
                printR(instruction,funct3,funct7);
            } else if (funct7 == "0000000") {
                cout << "Operation: srl\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "100") {
            if(funct7 == "0000000") {
                cout << "Operation: xor\n";
                printR(instruction,funct3,funct7);
            }
        } else if (funct3 == "011") {
            if(funct7 == "0000000") {
                cout << "Operation: sltu\n";
                printR(instruction,funct3,funct7);
            }
        }
    } else if (opcode == "0000011") {
        cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            cout << "Operation: lb\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        } else if (funct3 == "010") {
            cout << "Operation: lw\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        } else if (funct3 == "001") {
            cout << "Operation: lh\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        }
    } else if(opcode == "0010011") {
        cout << "Instruction Type: I\n";
        if(funct3 == "011") {
            cout << "Operation: sltiu\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        } else if(funct3 == "000") {
            cout << "Operation: addi\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        } else if(funct3 == "111") {
            cout << "Operation: andi\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        } else if(funct3 == "110") {
            cout << "Operation: ori\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        } else if(funct3 == "001") {
            if(funct7 == "0000000") {
                cout << "Operation: slli\n";
                cout << "Rs1: " << getRs1(instruction) << endl;
                cout << "Rd: " << getRd(instruction) << endl;
                cout << "Immediate: " << zerotofour((instruction)) << " (or 0x" << intToHex(zerotofour((instruction))) << ")" << endl;
            } 
        } else if(funct3 == "010") {
            cout << "Operation: slti\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        } else if(funct3 == "101") {
            if(funct7 == "0100000") {
                cout << "Operation: srai\n";
                cout << "Rs1: " << getRs1(instruction) << endl;
                cout << "Rd: " << getRd(instruction) << endl;
                cout << "Immediate: " << zerotofour((instruction)) << " (or 0x" << intToHex(zerotofour((instruction))) << ")" << endl;
            } else if (funct7 == "0000000") {
                cout << "Operation: srli\n";
                cout << "Rs1: " << getRs1(instruction) << endl;
                cout << "Rd: " << getRd(instruction) << endl;
                cout << "Immediate: " << zerotofour((instruction)) << " (or 0x" << intToHex(zerotofour((instruction))) << ")" << endl;
            }
        } else if(funct3 == "100") {
            cout << "Operation: xori\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        }

    } else if (opcode == "1100111") {
        cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            cout << "Operation: jalr\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rd: " << getRd(instruction) << endl;
            cout << "Immediate: " << todecimalForI((instruction)) << " (or 0x" << intToHex(todecimalForI((instruction))) << ")" << endl;
        }

    } else if (opcode == "0100011") {
        cout << "Instruction Type: S\n";
        if(funct3 == "000") {
            cout << "Operation: sb\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rs2: " << getRs2(instruction) << endl;
            cout << "Immediate: " << getSTypeImm(instruction) << " (or 0x" << intToHex(getSTypeImm(instruction)) << ")" << endl;
        } else if(funct3 == "001") {
            cout << "Operation: sh\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rs2: " << getRs2(instruction) << endl;
            cout << "Immediate: " << getSTypeImm(instruction) << " (or 0x" << intToHex(getSTypeImm(instruction)) << ")" << endl;
        } else if(funct3 == "010") {
            cout << "Operation: sw\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rs2: " << getRs2(instruction) << endl;
            cout << "Immediate: " << getSTypeImm(instruction) << " (or 0x" << intToHex(getSTypeImm(instruction)) << ")" << endl;
        }
    }
    else if (opcode == "1100011") {
        cout << "Instruction Type: SB\n";
        if(funct3 == "000") {
            cout << "Operation: beq\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rs2: " << getRs2(instruction) << endl;
            cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
        } else if(funct3 == "001") {
            cout << "Operation: bne\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rs2: " << getRs2(instruction) << endl;
            cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
        } else if(funct3 == "100") {
            cout << "Operation: blt\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rs2: " << getRs2(instruction) << endl;
            cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
        } else if(funct3 == "101") {
            cout << "Operation: bge\n";
            cout << "Rs1: " << getRs1(instruction) << endl;
            cout << "Rs2: " << getRs2(instruction) << endl;
            cout << "Immediate: " << (getSBTypeImm(instruction)) << " (or 0x" << intToHex(getSBTypeImm(instruction)) << ")" << endl;
        }
    } else if (opcode == "1101111") {
        cout << "Instruction Type: UJ\n";
        cout << "Operation: jal\n";
        cout << "Rd: " << getRd(instruction) << endl;
        cout << "Immediate: " << getUJTypeImm(instruction) << " (or 0x" << intToHex(getUJTypeImm(instruction)) << ")" << endl;
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
void fetch() {
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
        cout << "Fetched instruction at PC = 0x" << intToHex(PC) << ": " << instructions[PC / 4] << endl;
        decode(instructions[1]);
    } else {
        cout << "PC out of range: 0x" << intToHex(PC) << ". No instruction to fetch." << endl;
    }
}
void Execute(string alu_ctrl) {
    if (alu_ctrl == "0000") {
        alu_result = operandA & operandB;
    } else if (alu_ctrl == "0001") {
        alu_result = operandA | operandB;
    } else if (alu_ctrl == "0010") {
        alu_result = operandA + operandB;
    } else if (alu_ctrl == "0110") {
        alu_result = operandA - operandB;
    } else if (alu_ctrl == "0111") {
        alu_result = (operandA < operandB) ? 1 : 0;
    } else if (alu_ctrl == "1100") {
        alu_result = ~(operandA | operandB);
    } else {
        cout << "Invalid ALU control signal!" << endl;
    }
}
int Mem(string instructionType, int address, int valueToStore = 0) {
    int index = address / 4;

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

int main() {
    fetch_file("sample_part1.txt");
    branch_target = 0;
    is_branch_taken = false;
    for (int i = 0; i < 1; ++i) {
        fetch();
    }
    return 0;
}