#ifndef LD_THORPSD_H
#define LD_THORPSD_H

#include <LD_KCube.h>

#include <string>
#include <list>

struct PSD_Data{
    int16_t x_Diff;
    int16_t y_Diff;
    uint16_t xy_Sum;
};

class LD_ThorPSD : public LD_KCube
{
    public:
        LD_ThorPSD();
        LD_ThorPSD(std::string comport_Name);
        ~LD_ThorPSD();

        // The modes are 1:monitor, 2:openloop, 3:closedloop, 4:"auto"(see manual)
        // This code is mainly intended for open loop operation.
        int Get_Operation_Mode();
        int Set_Operation_Mode(uint8_t mode);

        // Read the X, Y and Sum values from the PSD.
        PSD_Data Get_PSD_Data();

    private:

};

#endif // LD_THORPSD_H
