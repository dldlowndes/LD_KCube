#include "LD_ThorPSD.h"
#include "LD_Timer.h"
#include "LD_ThorRotation.h"
#include "LD_Util.h"

#include <fstream>
#include <iostream>
#include <list>

int PSD_Example(){
    LD_ThorPSD my_PSD("ttyUSB0");

    PSD_Data my_PSD_Data;
    int i = 0;
    int num_Loops = 100;
    LD_Timer my_Timer;
    my_Timer.Start_Timer();
    std::list<PSD_Data> my_List;
    while(i < num_Loops){
        my_PSD_Data = my_PSD.Get_PSD_Data();
        std::cout << my_PSD_Data.x_Diff << ", " << my_PSD_Data.y_Diff << ", " << my_PSD_Data.xy_Sum << "\n";
        my_List.push_back(my_PSD_Data);
        i++;
    }
    my_Timer.Stop_Timer();
    int time = my_Timer.Get_Last_Time_Difference();
    std::cout << "Acquisition took " << time << "\n";
    std::cout << "Average step time " << (double)time / (double)num_Loops << "\n";

    std::ofstream file_Out;
    file_Out.open("test.csv");
    file_Out << "X, Y, Sum\n";
    for(auto x : my_List){
        file_Out << x.x_Diff << ", " << x.y_Diff << ", " << x.xy_Sum << "\n";
    }
    file_Out.close();
    return 0;
}

int Rot_Example(){
    LD_ThorRotation my_DDR("ttyUSB0");

    my_DDR.Move_Home();



    return 0;
}

int main(){
    Rot_Example();

    return 0;
}
