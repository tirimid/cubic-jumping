#include "player.h"

#include <stddef.h>

#include "cam.h"
#include "conf.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "map_list.h"
#include "options.h"
#include "sound.h"
#include "util.h"
#include "vfx.h"
#include "wnd.h"

#define COL_THRESHOLD 0.05f

struct Player g_Player;
enum PlayerState g_PlayerState;
struct PlayerCapMask g_PlayerCapMask;

static void UpdatePlaying(void);
static void UpdateDead(void);
static void Collide(struct MapTile *Tile);
static void CollideLeft(void);
static void CollideRight(void);
static void CollideBottom(void);
static void CollideTop(void);
static void ComputeCollisionDistances(void);
static void TestAndApplyCollisions(void);

void
Player_Update(void)
{
	static void (*UpdateFn[])(void) =
	{
		UpdatePlaying,
		UpdateDead
	};
	
	UpdateFn[g_PlayerState]();
}

void
Player_Draw(void)
{
	if (g_PlayerState == PS_DEAD)
		return;
	
	static uint8_t Cp[] = CONF_COLOR_PLAYER;
	SDL_SetRenderDrawColor(g_Rend, Cp[0], Cp[1], Cp[2], 255);
	RelativeDrawRect(
		g_Player.PosX,
		g_Player.PosY,
		CONF_PLAYER_SIZE,
		CONF_PLAYER_SIZE
	);
}

bool
Player_Grounded(void)
{
	return g_Player.DistBottom < COL_THRESHOLD && !g_Player.ShortCircuit;
}

bool
Player_WalledLeft(void)
{
	return g_Player.DistLeft < COL_THRESHOLD && !g_Player.ShortCircuit;
}

bool
Player_WalledRight(void)
{
	return g_Player.DistRight < COL_THRESHOLD && !g_Player.ShortCircuit;
}

void
Player_Die(void)
{
	// prevent "double-death", incrementing game deaths by 2 instead of 1.
	if (g_PlayerState == PS_DEAD)
		return;
	
	++g_Game.IlDeaths;
	++g_Game.TotalDeaths;
	
	g_Player.DeadTicks = CONF_PLAYER_DEATH_TICKS;
	g_PlayerState = PS_DEAD;
	
	for (int i = 0; i < CONF_PLAYER_SHARD_CNT; ++i)
	{
		Vfx_PutParticle(
			PT_PLAYER_SHARD,
			g_Player.PosX + CONF_PLAYER_SIZE / 2.0f,
			g_Player.PosY + CONF_PLAYER_SIZE / 2.0f
		);
	}
	
	Sound_PlaySfx(SI_DEATH);
}

void
Player_SetCapMask(enum PlayerCapId Id, bool State)
{
	switch (Id)
	{
	case PCI_JUMP:
		g_PlayerCapMask.NoJump = State;
		break;
	case PCI_WALLJUMP:
		g_PlayerCapMask.NoWalljump = State;
		break;
	case PCI_WALLSLIDE:
		g_PlayerCapMask.NoWallslide = State;
		break;
	case PCI_POWERJUMP:
		g_PlayerCapMask.NoPowerjump = State;
		break;
	case PCI_DASH_DOWN:
		g_PlayerCapMask.NoDashDown = State;
		break;
	}
}

