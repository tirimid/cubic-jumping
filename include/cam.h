#ifndef CAM_H
#define CAM_H

struct cam
{
	float pos_x, pos_y;
	float zoom;
};

extern struct cam g_cam;

void cam_update(void);

#endif
