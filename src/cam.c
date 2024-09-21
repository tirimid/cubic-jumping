#include "cam.h"

#include <math.h>

#include "conf.h"
#include "player.h"
#include "util.h"

cam_t g_cam =
{
	.pos_x = 0.0f,
	.pos_y = 0.0f,
	.zoom = CONF_CAM_MAX_ZOOM,
};

void
cam_update(void)
{
	// pan camera towards player.
	do
	{
		g_cam.pos_x = lerp(g_cam.pos_x,
		                   g_player.pos_x + CONF_PLAYER_SIZE / 2.0f,
		                   CONF_CAM_SPEED);
		
		g_cam.pos_y = lerp(g_cam.pos_y,
		                   g_player.pos_y + CONF_PLAYER_SIZE / 2.0f,
		                   CONF_CAM_SPEED);
	} while (0);
	
	// zoom out based on player speed.
	do
	{
		float psx2 = g_player.vel_x * g_player.vel_x;
		float psy2 = g_player.vel_y * g_player.vel_y;
		
		float zoom_ext = sqrtf(psx2 + psy2);
		zoom_ext = MIN(zoom_ext, CONF_CAM_MAX_ZOOM);
		zoom_ext = MAX(zoom_ext, CONF_CAM_MIN_ZOOM);
		
		g_cam.zoom = lerp(g_cam.zoom, 1.0f - zoom_ext, CONF_CAM_ZOOM_SPEED);
	} while (0);
}
