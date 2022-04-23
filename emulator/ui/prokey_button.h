#ifndef __PROKEY_BUTTON_H__
#define __PROKEY_BUTTON_H__

#if EMULATOR
#include <stdint.h>
#include <stdbool.h>

struct ProkeyRoundButton
{
    uint8_t x,y,r;
    uint16_t hardwareCode;
    bool isHover, isClicked;
};

#endif // EMULATOR
#endif // __PROKEY_BUTTON_H__