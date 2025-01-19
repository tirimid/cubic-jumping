#ifndef CAM_H
#define CAM_H

struct Cam
{
	float PosX, PosY;
	float Zoom;
};

extern struct Cam g_Cam;

void Cam_Update(void);

#endif
