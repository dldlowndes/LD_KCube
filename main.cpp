#include "LD_ThorPSD.h"

#include <iostream>

int main(){
    LD_ThorPSD my_PSD("ttyUSB0");

    PSD_Data my_PSD_Data;
    while(true){
        my_PSD_Data = my_PSD.Get_PSD_Data();
        std::cout << my_PSD_Data.x_Diff << ", " << my_PSD_Data.y_Diff << ", " << my_PSD_Data.xy_Sum << "\n";
    }

    return 0;
}
