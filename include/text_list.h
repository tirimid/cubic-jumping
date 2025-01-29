#ifndef TEXT_LIST_H
#define TEXT_LIST_H

// consult these values when placing message triggers in maps.
enum TextListItem
{
	TLI_TEST = 0x0,
	TLI_HOW_TO_MOVE = 0x1,
	TLI_HOW_TO_JUMP = 0x2,
	TLI_HOW_TO_WALLJUMP = 0x3,
	TLI_HOW_TO_CLIMB = 0x4,
	TLI_HOW_TO_SLIDE = 0x5,
	TLI_HOW_TO_WIN = 0x6,
	TLI_KILL_INTRO = 0x7,
	TLI_MOMENTUM_INTRO = 0x8,
	TLI_HOW_TO_POWERJUMP = 0x9,
	TLI_BOUNCE_INTRO = 0xa,
	TLI_LEAP_OF_FAITH = 0xb,
	TLI_BOUNCE_MULTIPLE = 0xc,
	TLI_LAUNCH_INTRO = 0xd,
	TLI_HORIZONTAL_LAUNCH_INTRO = 0xe,
	TLI_END_OFF_INTRO = 0xf,
	TLI_SWITCH_INTRO = 0x10,
	TLI_HOW_TO_DASH_DOWN = 0x11,
	TLI_EXPLOIT_RESTITUTION = 0x12,
	TLI_DODGE_OBSTACLES = 0x13,
	TLI_SLIPPERY_INTRO = 0x14,
	TLI_GRIP_INTRO = 0x15,
	TLI_TERMINATE_JUMP = 0x16,
	TLI_BEAM_INTRO = 0x17,
	TLI_FALL_DIRECTIONS = 0x18,
	TLI_SWAP_INTRO = 0x19,
	
	TLI_END__
};

void TextList_Term(void);
void TextList_Enqueue(enum TextListItem Item);
void TextList_Update(void);
void TextList_Draw(void);

#endif
