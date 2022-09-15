#ifndef STM32_CAN_H
#define STM32_CAN_H

class CanHardware
{
public:
    virtual void Send(int id, uint32_t* bytes, int length) = 0;
};

#endif