static void
UpdatePlaying(void)
{
	// rectify player bounds in map.
	{
		if (g_Player.PosX < 0.0f)
			g_Player.PosX = 0.0f;
		if (g_Player.PosY < 0.0f)
			g_Player.PosY = 0.0f;
		
		if (g_Player.PosX > g_Map.SizeX - CONF_PLAYER_SIZE)
			g_Player.PosX = g_Map.SizeX - CONF_PLAYER_SIZE;
		if (g_Player.PosY > g_Map.SizeY - CONF_PLAYER_SIZE)
			g_Player.PosY = g_Map.SizeY - CONF_PLAYER_SIZE;
	}
	
	// need to initially check collisions prior to player movement being
	// applied in order to fix bug when you can jump on kill blocks.
	{
		// the short circuit mechanism exists as a way to allow certain
		// collision tiles to demand exclusive collision handling instead of
		// the others.
		// for example, the level end tiles need to short circuit, as
		// otherwise the player landing on a level end tile could cause the
		// level end menu to open multiple times in a row as a result of
		// collision being tested twice during a player cycle.
		g_Player.ShortCircuit = false;
		
		TestAndApplyCollisions();
	}
	
	// apply user movement input velocity.
	{
		if (Player_Grounded() || (!Player_Grounded() && g_Player.AirControl))
		{
			float MvHoriz = Keybd_Down(g_Options.KRight) - Keybd_Down(g_Options.KLeft);
			MvHoriz *= Player_Grounded() ? CONF_PLAYER_SPEED : CONF_PLAYER_AIR_SPEED;
			g_Player.VelX += MvHoriz;
		}
		
		if (!g_PlayerCapMask.NoDashDown
			&& !Player_Grounded()
			&& Keybd_Pressed(g_Options.KDashDown))
		{
			g_Player.VelY = CONF_PLAYER_DASH_DOWN_SPEED;
			for (int i = 0; i < CONF_AIR_PUFF_CNT; ++i)
			{
				Vfx_PutParticle(
					PT_AIR_PUFF,
					g_Player.PosX + CONF_PLAYER_SIZE / 2.0f,
					g_Player.PosY + CONF_PLAYER_SIZE / 2.0f
				);
			}
			Sound_PlaySfx(SI_DASH_DOWN);
		}
		
		if (!g_PlayerCapMask.NoJump
			&& Player_Grounded()
			&& Keybd_Down(g_Options.KJump))
		{
			g_Player.VelY = -CONF_PLAYER_JUMP_FORCE;
			Sound_PlaySfx(SI_JUMP);
		}
		else if (!g_PlayerCapMask.NoPowerjump
			&& Player_Grounded()
			&& Keybd_Down(g_Options.KPowerjump))
		{
			if (g_Player.VelX > 0.0f)
				g_Player.VelX = CONF_PLAYER_POWERJUMP_FORCE_X;
			else if (g_Player.VelX < 0.0f)
				g_Player.VelX = -CONF_PLAYER_POWERJUMP_FORCE_X;
			g_Player.VelY = -CONF_PLAYER_POWERJUMP_FORCE_Y;
			Sound_PlaySfx(SI_POWERJUMP);
		}
		
		if (!g_PlayerCapMask.NoWallslide
			&& Player_WalledLeft()
			&& Keybd_Down(g_Options.KLeft)
			&& !Map_TileSlippery(g_Player.NearLeft->Type))
		{
			if (Map_TileClimbable(g_Player.NearLeft->Type))
				g_Player.VelY = -CONF_CLIMB_SPEED;
			else
				g_Player.VelY /= CONF_WALL_SLIDE_FRICTION;
		}
		
		if (!g_PlayerCapMask.NoWallslide
			&& Player_WalledRight()
			&& Keybd_Down(g_Options.KRight)
			&& !Map_TileSlippery(g_Player.NearRight->Type))
		{
			if (Map_TileClimbable(g_Player.NearRight->Type))
				g_Player.VelY = -CONF_CLIMB_SPEED;
			else
				g_Player.VelY /= CONF_WALL_SLIDE_FRICTION;
		}
		
		if (!g_PlayerCapMask.NoWalljump
			&& Player_WalledLeft()
			&& Keybd_Down(g_Options.KJump)
			&& !Map_TileSlippery(g_Player.NearLeft->Type))
		{
			g_Player.VelX = CONF_PLAYER_WALLJUMP_FORCE_X;
			g_Player.VelY = -CONF_PLAYER_WALLJUMP_FORCE_Y;
			Sound_PlaySfx(SI_WALLJUMP);
		}
		
		if (!g_PlayerCapMask.NoWalljump
			&& Player_WalledRight()
			&& Keybd_Down(g_Options.KJump)
			&& !Map_TileSlippery(g_Player.NearRight->Type))
		{
			g_Player.VelX = -CONF_PLAYER_WALLJUMP_FORCE_X;
			g_Player.VelY = -CONF_PLAYER_WALLJUMP_FORCE_Y;
			Sound_PlaySfx(SI_WALLJUMP);
		}
	}
	
	// apply environmental forces.
	{
		g_Player.VelY += CONF_GRAVITY;
		g_Player.VelX /= Player_Grounded() ? CONF_FRICTION : CONF_DRAG;
		g_Player.VelY /= CONF_DRAG;
	}
	
	// need to also apply collsions after all velocity changes.
	{
		TestAndApplyCollisions();
	}
	
	// actually move player.
	{
		g_Player.PosX += g_Player.VelX;
		g_Player.PosY += g_Player.VelY;
	}
	
	// advance trace spawn counter, and spawn if needed.
	{
		if (g_Player.TraceSpawnTicks == 0)
		{
			Vfx_PutParticle(
				PT_PLAYER_TRACE,
				g_Player.PosX + CONF_PLAYER_SIZE / 2.0f,
				g_Player.PosY + CONF_PLAYER_SIZE / 2.0f
			);
			g_Player.TraceSpawnTicks = CONF_PLAYER_TRACE_SPAWN_TICKS;
		}
		else
			--g_Player.TraceSpawnTicks;
	}
}

