#ifndef MENUS_H
#define MENUS_H

#include <SDL.h>

// used for communication between menus and sometimes their callers.
enum MenuRequest
{
	MR_NONE = 0,
	MR_NEXT,
	MR_RETRY
};

void MainMenuLoop(void);
void CustomLevelSelectMenuLoop(void);
enum MenuRequest LevelEndMenuLoop(void);
void PauseMenuLoop(void);
void OptionsMenuLoop(void);
SDL_Keycode KeyDetectMenuLoop(void);
void MessageMenuLoop(char const *Msg);
void ChapterEndMenuLoop(void);

#endif
