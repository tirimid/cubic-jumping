#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "util.h"

struct UiButton
{
	void (*Callback)(void);
	char const *Text;
	i32 x, y;
	i32 w, h;
	bool Hovered, Pressed;
};

struct UiTextField
{
	char *Out;
	u32 Csr, Len, Nmax;
	u32 FirstDraw, Ndraw;
	i32 x, y;
	i32 w, h;
	bool Hovered, Selected;
};

struct UiSlider
{
	void (*Callback)(float);
	i32 x, y;
	i32 w, h;
	f32 Val;
	bool Hovered, Pressed;
};

struct UiButton UiButton_Create(i32 x, i32 y, char const *Text, void (*Callback)(void));
void UiButton_Update(struct UiButton *Btn);
void UiButton_Draw(struct UiButton const *Btn);
struct UiTextField UiTextField_Create(i32 x, i32 y, u32 Ndraw, char *Out, u32 Nmax);
void UiTextField_Update(struct UiTextField *Tf);
void UiTextField_Draw(struct UiTextField const *Tf);
struct UiSlider UiSlider_Create(i32 x, i32 y, i32 w, i32 h, f32 Initial, void (*Callback)(f32));
void UiSlider_Update(struct UiSlider *s);
void UiSlider_Draw(struct UiSlider const *s);

#endif
