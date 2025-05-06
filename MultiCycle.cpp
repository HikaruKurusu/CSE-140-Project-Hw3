#include <iostream>
using namespace std;
#include <sstream>
#include <fstream>
#include <string>

struct IF_ID {
    bool fetch_bool = false;
    string instruction;
    int PC;
    int nextPC;
} if_id;

struct ID_EX {
    string instruction;
    bool decode_bool = false;
    string Funct3,Funct7;
    int rs1_val, rs2_val, imm, rd;
    int AluOp;
    string alu_ctrl;
    string opcode;
    int PC;
    int nextPC;
    int reg_write, branch,aluSrc,mem_write, mem_to_reg,mem_read,jal_sig,jalr_sig;
} id_ex;

struct EX_MEM {
    string instruction;
    bool exe_bool = false;
    string Funct3,Funct7;
    int alu_result,rs1_val, rs2_val,imm ,rd;
    int AluOp;
    string alu_ctrl;
    string opcode;
    int PC;
    int nextPC;
    int alu_zero;
    int branch_target;
    int reg_write, branch,aluSrc,mem_write, mem_to_reg,mem_read,jal_sig,jalr_sig;
} ex_mem;

struct MEM_WB {
    string instruction;
    bool mem_bool = false;
    string Funct3,Funct7;
    int alu_result,rs1_val, rs2_val,imm ,rd;
    int AluOp;
    string alu_ctrl;
    string opcode;
    int PC;
    int nextPC;
    int alu_zero;
    int branch_target;
    int reg_write, branch,aluSrc,mem_write, mem_to_reg,mem_read,jal_sig,jalr_sig;
    int recieved;
    
} mem_wb;

int PC = 0;

int nextPC = 0;

vector<string> instructions;

vector<int> rf(32,0);

vector<int> D_mem(32,0);

string currInstructions;

int branch_target = 0;
int alu_result = 0;
bool is_branch_taken = false;
int rs1G = 0;
int rs2G = 0;
int rdG = 0;
int ImmG =0;
int op1 = 0;
int op2 = 0;

string alu_ctrl;
int ctrl_sig = 0;

int alu_zero = 0;

string opcodeG;
string Funct7;
string Funct3;

int reg_write = 0;
int branch = 0;
int aluSrc = 0;
// int ex_mem.mem_write = 0;
// int ex_mem.mem_to_reg = 0;
// int ex_mem.mem_read = 0;
int AluOp = 0;
int jal_sig = 0;
int jalr_sig =0;
bool stall = false;


int recieved = 0;

