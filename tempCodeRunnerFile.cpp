std::vector<int> rf;
        std::string hold =instructions[1];
        for(int i =0; i<hold.size();i++){
            rf.push_back(hold[i]-'0');
        }
        for (const auto& number : rf) {
            std::cout << number << " ";
        }