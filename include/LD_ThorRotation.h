#ifndef LD_THORROTATION_H
#define LD_THORROTATION_H

#include "LD_KCube.h"

struct Home_Params{
    bool direction_Forwards;
    bool limit_Switch_Forwards;
    double velocity;
    double offset_Distance;
};

struct Device_Scale_Factors{
    double degrees;
    double velocity;
    double acceleration;
};

struct VelParams
{
    double min_Vel = 0;
    double accel;
    double max_Vel;
};

struct JogParams
{
    bool continuous_Mode;
    double step_Size;
    VelParams velocity;
    bool do_Profiled_Stop;
};

struct Limit_Sw_Params
{
    uint16_t cw_Hard_Limit;
    uint16_t ccw_Hard_Limit;
    double cw_Soft_Limit;
    double ccw_Soft_Limit;
    uint16_t sw_Limit_Mode;
};

struct DDR_Status
{
    double position;
    double velocity;
    uint32_t status_Bits;
};


class LD_ThorRotation : public LD_KCube
{
    public:
        LD_ThorRotation();
        ~LD_ThorRotation();
        LD_ThorRotation(std::string comport_Name);

        Device_Scale_Factors Get_Device_Scale_Factors();

        int Get_Enable_State();
        int Set_Enable_State(int do_Enable);

        double Get_Pos_Counter();
        int Set_Pos_Counter(double position_Degs);
        double Get_Enc_Counter();
        int Set_Enc_Counter(double position_Degs);


        DDR_Status Get_StatusUpdate();
        bool Get_Is_Moving();

        Home_Params Get_Home_Params();
        int Set_Home_Params(Home_Params params);
        VelParams Get_Vel_Params();
        int Set_Vel_Params(VelParams params);
        JogParams Get_Jog_Params();
        int Set_Jog_Params(JogParams params);
        double Get_Gen_Move_Params();
        int Set_Gen_Move_Params(double backlash);
        Limit_Sw_Params Get_Limit_Switch_Params();
        //int Set_Limit_Switch_Params(Limit_Sw_Params params);
        int Set_Default_Params();

        int Move_Home();
        int Move_Relative(double move_Distance, bool block);
        int Move_Absolute(double move_Distance, bool block);
        int Move_Jog(bool forwards);
        int Move_Velocity(VelParams params, bool forwards);
        int Move_Velocity(bool forwards);
        int Move_Stop(bool do_Profiled_Stop);

        // To be implemented
//        int Get_Digital_Outputs();
//        int Set_Digital_Outputs();
//        int Get_DC_Status_Update();
//        int Set_DC_Status_Update();
//        int Get_Status_Bits();

        // Ignore these?
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
        int Move(int32_t distance_DUs, bool relative, bool block);


        Device_Scale_Factors scale_Factors{4000, 26843.5, 2.74878};

        template <class T>
        void Degrees_To_Device(double degrees, T &device){
            device = degrees * scale_Factors.degrees;
        }
        template <class T>
        void Velocity_To_Device(double velocity, T &device){
            device = velocity * scale_Factors.velocity;
        }
        template <class T>
        void  Acceleration_To_Device(double acceleration, T &device){
            device = acceleration * scale_Factors.acceleration;
        }

        template <class T>
        void  Degrees_From_Device(T device, double &degrees){
            degrees = (double)device / scale_Factors.degrees;
        }

        template <class T>
        void   Velocity_From_Device(T device, double &velocity){
            velocity = (double)device / scale_Factors.velocity;
        }

        template <class T>
        void   Acceleration_From_Device(T device, double &acceleration){
            acceleration = (double)device / scale_Factors.acceleration;
        }
};

#endif // LD_THORROTATION_H
