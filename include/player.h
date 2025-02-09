#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include <unistd.h>

#include "map.h"
#include "util.h"

enum PlayerState
{
	PS_PLAYING = 0,
	PS_DEAD
};

enum PlayerCapId
{
	PCI_JUMP = 0,
	PCI_WALLJUMP = 1,
	PCI_WALLSLIDE = 2,
	PCI_POWERJUMP = 3,
	PCI_DASH_DOWN = 4,
	PCI_COLLIDE = 5
};

struct PlayerCapMask
{
	bool NoJump, NoWalljump, NoWallslide;
	bool NoPowerjump, NoDashDown;
	bool NoCollide;
};

struct Player
{
	// movement data.
	f32 PosX, PosY;
	f32 VelX, VelY;
	bool AirControl;
	
	// collision handling data.
	f32 DistLeft, DistRight, DistTop, DistBottom;
	struct MapTile *NearLeft, *NearRight, *NearTop, *NearBottom;
	bool ShortCircuit;
	
	// visual data.
	u32 TraceSpawnTicks;
	u32 DeadTicks;
};

extern struct Player g_Player;
extern enum PlayerState g_PlayerState;
extern struct PlayerCapMask g_PlayerCapMask;

void Player_Update(void);
void Player_Draw(void);
bool Player_Grounded(void);
bool Player_WalledLeft(void);
bool Player_WalledRight(void);
void Player_Die(void);
void Player_SetCapMask(enum PlayerCapId Id, bool State);

#endif