int total_clock_cycles = 0;
void controlUnit();
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
        // cout << "Instruction Type: R\n";
        if (funct3 == "000") {
            if (funct7 == "0000000") {
                // cout << "Operation: add\n";

                id_ex.rs1_val = getRs1(instruction);
                id_ex.rs2_val = getRs2(instruction);
                id_ex.rd = getRd(instruction);
                op1 = rf[id_ex.rs1_val];
                op2 = rf[id_ex.rs2_val];
                alu_ctrl = "0010";
                
            } else if(funct7 == "0100000"){
                // cout << "Operation: sub\n";

                id_ex.rs1_val = getRs1(instruction);
                id_ex.rs2_val = getRs2(instruction);
                id_ex.rd = getRd(instruction);
                op1 = rf[id_ex.rs1_val];
                op2 = rf[id_ex.rs2_val];
                alu_ctrl = "0110";
            }
        } else if (funct3 == "111") {
            if(funct7 == "0000000") {
                // cout << "Operation: and\n";
                id_ex.rs1_val = getRs1(instruction);
                id_ex.rs2_val = getRs2(instruction);
                id_ex.rd = getRd(instruction);
                op1 = rf[id_ex.rs1_val];
              
                op2 = rf[id_ex.rs2_val];
                alu_ctrl = "0000";
            }
        } else if (funct3 == "110") {
            if(funct7 == "0000000") {
                // cout << "Operation: or\n";
                id_ex.rs1_val = getRs1(instruction);
                id_ex.rs2_val = getRs2(instruction);
                id_ex.rd = getRd(instruction);
                op1 = rf[id_ex.rs1_val];
                op2 = rf[id_ex.rs2_val];
                alu_ctrl = "0001";
            }
        } else if (funct3 == "001") {
            if(funct7 == "0000000") {
                // cout << "Operation: sll\n";
                
            }
        } else if (funct3 == "010") {
            if(funct7 == "0000000") {
                // cout << "Operation: slt\n";
                
            }
        } else if (funct3 == "101") {
            if(funct7 == "0100000") {
                // cout << "Operation: sra\n";
                
            } else if (funct7 == "0000000") {
                // cout << "Operation: srl\n";
                
            }
        } else if (funct3 == "100") {
            if(funct7 == "0000000") {
                // cout << "Operation: xor\n";
                
            }
        } else if (funct3 == "011") {
            if(funct7 == "0000000") {
                // cout << "Operation: sltu\n";
            }
        }
    } else if (opcode == "0000011") {
        // cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            // cout << "Operation: lb\n";
        } else if (funct3 == "010") {
            // cout << "Operation: lw\n";
            id_ex.rs1_val = getRs1(instruction);
            id_ex.imm = todecimalForI((instruction));
            
            id_ex.rd = getRd(instruction);
            op1 = rf[id_ex.rs1_val];
            op2 = id_ex.imm;
            alu_ctrl = "0010";
        } else if (funct3 == "001") {
            // cout << "Operation: lh\n";
        }
    } else if(opcode == "0010011") {
        // cout << "Instruction Type: I\n";
        if(funct3 == "011") {
            // cout << "Operation: sltiu\n";
           
        } else if(funct3 == "000") {
            // cout << "Operation: addi\n";
            id_ex.rs1_val = getRs1(instruction);
            id_ex.rd = getRd(instruction);
            id_ex.imm = todecimalForI((instruction));
            op1 = rf[id_ex.rs1_val];
            op2 = id_ex.imm;
            alu_ctrl="0010";
        } else if(funct3 == "111") {
            // cout << "Operation: andi\n";
           id_ex.rs1_val = getRs1(instruction);
           id_ex.rd = getRd(instruction);
           id_ex.imm = todecimalForI((instruction));
           op1 = rf[id_ex.rs1_val];
           op2 = id_ex.imm;
           alu_ctrl="0000";
        } else if(funct3 == "110") {
            // cout << "Operation: ori\n";
           id_ex.rs1_val = getRs1(instruction);
           id_ex.rd = getRd(instruction);
           id_ex.imm = todecimalForI((instruction));
           op1 = rf[id_ex.rs1_val];
           op2 = id_ex.imm;
           alu_ctrl="0001";
        } else if(funct3 == "001") {
            if(funct7 == "0000000") {
                // cout << "Operation: slli\n";
            } 
        } else if(funct3 == "010") {
            // cout << "Operation: slti\n";
           
        } else if(funct3 == "101") {
            if(funct7 == "0100000") {
                // cout << "Operation: srai\n";
            } else if (funct7 == "0000000") {
                // cout << "Operation: srli\n";
            }
        } else if(funct3 == "100") {
            // cout << "Operation: xori\n";
           
        }

    } else if (opcode == "1100111") {
        // cout << "Instruction Type: I\n";
        if(funct3 == "000") {
            // cout << "Operation: jalr\n";

            id_ex.rs1_val = getRs1(instruction);
            id_ex.rd = getRd(instruction);
            id_ex.imm = todecimalForI((instruction));
            op1 = rf[id_ex.rs1_val];
            op2 = id_ex.imm;
            alu_ctrl = "0010";
        }

    } else if (opcode == "0100011") {
        // cout << "Instruction Type: S\n";
        if(funct3 == "000") {
            // cout << "Operation: sb\n";
        } else if(funct3 == "001") {
            // cout << "Operation: sh\n";
        } else if(funct3 == "010") {
            // cout << "Operation: sw\n";

            id_ex.rs1_val = getRs1(instruction);
            id_ex.rs2_val = getRs2(instruction);
            id_ex.imm = getSTypeImm(instruction);
            op1 = rf[id_ex.rs1_val];
            op2 = id_ex.imm;
            alu_ctrl = "0010";
        }
    }
    else if (opcode == "1100011") {
        // cout << "Instruction Type: SB\n";
        if(funct3 == "000") {
            // cout << "Operation: beq\n";
            id_ex.rs1_val = getRs1(instruction);
            id_ex.rs2_val = getRs2(instruction);
            id_ex.imm = getSBTypeImm(instruction);
            op1 = rf[id_ex.rs1_val];
            op2 = rf[id_ex.rs2_val];
            alu_ctrl = "0110";
        } else if(funct3 == "001") {
            // cout << "Operation: bne\n";

        } else if(funct3 == "100") {
            // cout << "Operation: blt\n";
        } else if(funct3 == "101") {
            // cout << "Operation: bge\n";
        }
    } else if (opcode == "1101111") {
        // cout << "Instruction Type: UJ\n";
        // cout << "Instruction: jal\n";

        id_ex.rd = getRd(instruction);
        id_ex.imm = getUJTypeImm(instruction);
        op1 = PC;
        op2 = id_ex.imm;
        alu_ctrl = "0010";
    }
    else {
        cout << "not valid\n";
    }
    
    controlUnit();
    id_ex.instruction = if_id.instruction;
    
    id_ex.alu_ctrl = alu_ctrl;
    
    id_ex.Funct3 =  Funct3;
    id_ex.Funct7 =  Funct7;
  
    id_ex.opcode =  opcodeG;
    id_ex.PC= if_id.PC;
    id_ex.nextPC= if_id.nextPC;
}

