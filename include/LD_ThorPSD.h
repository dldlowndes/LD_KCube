#ifndef LD_THORPSD_H
#define LD_THORPSD_H

#include <LD_KCube.h>

#include <string>

struct PSD_Data{
    int x_Diff;
    int y_Diff;
    int xy_Sum;
};

class LD_ThorPSD : public LD_KCube
{
    public:
        LD_ThorPSD();
        LD_ThorPSD(std::string comport_Name);
        ~LD_ThorPSD();

        int Get_Operation_Mode();
        int Set_Operation_Mode(uint8_t mode);

        PSD_Data Get_PSD_Data();

    private:
};

#endif // LD_THORPSD_H
