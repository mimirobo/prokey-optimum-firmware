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
  // todo: handle extra mouse events here
}

bool isMouseLeftButtonDown()
{
  static bool cached_state = false;
  if (g_MouseEvent.button == SDL_BUTTON_LEFT)
  {
    cached_state = (g_MouseEvent.state == SDL_PRESSED);
  }

  return cached_state;
}

bool isMouseOverButton(struct ProkeyRoundButton *button)
{
  if (g_MouseMotionEvent.y > (button->y - button->r) * g_scale && g_MouseMotionEvent.y < (button->y + button->r) * g_scale)
  {
    if (g_MouseMotionEvent.x > (button->x - button->r) * g_scale && g_MouseMotionEvent.x < (button->x + button->r) * g_scale)
    {
      return true;
    }
  }
  return false;
}