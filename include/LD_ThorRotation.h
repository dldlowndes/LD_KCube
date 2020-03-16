#ifndef LD_THORROTATION_H
#define LD_THORROTATION_H

#include "LD_KCube.h"

struct DDR_Status
{
    int position;
    int velocity;
    uint32_t status_Bits;
};

struct VelParams
{
    int min_Vel;
    int accel;
    int max_Vel;
};

struct JogParams
{
    int mode;
    int step_Size;
    int min_Vel;
    int accel;
    int max_Vel;
    int stop_Mode;
};

class LD_ThorRotation : public LD_KCube
{
    public:
        LD_ThorRotation();
        ~LD_ThorRotation();
        LD_ThorRotation(std::string comport_Name);

        int Get_Enable_State();
        int Set_Enable_State(int do_Enable);

        int Set_Home_Params(int direction,
                            int limit_Switch,
                            int home_Velocity,
                            int offset_Distance);

        // To be implemented
        int Get_Digital_Outputs();
        int Set_Digital_Outputs();
        int Get_Pos_Counter();
        int Set_Pos_Counter();
        int Get_Enc_Counter();
        int Set_Enc_Counter();
        int Get_Vel_Params();
        int Set_Vel_Params();
        int Get_Jog_Params();
        int Set_Jog_Params();
        int Get_Gen_Move_Params();
        int Set_Gen_Move_Params();
        int Get_Move_Rel_Params();
        int Set_Move_Rel_Params();
        int Get_Move_Abs_Params();
        int Set_Move_Abs_Params();
        int Get_Home_Params();
        int Set_Home_Params();

        int Get_DC_Status_Update();
        int Set_DC_Status_Update();
        int Get_Status_Bits();

        int Move_Home();
        int Move_Relative();
        int Move_Completed();
        int Move_Absolute();
        int Move_Jog();
        int Move_Velocity();
        int Move_Stop();
        int Move_Stopped();

//        int Get_Limit_Switch_Params();
//        int Set_Limit_Switch_Params();
//        int Get_Position_Loop_Params();
//        int Set_Position_Loop_Params();
//        int Get_Motor_Output_Params();
//        int Set_Motor_Output_Params();
//        int Get_Track_Settle_Params();
//        int Set_Track_Settle_Params();
//        int Get_Profile_Mode_Params();
//        int Set_Profile_Mode_Params();
//        int Get_Joystick_Params();
//        int Set_Joystick_Params();
//        int Get_Current_Loop_Params();
//        int Set_Current_Loop_Params();
//        int Get_Settled_Current_Loop_Params();
//        int Set_Settled_Current_Loop_Params();
//        int Get_Stage_Axis_Params();
//        int Set_Stage_Axis_Params();
//        int Set_Trigger();
//        int Req_Trigger();

    private:
};

#endif // LD_THORROTATION_H
