#ifndef CAM_H
#define CAM_H

typedef struct cam
{
	float pos_x, pos_y;
	float zoom;
} cam_t;

extern cam_t g_cam;

void cam_update(void);

#endif
