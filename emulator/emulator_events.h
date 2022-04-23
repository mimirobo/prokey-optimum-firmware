#ifndef __EMULATOR_EVENTS__
#define __EMULATOR_EVENTS__

#include <SDL2/SDL.h>
#include "buttons.h"

extern SDL_MouseButtonEvent g_MouseEvent;
extern bool g_MouseIsDown;
extern int g_scale;


void handleKeyboardEvents(uint16_t *state);
void handleMouseEvents(uint16_t *state);

#endif // __EMULATOR_EVENTS__