static void
UpdateDead(void)
{
	if (g_Player.DeadTicks == 0)
	{
		// map list reload simulates a respawn procedure.
		MapList_SoftReload();
		return;
	}
	
	--g_Player.DeadTicks;
}

static void
Collide(struct MapTile *Tile)
{
	if (!Tile)
		return;
	
	g_Player.AirControl = true;
	
	// implement behavior that doesn't depend on collision direction.
	switch (Tile->Type)
	{
	case MTT_KILL:
		g_Player.ShortCircuit = true;
		Player_Die();
		break;
	case MTT_LAUNCH:
		Sound_PlaySfx(SI_LAUNCH);
		break;
	case MTT_END_ON:
		g_Player.ShortCircuit = true;
		Sound_PlaySfx(SI_END);
		MapList_LoadNext();
		break;
	case MTT_SWITCH_OFF:
		Tile->Type = MTT_SWITCH_ON;
		Sound_PlaySfx(SI_SWITCH);
		Game_EnableSwitch();
		break;
	default:
		break;
	}
}

static void
CollideLeft(void)
{
	if (!g_Player.NearLeft)
		return;
	
	g_Player.PosX -= g_Player.DistLeft - 0.001f;
	
	switch (g_Player.NearLeft->Type)
	{
	case MTT_BOUNCE:
		if (g_Player.VelX < -CONF_MIN_RESTITUTION_SPEED)
		{
			g_Player.VelX *= -CONF_RESTITUTION;
			Sound_PlaySfx(SI_BOUNCE);
		}
		else
			g_Player.VelX = 0.0f;
		break;
	case MTT_LAUNCH:
		g_Player.AirControl = false;
		g_Player.ShortCircuit = true;
		g_Player.VelX = CONF_WALL_LAUNCH_FORCE_X;
		g_Player.VelY = -CONF_WALL_LAUNCH_FORCE_Y;
		break;
	default:
		g_Player.VelX = 0.0f;
		break;
	}
}

