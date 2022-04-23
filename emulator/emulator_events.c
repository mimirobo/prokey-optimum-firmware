#include "emulator_events.h"

void handleKeyboardEvents(uint16_t *state)
{
  const uint8_t *scancodes = SDL_GetKeyboardState(NULL);
  if (scancodes[SDL_SCANCODE_LEFT]) {
    *state |= BTN_PIN_NO;
  }
  if (scancodes[SDL_SCANCODE_RIGHT]) {
    *state |= BTN_PIN_YES;
  }
  if (scancodes[SDL_SCANCODE_UP])
  {
    *state |= BTN_PIN_UP;
  }
  if (scancodes[SDL_SCANCODE_DOWN])
  {
    *state |= BTN_PIN_DOWN;
  }
}

void handleMouseEvents(uint16_t *state)
{
    if (g_MouseIsDown)
  {
      if (g_MouseEvent.y > 136 * g_scale && g_MouseEvent.y < 156 * g_scale)
      {
        if (g_MouseEvent.x > 19 * g_scale && g_MouseEvent.x < 41 * g_scale)
        {
          *state |= BTN_PIN_NO;
        }
        else if (g_MouseEvent.x > 49 * g_scale && g_MouseEvent.x < 71 * g_scale)
        {
          *state |= BTN_PIN_DOWN;
        }
        else if (g_MouseEvent.x > 78 * g_scale && g_MouseEvent.x < 99 * g_scale)
        {
          *state |= BTN_PIN_UP;
        }
        else if (g_MouseEvent.x > 107 * g_scale && g_MouseEvent.x < 128 * g_scale)
        {
          *state |= BTN_PIN_YES;
        }
      }
  }
}