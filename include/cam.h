#ifndef CAM_H
#define CAM_H

#include "util.h"

struct Cam
{
	f32 PosX, PosY;
	f32 Zoom;
	f32 Shake;
};

extern struct Cam g_Cam;

void Cam_Update(void);

#endif
