#include "LD_ThorRotation.h"

#include "LD_Util.h"

#include <iostream>

LD_ThorRotation::LD_ThorRotation(){
    std::cout << "I'm a rotation stage" << std::endl;
}

LD_ThorRotation::~LD_ThorRotation(){
    std::cout << "Rotation stage says bye" << std::endl;
    //Move_Stop(true);
}

LD_ThorRotation::LD_ThorRotation(std::string comport_Name, double origin_Angle){
    Init(comport_Name, origin_Angle);
}

int LD_ThorRotation::Init(std::string comport_Name, double origin_Angle){
    m_Origin_Angle = origin_Angle;

    Connect_Serial(comport_Name);

    Set_Default_Params();

    // Enable the device.
    Set_Enable_State(true);

    Move_Home(false);

    MySleep(500);

    Move_Absolute(0, true);

    return 0;
}

int LD_ThorRotation::Set_Default_Params(){
    std::cout << "Set params to default" << std::endl;

    // Set the parameters to something sensible.
    Home_Params default_Home_Params;
    default_Home_Params.direction_Forwards = true;
    default_Home_Params.limit_Switch_Forwards = false;
    default_Home_Params.velocity = 450;
    default_Home_Params.offset_Distance = 0;
    Set_Home_Params(default_Home_Params);

    VelParams default_Vel_Params;
    default_Vel_Params.min_Vel = 0;
    default_Vel_Params.accel = 1200;
    default_Vel_Params.max_Vel = 1200;
    Set_Vel_Params(default_Vel_Params);

    JogParams default_Jog_Params;
    default_Jog_Params.continuous_Mode = true;
    default_Jog_Params.step_Size = 10;
    default_Jog_Params.velocity = default_Vel_Params;
    default_Jog_Params.do_Profiled_Stop = false;
    Set_Jog_Params(default_Jog_Params);

//    Limit_Sw_Params default_Limit_Params;
//    default_Limit_Params.cw_Hard_Limit = 0x04;
//    default_Limit_Params.ccw_Hard_Limit = 0x01;
//    default_Limit_Params.cw_Soft_Limit = 0;
//    default_Limit_Params.ccw_Soft_Limit = 0;
//    default_Limit_Params.sw_Limit_Mode = 0x01;
//    Set_Limit_Switch_Params(default_Limit_Params);
    return 0;
}

Device_Scale_Factors LD_ThorRotation::Get_Device_Scale_Factors(){
    return scale_Factors;
}

int LD_ThorRotation::Get_Enable_State(){
    std::vector<uint8_t> command{0x11, 0x02, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x12, 0x02};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 6, 100, 1000);

    CheckResponse(response, expected);

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
        std::cout << std::hex;
        for(auto x: response){
            std::cout << (int)x << ", ";
        }
        std::cout << "\n";
        for(auto x: expected){
            std::cout << (int)x << ", ";
        }
        std::cout << "\n";
        std::cout << std::dec;
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

double LD_ThorRotation::Get_Pos_Counter(){
    std::vector<uint8_t> command{0x11, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x12, 0x04, 0x06, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 12);

    int32_t position_DUs;
    double position_Degs;
    if(CheckResponse(response, expected)){
        Buf_To(response, 8, position_DUs);
        Degrees_From_Device(position_DUs, position_Degs);
    }
    else{
        std::cout << "Pos counter fail\n";
        position_Degs = 0;
    }
    return position_Degs;
}

int LD_ThorRotation::Set_Pos_Counter(double position_Degs){
    std::vector<uint8_t> command{0x10, 0x04, 0x06, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00, 0x00, 0x00};
    int32_t position_DUs;
    Degrees_To_Device(position_Degs, position_DUs);
    To_Buf(position_DUs, command, 8);

    SendCommand(command);
    return 0;
}

double LD_ThorRotation::Get_Enc_Counter(){
    std::vector<uint8_t> command{0x0A, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x0B, 0x04, 0x06, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 12);

    int32_t position_DUs;
    double position_Degs;
    if(CheckResponse(response, expected)){
        Buf_To(response, 8, position_DUs);
        Degrees_From_Device(position_DUs, position_Degs);
    }
    else{
        std::cout << "Pos counter fail\n";
        position_Degs = 0;
    }
    return position_Degs;
}

