#ifndef CAM_H
#define CAM_H

#include "util.h"

struct Cam
{
	f32 PosX, PosY;
	f32 Zoom;
};

extern struct Cam g_Cam;

void Cam_Update(void);

#endif
