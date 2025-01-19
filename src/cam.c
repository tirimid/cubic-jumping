#include "cam.h"

#include <math.h>

#include "conf.h"
#include "player.h"
#include "util.h"

struct Cam g_Cam =
{
	.Zoom = CONF_CAM_MAX_ZOOM
};

void
Cam_Update(void)
{
	// pan camera towards player destination.
	{
		g_Cam.PosX = Lerp(
			g_Cam.PosX,
			g_Player.PosX + CONF_PLAYER_SIZE / 2.0f + CONF_CAM_LOOKAHEAD_X * g_Player.VelX,
			CONF_CAM_SPEED
		);
		
		g_Cam.PosY = Lerp(
			g_Cam.PosY,
			g_Player.PosY + CONF_PLAYER_SIZE / 2.0f + CONF_CAM_LOOKAHEAD_Y * g_Player.VelY,
			CONF_CAM_SPEED
		);
	}
	
	// zoom out based on player speed.
	{
		float Psx2 = g_Player.VelX * g_Player.VelX;
		float Psy2 = g_Player.VelY * g_Player.VelY;
		
		float ZoomExt = sqrtf(Psx2 + Psy2);
		ZoomExt = MIN(ZoomExt, CONF_CAM_MAX_ZOOM);
		ZoomExt = MAX(ZoomExt, CONF_CAM_MIN_ZOOM);
		
		g_Cam.Zoom = Lerp(g_Cam.Zoom, 1.0f - ZoomExt, CONF_CAM_ZOOM_SPEED);
	}
}
