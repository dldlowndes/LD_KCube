import serial

ser = serial.Serial("COM9", 115200)

def Send(bytes_List):
    message = bytearray(bytes_List)
    ser.write(message)
    
def Send_Receive(bytes_List, response_Size):
    Send(bytes_List)
    return Receive(response_Size)

def Receive(response_Size):
    response = ser.read(size=response_Size)
    return [hex(x) for x in response]

home_Get = [0x41, 0x04, 0x01, 0x00, 0x50, 0x01]

params_Get = [0x21, 0x05, 0x01, 0x00, 0x50, 0x01]


#ser.close()