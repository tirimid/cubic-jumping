#ifndef CAM_H
#define CAM_H

typedef struct cam
{
	float pos_x, pos_y;
	float zoom;
} cam;

extern cam g_cam;

void cam_update(void);

#endif
