#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#include <SDL2/SDL.h>

enum MouseButton
{
	MB_LEFT = 0,
	MB_RIGHT,
	MB_MIDDLE,
	
	MB_END__
};

void Input_HandleEvents(void);
void Input_PostUpdate(void);

// keyboard input functionality.
void Keybd_SetKeyState(SDL_Event const *e, bool Pressed);
void Keybd_RegisterTextInput(SDL_Event const *e);
void Keybd_PostUpdate(void);
bool Keybd_Down(SDL_Keycode k);
bool Keybd_Pressed(SDL_Keycode k);
bool Keybd_TextInputReceived(char Ch);
bool Keybd_ShiftHeld(void);

// mouse input functionality.
void Mouse_PressButton(SDL_Event const *e);
void Mouse_ReleaseButton(SDL_Event const *e);
void Mouse_PostUpdate(void);
bool Mouse_Down(enum MouseButton b);
bool Mouse_Pressed(enum MouseButton b);
bool Mouse_Released(enum MouseButton b);
void Mouse_Pos(int *OutX, int *OutY);

#endif
