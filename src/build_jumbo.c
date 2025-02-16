#include "src/cam.c"
#include "src/editor.c"
#define DrawBg Game_DrawBg
#define DrawIndicators Game_DrawIndicators
#include "src/game.c"
#undef DrawBg
#undef DrawIndicators
#include "src/input.c"
#include "src/main.c"
#include "src/map.c"
#include "src/menus.c"
#include "src/options.c"
#include "src/player.c"
#define RdUint8 Save_RdUint8
#define RdUint32 Save_RdUint32
#define WrUint8 Save_WrUint8
#define WrUint32 Save_WrUint32
#include "src/save.c"
#undef RdUint8
#undef RdUint32
#undef WrUint8
#undef WrUint32
#define DrawBg Sequences_DrawBg
#include "src/sequences.c"
#undef DrawBg
#include "src/text.c"
#include "src/text_list.c"
#include "src/textures.c"
#define Collide Triggers_Collide
#include "src/triggers.c"
#undef Collide
#include "src/ui.c"
#include "src/util.c"
#include "src/vfx.c"
#include "src/wnd.c"
