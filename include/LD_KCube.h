#ifndef LD_KCUBE_H
#define LD_KCUBE_H

#include <cstring>
#include <string>
#include <vector>

struct HWInfo
{
    long serial_Number;
    std::string model_Number;
    short hw_Type;
    long fw_Version;
    short hw_Version;
    short mod_State;
    short num_Chans;
};

class LD_KCube
{
    public:
        // Constructor that doesn't need to know the comport name to
        // instantiate. (Obviously needs to be set later with
        // Connect(comport_Name).
        LD_KCube();
        // If the comport name is known at creation it can be given to
        // the constructor (this just runs the normal constructor and
        // then runs Connect(comport_Name).
        //LD_KCube(std::string comport_Name);
        // Will do any required destructing but also calls Close() to
        // handle the hardware
        ~LD_KCube();

        // Connect to the specified comport.
        int Connect_Serial(std::string comport_Name);
        // Disconnect the comport and any other housekeeping that might
        // be reqired on shutdown (like
        int Close();

        // General functions that all K Cubes will respond to, identifying
        // them in various ways.
        // Loads of boring info about the device.
        HWInfo Get_HWInfo();
        // Serial number prefix identifies the class of device (see APT manual)
        int Get_SerialNo_Prefix();
        // Flashes the display on the K Cube so you can (physically) see
        // which one you're talking to.
        int Identify();

        // Section of methods that are in the manual's "Generic" section
        // but I don't have the hardware to test the functionality described.
        //Get_Rack_Bay_Used(uint8_t bay_ID);
        //Get_Hub_Bay_Used(uint8_t bay_ID);

    protected:
        // Do all the set up for the device.
        int Init();
        // Send a command to the K Cube over serial. It should be created as
        // per the documentation in the APT manual.
        int SendCommand(std::vector<uint8_t> command);
        // Get a response from the K cube. The response vector is cleared and
        // fills with the data from the K cube (or zeros on fail). Each command
        // has a specified response length so that must be specifed.
        // Wait_ms is a wait between attempts to read the port (until the number
        // of characters is read or timeout). Timeout_ms is how many ms to wait
        // until giving up on the response. (i.e something probably went wrong).
        int RecvResponse(std::vector<uint8_t> &response, int num_Chars, int wait_ms=1, int timeout_ms=100);
        // Each message has a header and then (optionally) a payload. Obviously the
        // payload is only valid if the header is as expected. Pass in the message
        // and expected header and check they match.
        bool CheckResponse(std::vector<uint8_t> response, std::vector<uint8_t> expected_Header);

        int ser_Ret;
        int m_Comport_Number;
        HWInfo m_HWInfo;

};

// Some of the returned data from the K Cubes is strings so construct a string
// from the relevant range of bytes.
inline std::string Buf_To_String(std::vector<uint8_t>buffer, int start_Byte, int length){
    return std::string(&buffer[start_Byte], &buffer[start_Byte + length]);
}

// Copy some bytes from a K Cube "buffer" (starting at "start_Byte" into a value of some type
// "value_Out". The manual specifies the type of return from each function. The number of bytes
// from the buffer is determined by the size of "T".
template<class T>
void Buf_To(std::vector<uint8_t> buffer, int start_Byte, T& value_Out){
    value_Out = *((T*)&buffer[start_Byte]);
}

// Copy a value "value_In" into a byte vector "buffer" (that will presumably be sent
// to a K Cube), number of bytes copied is determined by the size of "T"
template<class T>
void To_Buf(T value_In, std::vector<uint8_t>&buffer, int start_Byte){
    memcpy(&buffer[start_Byte], &value_In, sizeof(T));
}

#endif // LD_KCUBE_H