static void
CollideRight(void)
{
	if (!g_Player.NearRight)
		return;
	
	g_Player.PosX += g_Player.DistRight - 0.001f;
	
	switch (g_Player.NearRight->Type)
	{
	case MTT_BOUNCE:
		if (g_Player.VelX > CONF_MIN_RESTITUTION_SPEED)
		{
			g_Player.VelX *= -CONF_RESTITUTION;
			Sound_PlaySfx(SI_BOUNCE);
		}
		else
			g_Player.VelX = 0.0f;
		break;
	case MTT_LAUNCH:
		g_Player.AirControl = false;
		g_Player.ShortCircuit = true;
		g_Player.VelX = -CONF_WALL_LAUNCH_FORCE_X;
		g_Player.VelY = -CONF_WALL_LAUNCH_FORCE_Y;
		break;
	default:
		g_Player.VelX = 0.0f;
		break;
	}
}

static void
CollideBottom(void)
{
	if (!g_Player.NearBottom)
		return;
	
	g_Player.PosY += g_Player.DistBottom - 0.001f;
	
	switch (g_Player.NearBottom->Type)
	{
	case MTT_BOUNCE:
		if (g_Player.VelY > CONF_MIN_RESTITUTION_SPEED)
		{
			g_Player.VelY *= -CONF_RESTITUTION;
			Sound_PlaySfx(SI_BOUNCE);
		}
		else
			g_Player.VelY = 0.0f;
		break;
	case MTT_LAUNCH:
		g_Player.VelY = -CONF_LAUNCH_FORCE;
		break;
	default:
		g_Player.VelY = 0.0f;
		break;
	}
}

static void
CollideTop(void)
{
	if (!g_Player.NearTop)
		return;
	
	g_Player.PosY -= g_Player.DistTop - 0.001f;
	
	switch (g_Player.NearTop->Type)
	{
	case MTT_BOUNCE:
		if (g_Player.VelY < -CONF_MIN_RESTITUTION_SPEED)
		{
			g_Player.VelY *= -CONF_RESTITUTION;
			Sound_PlaySfx(SI_BOUNCE);
		}
		else
			g_Player.VelY = 0.0f;
		break;
	default:
		g_Player.VelY = 0.0f;
		break;
	}
}

