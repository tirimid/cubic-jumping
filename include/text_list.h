#ifndef TEXT_LIST_H
#define TEXT_LIST_H

// consult these values when placing message triggers in maps.
enum text_list_item
{
	TLI_CTE0_TEST = 0x0,
	
	// c0e0.
	TLI_C0E0_HOW_TO_MOVE = 0x1,
	TLI_C0E0_HOW_TO_JUMP = 0x2,
	TLI_C0E0_HOW_TO_WALLJUMP = 0x3,
	TLI_C0E0_HOW_TO_CLIMB = 0x4,
	TLI_C0E0_HOW_TO_SLIDE = 0x5,
	TLI_C0E0_HOW_TO_WIN = 0x6,
	
	// c0e1.
	TLI_C0E1_KILL_INTRO = 0x7,
	TLI_C0E1_MOMENTUM_INTRO = 0x8,
	TLI_C0E1_HOW_TO_POWERJUMP = 0x9,
	
	// c0e2.
	TLI_C0E2_BOUNCE_INTRO = 0xa,
	
	// c0e3.
	TLI_C0E3_LEAP_OF_FAITH = 0xb,
	
	// c0e4.
	TLI_C0E4_HOW_TO_AIR_CONTROL = 0xc,
	TLI_C0E4_LAUNCH_INTRO = 0xd,
	
	// c0e5.
	TLI_C0E5_HORIZONTAL_LAUNCH_INTRO = 0xe,
	
	// c0e6.
	TLI_C0E6_END_OFF_INTRO = 0xf,
	TLI_C0E6_SWITCH_INTRO = 0x10,
	
	// c0e8.
	TLI_C0E8_HOW_TO_DASH_DOWN = 0x11,
	TLI_C0E8_EXPLOIT_RESTITUTION = 0x12,
	TLI_C0E8_DODGE_OBSTACLES = 0x13,
	
	// c0e9.
	TLI_C0E9_SLIPPERY_INTRO = 0x14,
	
	// c0e11.
	TLI_C0E11_GRIP_INTRO = 0x15,
	
	TLI_END__,
};

void text_list_term(void);
void text_list_enqueue(enum text_list_item item);
void text_list_update(void);
void text_list_draw(void);

#endif
