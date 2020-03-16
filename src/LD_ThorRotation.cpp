#include "LD_ThorRotation.h"

#include <iostream>

LD_ThorRotation::LD_ThorRotation(){
}

LD_ThorRotation::~LD_ThorRotation(){
}

LD_ThorRotation::LD_ThorRotation(std::string comport_Name){
    LD_ThorRotation();
    Connect(comport_Name);
}

int LD_ThorRotation::Get_Enable_State(){
    std::vector<uint8_t> command{0x11, 0x02, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x12, 0x02};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 6, 100, 1000);

    bool is_Enabled = -1;
    if ((response[0] == expected[0]) & (response[1] == expected[1])){
        if(response[3] == 0x01){
            is_Enabled = 1;
        }
        if(response[3] == 0x02){
            is_Enabled = 0;
        }
    }
    else{
        std::cout << "Unexpected response, unknown enable state\n";
    }

    return is_Enabled;
}

int LD_ThorRotation::Set_Enable_State(int do_Enable){
    std::vector<uint8_t> command{0x10, 0x02, 0x01, 0x00, 0x50, 0x01};
    if(do_Enable == 1){
        command[3] = 0x01;
    }
    else if (do_Enable == 0){
        command[3] = 0x02;
    }
    else{
        std::cout << "Invalid request\n";
    }
    SendCommand(command);

    // Verify.
    int enable_State = Get_Enable_State();
    std::cout << "Enable, asked for " << do_Enable << " got " << enable_State << std::endl;
    return enable_State;
}

int LD_ThorRotation::Set_Home_Params(int direction,
                               int limit_Switch,
                               int home_Velocity,
                               int offset_Distance){
std::vector<uint8_t> command{0x40, 0x04, 0x0E, 0x00, 0xD0, 0x01,
                             0x01, 0x00,
                             0x00, 0x00,
                             0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00};

}