static void
ComputeCollisionDistances(void)
{
	// find nearest left edge.
	{
		int Cxtl;
		struct MapTile *CxtlTile = NULL;
		for (Cxtl = g_Player.PosX; Cxtl >= 0; --Cxtl)
		{
			struct MapTile *Tile = Map_Get(Cxtl, g_Player.PosY);
			if (Map_TileCollision(Tile->Type))
			{
				CxtlTile = Tile;
				break;
			}
		}
		
		int Cxbl;
		struct MapTile *CxblTile = NULL;
		for (Cxbl = g_Player.PosX; Cxbl >= 0; --Cxbl)
		{
			struct MapTile *Tile = Map_Get(Cxbl, g_Player.PosY + CONF_PLAYER_SIZE);
			if (Map_TileCollision(Tile->Type))
			{
				CxblTile = Tile;
				break;
			}
		}
		
		int Cxl = MAX(Cxtl, Cxbl);
		g_Player.DistLeft = g_Player.PosX - Cxl - 1.0f;
		g_Player.NearLeft = Cxtl > Cxbl ? CxtlTile : CxblTile;
	}
	
	// find nearest right edge.
	{
		int Cxtr;
		struct MapTile *CxtrTile = NULL;
		for (Cxtr = g_Player.PosX + CONF_PLAYER_SIZE; Cxtr < g_Map.SizeX; ++Cxtr)
		{
			struct MapTile *Tile = Map_Get(Cxtr, g_Player.PosY);
			if (Map_TileCollision(Tile->Type))
			{
				CxtrTile = Tile;
				break;
			}
		}
		
		int Cxbr;
		struct MapTile *CxbrTile = NULL;
		for (Cxbr = g_Player.PosX + CONF_PLAYER_SIZE; Cxbr < g_Map.SizeX; ++Cxbr)
		{
			struct MapTile *Tile = Map_Get(Cxbr, g_Player.PosY + CONF_PLAYER_SIZE);
			if (Map_TileCollision(Tile->Type))
			{
				CxbrTile = Tile;
				break;
			}
		}
		
		int Cxr = MIN(Cxtr, Cxbr);
		g_Player.DistRight = Cxr - g_Player.PosX - CONF_PLAYER_SIZE;
		g_Player.NearRight = Cxtr < Cxbr ? CxtrTile : CxbrTile;
	}
	
	// find nearest top edge.
	{
		int Cytl;
		struct MapTile *CytlTile = NULL;
		for (Cytl = g_Player.PosY; Cytl >= 0; --Cytl)
		{
			struct MapTile *Tile = Map_Get(g_Player.PosX, Cytl);
			if (Map_TileCollision(Tile->Type))
			{
				CytlTile = Tile;
				break;
			}
		}
		
		int Cytr;
		struct MapTile *CytrTile = NULL;
		for (Cytr = g_Player.PosY; Cytr >= 0; --Cytr)
		{
			struct MapTile *Tile = Map_Get(g_Player.PosX + CONF_PLAYER_SIZE, Cytr);
			if (Map_TileCollision(Tile->Type))
			{
				CytrTile = Tile;
				break;
			}
		}
		
		int Cyt = MAX(Cytl, Cytr);
		g_Player.DistTop = g_Player.PosY - Cyt - 1.0f;
		g_Player.NearTop = Cytl > Cytr ? CytlTile : CytrTile;
	}
	
	// find nearest bottom edge.
	{
		int Cybl;
		struct MapTile *CyblTile = NULL;
		for (Cybl = g_Player.PosY + CONF_PLAYER_SIZE; Cybl < g_Map.SizeY; ++Cybl)
		{
			struct MapTile *Tile = Map_Get(g_Player.PosX, Cybl);
			if (Map_TileCollision(Tile->Type))
			{
				CyblTile = Tile;
				break;
			}
		}
		
		int Cybr;
		struct MapTile *CybrTile = NULL;
		for (Cybr = g_Player.PosY + CONF_PLAYER_SIZE; Cybr < g_Map.SizeY; ++Cybr)
		{
			struct MapTile *Tile = Map_Get(g_Player.PosX + CONF_PLAYER_SIZE, Cybr);
			if (Map_TileCollision(Tile->Type))
			{
				CybrTile = Tile;
				break;
			}
		}
		
		int Cyb = MIN(Cybl, Cybr);
		g_Player.DistBottom = Cyb - g_Player.PosY - CONF_PLAYER_SIZE;
		g_Player.NearBottom = Cybl < Cybr ? CyblTile : CybrTile;
	}
}

static void
TestAndApplyCollisions(void)
{
	if (g_Player.ShortCircuit)
		return;
	
	ComputeCollisionDistances();
	if (-g_Player.VelY >= g_Player.DistTop
		&& g_Player.DistTop < g_Player.DistBottom)
	{
		Collide(g_Player.NearTop);
		CollideTop();
	}
	
	if (g_Player.ShortCircuit)
		return;
	
	ComputeCollisionDistances();
	if (-g_Player.VelX >= g_Player.DistLeft
		&& g_Player.DistLeft < g_Player.DistRight)
	{
		Collide(g_Player.NearLeft);
		CollideLeft();
	}
	
	if (g_Player.ShortCircuit)
		return;
	
	ComputeCollisionDistances();
	if (g_Player.VelX >= g_Player.DistRight
		&& g_Player.DistRight < g_Player.DistLeft)
	{
		Collide(g_Player.NearRight);
		CollideRight();
	}
	
	if (g_Player.ShortCircuit)
		return;
	
	ComputeCollisionDistances();
	if (g_Player.VelY >= g_Player.DistBottom
		&& g_Player.DistBottom < g_Player.DistTop)
	{
		Collide(g_Player.NearBottom);
		CollideBottom();
	}
}