// Load all instructions from the file (do this once)
void loadFile(const string& filename) {
    cout<< filename <<endl;
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
    currInstructions = instructions[if_id.PC/4];
    if_id.PC+=4;
    if_id.nextPC = if_id.PC;
    if_id.instruction = currInstructions;
     
}

void execute() {
   
    if(id_ex.aluSrc == 0){
        op2 = rf[id_ex.rs2_val];

    }else if (id_ex.aluSrc == 1){
        op2 = id_ex.imm;
    }
    op1 = rf[id_ex.rs1_val];
   
    if(id_ex.alu_ctrl == "0010") {//lw add sw
        ex_mem.alu_result = op1 + op2;
       //cout << ex_mem.alu_result<<" "<< op2<< " " << op1<<endl;
       
    } else if(id_ex.alu_ctrl == "0110") {//sub beq
        ex_mem.alu_result = op1 - op2;
        
    }else if (id_ex.alu_ctrl == "0000") {//and
        ex_mem.alu_result = op1 & op2;
    }   else if (id_ex.alu_ctrl == "0001") {//or
        ex_mem.alu_result = op1 | op2;
    }     
    if( ex_mem.alu_result == 0) {
        ex_mem.alu_zero = 1;
        ex_mem.branch_target = id_ex.PC + id_ex.imm;
    } else {
        ex_mem.alu_zero = 0;
    }
    ex_mem.instruction = id_ex.instruction;
    ex_mem.instruction = id_ex.instruction;
    ex_mem.rs1_val =  id_ex.rs1_val;
    ex_mem.rs2_val =  id_ex.rs2_val;
    ex_mem.imm =  id_ex.imm;
    ex_mem.Funct3 =  id_ex.Funct3;
    ex_mem.Funct7 =  id_ex.Funct7;
    ex_mem.rd =  id_ex.rd;
    ex_mem.AluOp =  id_ex.AluOp;
    ex_mem.opcode =  id_ex.opcode;
    ex_mem.alu_ctrl =  id_ex.alu_ctrl;
    ex_mem.reg_write =  id_ex.reg_write;
    ex_mem.branch =  id_ex.branch;
    ex_mem.aluSrc =  id_ex.aluSrc;
    ex_mem.mem_write =  id_ex.mem_write;
    ex_mem.mem_to_reg =  id_ex.mem_to_reg;
    ex_mem.mem_read =  id_ex.mem_read;
    ex_mem.jal_sig =  id_ex.jal_sig;
    ex_mem.jalr_sig =  id_ex.jalr_sig;
    ex_mem.PC= id_ex.PC;
    ex_mem.nextPC= id_ex.nextPC;
}
void controlUnit() {
    if(opcodeG == "0110011") { // type R
        id_ex.reg_write = 1;
        id_ex.branch = 0;
        id_ex.aluSrc = 0;
        id_ex.mem_write = 0;
        id_ex.mem_to_reg = 0;
        id_ex.mem_read = 0;
        id_ex.AluOp = 2; 
        id_ex.jal_sig = 0;
        id_ex.jalr_sig = 0;
    } else if(opcodeG == "0000011") { // lw
        id_ex.reg_write = 1;
        id_ex.branch = 0;
        id_ex.aluSrc = 1;
        id_ex.mem_write = 0;
        id_ex.mem_to_reg = 1;
        id_ex.mem_read = 1;
        id_ex.AluOp = 0;
        id_ex.jal_sig = 0;
        id_ex.jalr_sig = 0;
    } else if(opcodeG == "0100011") { // sw
        id_ex.reg_write = 0;
        id_ex.branch = 0;
        id_ex.aluSrc = 1;
        id_ex.mem_write = 1;
        id_ex.mem_to_reg = 0;
        id_ex.mem_read = 0;
        id_ex.AluOp = 0;
        id_ex.jal_sig = 0;
        id_ex.jalr_sig = 0;
    } else if(opcodeG == "1100111") { // jalr
        id_ex.reg_write = 1;
        id_ex.branch = 0;
        id_ex.aluSrc = 1;
        id_ex.mem_write = 0;
        id_ex.mem_to_reg = 0;
        id_ex.mem_read = 0;
        id_ex.AluOp = 0;
        id_ex.jal_sig = 1;
        id_ex.jalr_sig =1;
        

    } else if(opcodeG == "1101111") { // jal
        id_ex.reg_write = 1;
        id_ex.branch = 0;
        id_ex.aluSrc = 0;
        id_ex.mem_write = 0;
        id_ex.mem_to_reg = 0;
        id_ex.mem_read = 0;
        id_ex.AluOp = 0;
        id_ex.jal_sig = 1;
        id_ex.jalr_sig = 0;
    }else if(opcodeG == "0010011"){//I type
        id_ex.reg_write = 1;
        id_ex.branch = 0;
        id_ex.aluSrc = 1;
        id_ex.mem_write = 0;
        id_ex.mem_to_reg = 0;
        id_ex.mem_read = 0;
        id_ex.AluOp = 0;
        id_ex.jal_sig = 0;
        id_ex.jalr_sig = 0;
    }else if(opcodeG == "1100011"){//beq
        id_ex.reg_write = 0;
        id_ex.branch = 1;
        id_ex.aluSrc = 0;
        id_ex.mem_write = 0;
        id_ex.mem_to_reg = 0;
        id_ex.mem_read = 0;
        id_ex.AluOp = 0;
        id_ex.jal_sig = 0;
        id_ex.jalr_sig = 0;
    }
    //ALU COntrol RIGHT HERE
    if(id_ex.AluOp == 0) {
        id_ex.alu_ctrl = "0010";
    } else if(id_ex.AluOp == 1) {
        id_ex.alu_ctrl = "0110";
    } else if(id_ex.AluOp == 2) {
        if(id_ex.Funct3 == "000") {
            if(id_ex.Funct7.substr(1, 1) == "0") {
                id_ex.alu_ctrl = "0010";
            } else {
                id_ex.alu_ctrl = "0110";
            }
        } else if(id_ex.Funct3 == "001") {
            id_ex.alu_ctrl = "1001";
        } else if(id_ex.Funct3 == "010") {
            id_ex.alu_ctrl = "0111";
        } else if(id_ex.Funct3 == "100") {
            id_ex.alu_ctrl = "0100";
        } else if(id_ex.Funct3 == "011") {
            id_ex.alu_ctrl = "1111";
        } else if(id_ex.Funct3 == "101") {
            if(id_ex.Funct7.substr(1, 1) == "0") {
                id_ex.alu_ctrl = "1010";
            } else {
                id_ex.alu_ctrl = "1011";
            }
        } else if(id_ex.Funct3 == "110") {
            id_ex.alu_ctrl = "0001";
        } else if(id_ex.Funct3 == "111") {
            id_ex.alu_ctrl = "0000";
        }
    } else if(id_ex.AluOp == 3) {
        if(id_ex.Funct3 == "000") {
            if(id_ex.Funct7.substr(1, 1) == "0") {
                id_ex.alu_ctrl = "0010";
            } else {
                id_ex.alu_ctrl = "0110";
            }
        } else if(id_ex.Funct3 == "001") {
            id_ex.alu_ctrl = "1001";
        } else if(id_ex.Funct3 == "010") {
            id_ex.alu_ctrl = "0111";
        } else if(Funct3 == "100") {
            id_ex.alu_ctrl = "0100";
        } else if(Funct3 == "011") {
            id_ex.alu_ctrl = "1111";
        } else if(id_ex.Funct3 == "101") {
            if(id_ex.Funct7.substr(1, 1) == "0") {
                id_ex.alu_ctrl = "1010";
            } else {
                id_ex.alu_ctrl = "1011";
            }
        } else if(id_ex.Funct3 == "110") {
            id_ex.alu_ctrl = "0001";
        } else if(Funct3 == "111") {
            id_ex.alu_ctrl = "0000";
        }
    }

}
int Mem(){

    int index =( ex_mem.alu_result)/4;
    if(ex_mem.mem_read == 1 ){
        recieved = D_mem[index];

    }else if (ex_mem.mem_write == 1){
        D_mem[index]=  rf[ex_mem.rs2_val];
        
    } 
    mem_wb.instruction = ex_mem.instruction;
    mem_wb.recieved = recieved;
    mem_wb.PC = ex_mem.PC;
    mem_wb.nextPC = ex_mem.nextPC;
    mem_wb.mem_write = ex_mem.mem_write;
    mem_wb.reg_write = ex_mem.reg_write;
    mem_wb.alu_result = ex_mem.alu_result;
    mem_wb.rd = ex_mem.rd;
    mem_wb.alu_zero = ex_mem.alu_zero;
    mem_wb.branch_target = ex_mem.branch_target;
    mem_wb.mem_read = ex_mem.mem_read;
    mem_wb.jal_sig = ex_mem.jal_sig;
    mem_wb.jalr_sig = ex_mem.jalr_sig;
    mem_wb.imm =ex_mem.imm;

}
int Writeback(){
    
    if(mem_wb.reg_write == 1){
        
        if(mem_wb.mem_read == 1){
            rf[mem_wb.rd] = mem_wb.recieved;
          
        }else{
            if(mem_wb.jal_sig == 1){
             
                if(mem_wb.jalr_sig == 1){
                    mem_wb.PC = rf[mem_wb.rs1_val]+mem_wb.imm;
                }else{
                    
                    mem_wb.PC = mem_wb.PC + mem_wb.imm - 4 ;
                    
                }
                rf[mem_wb.rd] = mem_wb.nextPC;
            }else{
                
                rf[mem_wb.rd] =  mem_wb.alu_result;
            }
            
        }
    }
    
  
} 
int main() {
    //sample_part1.txt
    rf[1]=0x20;
    rf[2]=0x5;
    rf[10]=0x70;
    rf[11]=0x4;
    D_mem[28]=0x5;
    D_mem[29]=0x10;
    //sample_part2.txt 
    // rf[8]=0x20;
    // rf[10]=0x5;
    // rf[11]=0x2;
    // rf[12]=0xa;
    // rf[13]=0xf;
    cout<< "Enter the program file name to run"<<endl;
    cout<<endl;
    loadFile("sample_part1.txt");
    
    while(if_id.fetch_bool || id_ex.decode_bool || ex_mem.exe_bool || mem_wb.mem_bool || mem_wb.mem_bool || (if_id.PC/4 < instructions.size())) {
        // fetch();
        // decode(currInstructions);
        // execute();
        // Mem();
        // Writeback();
       
        total_clock_cycles++;
        cout<<"total_clock_cycles "<< total_clock_cycles<<" :" <<endl;
       
        
        if(mem_wb.mem_bool == true){
          
            
            Writeback();
            mem_wb.mem_bool = false;
            cout<< "WB: " << mem_wb.instruction  <<endl;
            if( mem_wb.mem_write == 1 ){
            
                cout<<"memory 0x"<<intToHex( mem_wb.alu_result) << " is modified to 0x"<< intToHex(D_mem[( mem_wb.alu_result)/4])<<endl;
            }
            if(mem_wb.reg_write==1){
                cout<<"x"<<mem_wb.rd << " is modified to 0x"<< intToHex(rf[mem_wb.rd])<<endl;
            }
          
            cout<< "pc is modified to 0x"<< intToHex(mem_wb.PC)<<endl;
            cout<<endl;
            
        }
        
        if(ex_mem.exe_bool == true){
           
            Mem();
            cout<< "MEM: " << mem_wb.instruction  <<endl;
            cout<< "pc is modified to 0x"<< intToHex(mem_wb.PC)<<endl;
            cout<<endl;
            mem_wb.mem_bool = true;
            ex_mem.exe_bool = false;
        }
        
        if(id_ex.decode_bool == true){
            if((mem_wb.mem_bool && (id_ex.rs1_val == mem_wb.rd || id_ex.rs2_val == mem_wb.rd)) || (ex_mem.exe_bool && (id_ex.rs1_val == ex_mem.rd || id_ex.rs2_val == ex_mem.rd))) {
                stall = true;
                continue;
            }else{
                execute();
                cout<< "EXE: " << id_ex.instruction  <<endl;
                cout<< "pc is modified to 0x"<< intToHex(id_ex.PC)<<endl;
                cout<<endl;
                ex_mem.exe_bool= true;
                id_ex.decode_bool = false;
                stall =false;
            }
           
            
        }

        if(stall == false){
            if(if_id.fetch_bool == true){
            
                decode(if_id.instruction);
                cout<< "ID: " << if_id.instruction  <<endl;
                cout<< "pc is modified to 0x"<< intToHex(if_id.PC)<<endl;
                cout<<endl;
                id_ex.decode_bool = true;
                if_id.fetch_bool =false;
            }
            if(if_id.PC/4 < instructions.size()){
               
                fetch();
                cout<< "IF: " << if_id.instruction  <<endl;
                cout<< "pc is modified to 0x"<< intToHex(if_id.PC)<<endl;
                cout<<endl;
                if_id.fetch_bool = true;
                
            }
        }

        
        
    }
    
    return 0;
}