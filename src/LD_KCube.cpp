#include "LD_KCube.h"

#include <iostream>

#include "rs232.h"
#include "LD_Util.h"

LD_KCube::LD_KCube(){
}

LD_KCube::~LD_KCube(){
}

LD_KCube::LD_KCube(std::string comport_Name)
{
    LD_KCube();
    Connect(comport_Name);
}

int LD_KCube::Connect(std::string comport_Name){
    m_Comport_Number = RS232_GetPortnr(comport_Name.c_str());
    //std::cout << comport_Name << " is " << m_Comport_Number << "\n";

    ser_Ret = RS232_OpenComport(m_Comport_Number, 115200, "8n1");
    if (ser_Ret == 0){
        std::cout << "Connected to " << comport_Name << " (number " << m_Comport_Number << ")\n";
    }
    else{
        std::cout << "Connect fail." << std::endl;
    }
    //std::cout << "Flushhh\n";
    RS232_flushRXTX(m_Comport_Number);
    MySleep(100);

    Init();

    return 0;
}

int LD_KCube::Close(){
    RS232_flushRXTX(m_Comport_Number);
    MySleep(500);
    RS232_CloseComport(m_Comport_Number);
    MySleep(100);

    return ser_Ret;
}

int LD_KCube::Init(){
    m_HWInfo = Get_HWInfo();
    std::cout << "HWInfo output\n";
    std::cout << "\tSerial No: " << (int)m_HWInfo.serial_Number << "\n";
    std::cout << "\tModel No: " << m_HWInfo.model_Number << "\n";
    std::cout << "\tHW Type: " << (int)m_HWInfo.hw_Type << "\n";
    std::cout << "\tFW Version: " << (int)m_HWInfo.fw_Version << "\n";
    std::cout << "\tHW Version: " << (int)m_HWInfo.hw_Version << "\n";
    std::cout << "\tMod state: " << (int)m_HWInfo.mod_State << "\n";
    std::cout << "\tNum Chans: " << (int)m_HWInfo.num_Chans << "\n";

    return 0;
}

int LD_KCube::SendCommand(std::vector<uint8_t> command){
    // Flush the RX buffer in case there's leftovers in there.
    RS232_flushRX(m_Comport_Number);

    // Send the command.
    ser_Ret = RS232_SendBuf(m_Comport_Number, command.data(), command.size());

    // Ensure the data has left the buffer.
    RS232_flushTX(m_Comport_Number);

    return ser_Ret;
}

int LD_KCube::RecvResponse(std::vector<uint8_t> &response, int num_Chars, int wait_ms, int timeout_ms){
    // Start with a clean slate.
    response.clear();
    // So reserve doens't have to reallocate during this loop.
    response.reserve(num_Chars);

    int time_Waited = 0;
    int this_Bytes_Recvd = 0;
    int total_Bytes_Recvd = 0;

    // Each read goes into a buffer (that's plenty big enough for anything we'll receive)
    // and this will get emptied out into the response vector after each comport polling.
    unsigned char my_Buf[255];

    while(true){
        // Try the comport
        this_Bytes_Recvd = RS232_PollComport(m_Comport_Number, my_Buf, num_Chars);

        // Put any bytes received into the buffer.
        for(int i = 0; i < this_Bytes_Recvd; i++){
            response.push_back(my_Buf[i]);
        }

        // Check whether the loop should end yet.
        total_Bytes_Recvd += this_Bytes_Recvd;
        if(total_Bytes_Recvd < num_Chars){
            // Haven't read enough bytes yet.
            if (time_Waited > timeout_ms){
                // Timeout exceeded.
                response.resize(num_Chars);
                break;
            }
            // Have a sleep (and record for how long)
            MySleep(wait_ms);
            time_Waited += wait_ms;
        }
        else{
            // Enough bytes read.
            break;
        }
    }

    return response.size();
}

bool LD_KCube::CheckResponse(std::vector<uint8_t> response, std::vector<uint8_t> expected_Header){
    bool header_Good = std::equal(response.begin(), response.begin()+6, expected_Header.begin());
    return header_Good;
}

HWInfo LD_KCube::Get_HWInfo(){
    //std::cout << "Get HWInfo\n";
    std::vector<uint8_t> command = {0x05, 0x00, 0x00, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected = {0x06, 0x00, 0x54, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 90, 5, 500);

    if (CheckResponse(response, expected)){
        Buf_To(response, 6, m_HWInfo.serial_Number);


        m_HWInfo.model_Number = Buf_To_String(response, 10, 8);
        Buf_To(response, 18, m_HWInfo.hw_Type);
        Buf_To(response, 20, m_HWInfo.fw_Version);
        Buf_To(response, 84, m_HWInfo.hw_Version);
        Buf_To(response, 86, m_HWInfo.mod_State);
        Buf_To(response, 88, m_HWInfo.num_Chans);
    }
    return m_HWInfo;

}

int LD_KCube::Get_SerialNo_Prefix(){
    // The first two digits of the serial number identify the type of device.
    return m_HWInfo.serial_Number / 1000000;
}

int LD_KCube::Identify(){
    // Instruct hardware unit to identify itself (by flashing its front panel LEDs).
    std::vector<uint8_t> command{0x23, 0x02, 0x01, 0x00, 0x50, 0x01};
    SendCommand(command);

    return 0;
}

// Functions superceded by template in header
//short Buf_To_Short(std::vector<uint8_t>buffer, int start_Byte){
//    return *((short*)&buffer[start_Byte]);
//}
//
//uint16_t Buf_To_Uint16(std::vector<uint8_t>buffer, int start_Byte){
//    return *((uint16_t*)&buffer[start_Byte]);
//}
//
//long Buf_To_Long(std::vector<uint8_t>buffer, int start_Byte){
//    return *((long*)&buffer[start_Byte]);
//}
//
//uint32_t Buf_To_Uint32(std::vector<uint8_t>buffer, int start_Byte){
//    return *((uint32_t*)&buffer[start_Byte]);
//}
//
//long long Buf_To_LongLong(std::vector<uint8_t>buffer, int start_Byte){
//    return *((long long*)&buffer[start_Byte]);
//}
//
//uint64_t Buf_To_Uint64(std::vector<uint8_t>buffer, int start_Byte){
//    return *((uint64_t*)&buffer[start_Byte]);
//}