int LD_ThorRotation::Set_Enc_Counter(double position_Degs){
    std::vector<uint8_t> command{0x09, 0x04, 0x06, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00, 0x00, 0x00};
    int32_t position_DUs;
    Degrees_To_Device(position_Degs, position_DUs);
    To_Buf(position_DUs, command, 8);

    SendCommand(command);
    return 0;
}

Home_Params LD_ThorRotation::Get_Home_Params(){
    std::vector<uint8_t> command = {0x41, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected = {0x42, 0x04, 0x0e, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 20);

    Home_Params params{0,0,0,0};
    uint16_t direction;
    uint16_t limit_Switch;
    int32_t velocity;
    int32_t offset;

    if(CheckResponse(response, expected)){
        Buf_To(response, 8, direction);
        if(direction == 1){
            params.direction_Forwards = true;
        }
        else{
            params.direction_Forwards = false;
        }
        Buf_To(response, 10, limit_Switch);
        params.limit_Switch_Forwards = (limit_Switch==1 ? true : false);
        if (limit_Switch == 1){
            params.limit_Switch_Forwards = true;
        }
        else{
            params.limit_Switch_Forwards = false;
        }
        Buf_To(response, 12, velocity);
        Velocity_From_Device(velocity, params.velocity);
        Buf_To(response, 16, offset);
        Degrees_From_Device(offset, params.offset_Distance);
    }

    std::cout << "Home params are:\n";
    std::cout << "\tDirection: " << params.direction_Forwards << "\n";
    std::cout << "\tLimit Switch: " << params.limit_Switch_Forwards << "\n";
    std::cout << "\tVelocity: " << params.velocity << "\n";
    std::cout << "\tOffset:: " << params.offset_Distance << "\n";

    return params;
}

int LD_ThorRotation::Set_Home_Params(Home_Params params){
    std::vector<uint8_t> command{0x40, 0x04, 0x0E, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00,
                                 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00};
    // Convert the params into device units.
    uint16_t direction;
    uint16_t limit_Switch;
    int32_t velocity;
    int32_t offset;
    if(params.direction_Forwards){
        direction = 1;
    }
    else{
        direction = 2;
    }
    if(params.limit_Switch_Forwards){
        limit_Switch = 1;
    }
    else{
        limit_Switch = 4;
    }
    Velocity_To_Device(params.velocity, velocity);
    if (params.offset_Distance != 0){
        std::cout << "Warning: homing position offset doesn't work right now\n"
                  << "setting offset to zero\n";
        params.offset_Distance = 0;
    }
    Degrees_To_Device(params.offset_Distance, offset);

    // Copy the params into the buffer
    To_Buf(direction, command, 8);
    To_Buf(limit_Switch, command, 10);
    To_Buf(velocity, command, 12);
    To_Buf(offset, command, 16);

    std::cout << "Set home params\n";
    SendCommand(command);

    // Check the params were set right.
    Home_Params actual_Params = Get_Home_Params();
    if(
        (params.direction_Forwards == actual_Params.direction_Forwards) &
        (params.limit_Switch_Forwards== actual_Params.limit_Switch_Forwards) &
        (params.offset_Distance == actual_Params.offset_Distance) &
        (params.velocity == actual_Params.velocity)
    ){
        return 0;
    }
    else{
        std::cout << "Home params not set properly\n";
        std::cout << "Direction: " << params.direction_Forwards << ", " << actual_Params.direction_Forwards << "\n";
        std::cout << "Limit Sw: " << params.limit_Switch_Forwards << ", " << actual_Params.limit_Switch_Forwards << "\n";
        std::cout << "Offset dist: " << params.offset_Distance << ", " << actual_Params.offset_Distance << "\n";
        std::cout << "Velocity: " << params.velocity << ", " << actual_Params.velocity << "\n";
        return 1;
    }
}

VelParams LD_ThorRotation::Get_Vel_Params(){
    std::vector<uint8_t> command{0x14, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x15, 0x04, 0x06, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 20);

    VelParams params;
    uint32_t min_Vel = 0;
    uint32_t acceleration = 0;
    uint32_t max_Vel = 0;
    if (CheckResponse(response, expected)){
        Buf_To(response, 8, min_Vel);
        Buf_To(response, 12, acceleration);
        Buf_To(response, 16, max_Vel);
        Velocity_From_Device(min_Vel, params.min_Vel);
        Acceleration_From_Device(acceleration, params.accel);
        Velocity_From_Device(max_Vel, params.max_Vel);
    }
    else{
        std::cout << "get vel params fail\n";
    }

    //TODO: Do something with this.
    Get_Vel_Params();

    return params;
}

int LD_ThorRotation::Set_Vel_Params(VelParams params){
    std::vector<uint8_t> command{0x13, 0x04, 0x0E, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00};
    uint32_t min_Vel;
    uint32_t acceleration;
    uint32_t max_Vel;

    Velocity_To_Device(params.min_Vel, min_Vel);
    Acceleration_To_Device(params.accel, acceleration);
    Velocity_To_Device(params.max_Vel, max_Vel);

    To_Buf(min_Vel, command, 8);
    To_Buf(acceleration, command, 12);
    To_Buf(max_Vel, command, 16);

    std::cout << "Set vel params\n";
    SendCommand(command);
    return 0;
}


JogParams LD_ThorRotation::Get_Jog_Params(){
    std::vector<uint8_t> command{0x17, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x18, 0x04, 0x16, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 28);

    uint16_t j_Mode;
    uint32_t step;
    uint32_t min_Vel;
    uint32_t accel;
    uint32_t max_Vel;
    uint16_t s_Mode;

    Buf_To(response, 8, j_Mode);
    Buf_To(response, 10, step);
    Buf_To(response, 14, min_Vel);
    Buf_To(response, 18, accel);
    Buf_To(response, 22, max_Vel);
    Buf_To(response, 26, s_Mode);

    JogParams params;
    if (CheckResponse(response, expected)){
        if (j_Mode == 1){params.continuous_Mode = true;}
        else{params.continuous_Mode = false;}
        Degrees_From_Device(step, params.step_Size);
        Velocity_From_Device(min_Vel, params.velocity.min_Vel);
        Acceleration_From_Device(accel, params.velocity.accel);
        Velocity_From_Device(max_Vel, params.velocity.max_Vel);
        if (s_Mode == 2){params.do_Profiled_Stop = true;}
        else{params.do_Profiled_Stop = false;}
    }
    else{
        std::cout << "Get Jog params fail\n";
    }

    // TODO: Do something with this.
    Get_Jog_Params();

    return params;
}

int LD_ThorRotation::Set_Jog_Params(JogParams params){
    std::vector<uint8_t> command{0x16, 0x04, 0x16, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00};
    uint16_t j_Mode;
    uint32_t step;
    uint32_t min_Vel;
    uint32_t accel;
    uint32_t max_Vel;
    uint16_t s_Mode;

    if (params.continuous_Mode){j_Mode = 1;}
    else{j_Mode = 2;}
    Degrees_To_Device(params.step_Size, step);
    Velocity_To_Device(params.velocity.min_Vel, min_Vel);
    Acceleration_To_Device(params.velocity.accel, accel);
    Velocity_To_Device(params.velocity.max_Vel, max_Vel);
    if(params.do_Profiled_Stop){s_Mode = 2;}
    else{s_Mode = 1;}

    To_Buf(j_Mode, command, 8);
    To_Buf(step, command, 10);
    To_Buf(min_Vel, command, 14);
    To_Buf(accel, command, 18);
    To_Buf(max_Vel, command, 22);
    To_Buf(s_Mode, command, 26);

    std::cout << "Set jog params\n";
    SendCommand(command);
    return 0;
}

double LD_ThorRotation::Get_Gen_Move_Params(){
    std::vector<uint8_t> command{0x3b, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x3c, 0x04, 0x06, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 12);

    uint32_t backlash_DUs;
    double backlash_Degs = 0;
    if(CheckResponse(response, expected)){
        Buf_To(response, 8, backlash_DUs);
        Degrees_From_Device(backlash_DUs, backlash_Degs);
    }
    else{
        std::cout << "get gen params fail\n";
    }
    return backlash_Degs;
}

int LD_ThorRotation::Set_Gen_Move_Params(double backlash){
    std::vector<uint8_t> command{0x3a, 0x04, 0x06, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00, 0x00, 0x00};

    uint32_t backlash_DUs;
    Degrees_To_Device(backlash, backlash_DUs);
    To_Buf(backlash_DUs, command, 8);

    std::cout << "Set gen move params\n";
    SendCommand(command);

    // TODO: Do something with this.
    Get_Gen_Move_Params();

    return 0;
}

Limit_Sw_Params LD_ThorRotation::Get_Limit_Switch_Params(){
    std::vector<uint8_t> command{0x24, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x25, 0x04, 0x10, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 20);

    uint32_t cw_Soft;
    uint32_t ccw_Soft;
    Limit_Sw_Params params;

    if (CheckResponse(response, expected)){
        Buf_To(response, 8, params.cw_Hard_Limit);
        Buf_To(response, 10, params.ccw_Hard_Limit);
        Buf_To(response, 12, cw_Soft);
        Buf_To(response, 16, ccw_Soft);
        Buf_To(response, 20, params.sw_Limit_Mode);

        Degrees_From_Device(cw_Soft, params.cw_Soft_Limit);
        Degrees_From_Device(ccw_Soft, params.ccw_Soft_Limit);
    }
    else{
        std::cout << "Get Limit params fail\n";
    }

    return params;
}

/*
// This function doesn't work right now.
int LD_ThorRotation::Set_Limit_Switch_Params(Limit_Sw_Params params){
    std::vector<uint8_t> command{0x23, 0x04, 0x10, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00};
    uint32_t cw_Soft;
    uint32_t ccw_Soft;

    Degrees_To_Device(params.cw_Soft_Limit, cw_Soft);
    Degrees_To_Device(params.ccw_Soft_Limit, ccw_Soft);

    To_Buf(params.cw_Hard_Limit, command, 8);
    To_Buf(params.ccw_Hard_Limit, command, 10);
    To_Buf(cw_Soft, command, 12);
    To_Buf(ccw_Soft, command, 16);
    To_Buf(params.sw_Limit_Mode, command, 20);

    std::cout << "Set lim sw params\n";
    SendCommand(command);

    // TODO: Do something with this
    Get_Limit_Switch_Params();

    return 0;
}
*/

int LD_ThorRotation::Move_Home(bool force){
    std::vector<uint8_t> command{0x43, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x44, 0x04, 0x01, 0x00, 0x01, 0x50};
    std::vector<uint8_t> response;

    DDR_Status status = Get_StatusUpdate();
    bool homed_Already = status.status_Bits & 0x00000400;
    if (homed_Already & !force){
        std::cout << "Homed already. Not moving\n";
        return 0;
    }

    std::cout << "Move home\n";
    SendCommand(command);

    // Wait a while for the homed response.
    RecvResponse(response, 6, 50, 10000);

    if (CheckResponse(response, expected)){
        std::cout << "Homed successfully\n";
        return 0;
    }
    else{
        std::cout << "Homing failed/timeout\n";
        return 1;
    }

    return 0;
}

DDR_Status LD_ThorRotation::Get_StatusUpdate(){
    std::vector<uint8_t> command{0x90, 0x04, 0x01, 0x00, 0x50, 0x01};
    std::vector<uint8_t> expected{0x91, 0x04, 0x0e, 0x00, 0x80, 0x50};
    std::vector<uint8_t> response;

    SendCommand(command);
    RecvResponse(response, 20);

    uint32_t position;
    uint16_t velocity;
    DDR_Status my_Status;

    Buf_To(response, 8, position);
    Buf_To(response, 12, velocity);
    Buf_To(response, 16, my_Status.status_Bits);
    Degrees_From_Device(position, my_Status.position);
    Velocity_From_Device(velocity, my_Status.velocity);

    return my_Status;

}

bool LD_ThorRotation::Get_Is_Moving(){
    std::cout << "Stage still moving? ";
    DDR_Status my_Status = Get_StatusUpdate();
    bool is_In_Motion = ((my_Status.status_Bits & 0x00000010) != 0) |
                         (my_Status.status_Bits == 0x00000200) |
                         (my_Status.status_Bits == 0x00001000);
    std::cout << is_In_Motion << "\n";
    return is_In_Motion;
}

int LD_ThorRotation::Move(int32_t distance_DUs, bool relative, bool block){
    std::vector<uint8_t> command{0x53, 0x04, 0x06, 0x00, 0xD0, 0x01,
                                 0x01, 0x00,
                                 0x00, 0x00, 0x00, 0x00};
    if (relative){
        command[0] = 0x48;
    }

    To_Buf(distance_DUs, command, 8);
    std::cout << "Moving" << std::endl;
    SendCommand(command);

    // If requested (block=true), wait until the device has stopped reporting
    // that it is moving. i.e. the requested movement is completed.
    // For now there's a hard coded timeout in here just to stop it hanging
    // if something weird happens.
    int timeout_Tries = 100; // TODO: Put this somewhere settable.
    int i = 0;
    if(block){
        do{
            MySleep(100);
            i++;
        }while(Get_Is_Moving() && (i < timeout_Tries));

    }

    return 0;
}

int LD_ThorRotation::Move_Relative(double move_Distance, bool block){
    int32_t distance_DUs;
    Degrees_To_Device(move_Distance, distance_DUs);
    return Move(distance_DUs, true, block);
}

int LD_ThorRotation::Move_Absolute(double move_Distance, bool block){
    move_Distance += m_Origin_Angle;

    int32_t distance_DUs;
    Degrees_To_Device(move_Distance, distance_DUs);
    return Move(distance_DUs, false, block);
}

int LD_ThorRotation::Move_Jog(bool forwards){
    std::vector<uint8_t> command{0x6A, 0x04, 0x01, 0x01, 0x50, 0x01};
    if(!forwards){
        command[3] = 0x02;
    }
    std::cout << "Jog\n";
    SendCommand(command);
    return 0;
}

int LD_ThorRotation::Move_Velocity(VelParams params, bool forwards){
    Set_Vel_Params(params);
    return Move_Velocity(forwards);
}

int LD_ThorRotation::Move_Velocity(bool forwards){
    std::vector<uint8_t> command{0x57, 0x04, 0x01, 0x01, 0x50, 0x01};
    if(!forwards){
        command[3] = 0x02;
    }
    SendCommand(command);
    return 0;
}

int LD_ThorRotation::Move_Stop(bool do_Profiled_Stop){
    std::vector<uint8_t> command{0x65, 0x04, 0x01, 0x01, 0x50, 0x01};
    std::vector<uint8_t> expected{0x66, 0x04, 0x0e, 0x00, 0x81, 0x50};
    std::vector<uint8_t> response;

    if(do_Profiled_Stop){
        command[3] = 0x02;
    }
    std::cout << "Stopping\n";
    SendCommand(command);
    RecvResponse(response, 6, 5, 1000);

    if(CheckResponse(response, expected)){
        std::cout << "Stopped\n";
    }
    else{
        std::cout << "Weird response...\n";
    }

    return 0;
}

//// boilerplate for get and set
//int Get(){
//    std::vector<uint8_t> command{0,0,0,0,0,0};
//    std::vector<uint8_t> expected{0,0,0,0,0,0};
//    std::vector<uint8_t> response;
//
//    SendCommand(command);
//    RecvResponse(response, 10);
//
//    if (CheckResponse(response, expected)){
//        //do stuff
//    }
//}
//
//int Set(){
//    std::vector<uint8_t> command{0,0,0,0,0,0,
//                                 0,0,
//                                 0,0};
//    SendCommand(command)
//}
