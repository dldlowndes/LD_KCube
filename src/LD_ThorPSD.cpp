#include "LD_ThorPSD.h"

#include <iostream>
#include <vector>

LD_ThorPSD::LD_ThorPSD(){
}

LD_ThorPSD::LD_ThorPSD(std::string comport_Name){
    LD_ThorPSD();

    Connect(comport_Name);

    Set_Operation_Mode(0x02);
}

LD_ThorPSD::~LD_ThorPSD(){
}

int LD_ThorPSD::Get_Operation_Mode(){
    std::vector<uint8_t> command{0x71, 0x08, 0x07, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x70, 0x08, 0x08, 0x00, 0x50, 0x01};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 10, 100, 1000);

    if(CheckResponse(command, expected)){
        int mode = response[8];
        std::cout << "Op mode is " << std::hex << (int)mode << std::dec << "\n";
        return mode;
    }
    else{
        return -1;
    }
}

int LD_ThorPSD::Set_Operation_Mode(uint8_t mode){
    std::cout << "Set operation mode to " << std::hex << (int)mode << std::dec << "\n";
    std::vector<uint8_t> command{0x70, 0x08, 0x04, 0x00, 0xD0, 0x01,
                                 0x07, 0x00,
                                 mode, 0x00};
    SendCommand(command);

    std::cout << "Check Operation mode set properly\n";
    int mode_Check = Get_Operation_Mode();

    if(mode_Check != mode){
        std::cout << "Error, mode not set correctly" << std::endl;
        return 1;
    }
    return 0;
}

PSD_Data LD_ThorPSD::Get_PSD_Data(){
    std::vector<uint8_t> command{0x71, 0x08, 0x03, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x72, 0x08, 0x0C, 0x00, 0x50, 0x01};
    std::vector<uint8_t> response;
    SendCommand(command);
    RecvResponse(response, 18, 0.1, 1000);

    PSD_Data my_PSD_Data{0, 0, 0};

    if(CheckResponse(command, expected)){
        Buf_To(response, 8, my_PSD_Data.x_Diff);
        Buf_To(response, 10, my_PSD_Data.y_Diff);
        Buf_To(response, 12, my_PSD_Data.xy_Sum);
    }

    return my_PSD_Data;
}

