#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stddef.h>

struct UiButton
{
	void (*Callback)(void);
	char const *Text;
	int x, y;
	int w, h;
	bool Hovered, Pressed;
};

struct UiTextField
{
	char *Out;
	size_t Csr, Len, Nmax;
	size_t FirstDraw, Ndraw;
	int x, y;
	int w, h;
	bool Hovered, Selected;
};

struct UiSlider
{
	void (*Callback)(float);
	int x, y;
	int w, h;
	float Val;
	bool Hovered, Pressed;
};

struct UiButton UiButton_Create(int x, int y, char const *Text, void (*Callback)(void));
void UiButton_Update(struct UiButton *Btn);
void UiButton_Draw(struct UiButton const *Btn);
struct UiTextField UiTextField_Create(int x, int y, size_t Ndraw, char *Out, size_t Nmax);
void UiTextField_Update(struct UiTextField *Tf);
void UiTextField_Draw(struct UiTextField const *Tf);
struct UiSlider UiSlider_Create(int x, int y, int w, int h, float Initial, void (*Callback)(float));
void UiSlider_Update(struct UiSlider *s);
void UiSlider_Draw(struct UiSlider const *s);

#endif
