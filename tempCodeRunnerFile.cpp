        if(if_id.fetch_bool == true){
            if_id.fetch_bool = false;
            
        }else{
            fetch();
            cout<< "IF: " << if_id.instruction  <<endl;
            cout<< "pc is modified to 0x"<< intToHex(PC)<<endl;
            if_id.fetch_bool = true;
            
        }
        if(id_ex.decode_bool == true){
            id_ex.decode_bool = false;
            
        }else{
            decode(if_id.instruction);
            cout<< "ID: " << if_id.instruction  <<endl;
            cout<< "pc is modified to 0x"<< intToHex(PC)<<endl;
            id_ex.decode_bool = true;
        }
        if(ex_mem.exe_bool == true){
            ex_mem.exe_bool = false;
            
        }else{
            execute();
            cout<< "EXE: " << if_id.instruction  <<endl;
            cout<< "pc is modified to 0x"<< intToHex(PC)<<endl;
            ex_mem.exe_bool = true;
        }
        if(mem_wb.mem_bool == true){
            mem_wb.mem_bool  = false;
            
        }else{
            Mem();
            cout<< "MEM: " << if_id.instruction  <<endl;
            cout<< "pc is modified to 0x"<< intToHex(PC)<<endl;
            mem_wb.mem_bool = true;
        }
        if(mem_wb.wb_bool == true){
            mem_wb.wb_bool  = false;
            
        }else{
            Writeback();
            cout<< "WB: " << if_id.instruction  <<endl;
            if( ex_mem.mem_write == 1 ){
            
                cout<<"memory 0x"<<intToHex(ctrl_sig) << " is modified to 0x"<< intToHex(D_mem[(ctrl_sig)/4])<<endl;
            }
            if(ex_mem.reg_write==1){
                cout<<"x"<<id_ex.rd << " is modified to 0x"<< intToHex(rf[id_ex.rd])<<endl;
            }
          
            cout<< "pc is modified to 0x"<< intToHex(PC)<<endl;
            cout<<endl;
            
        }