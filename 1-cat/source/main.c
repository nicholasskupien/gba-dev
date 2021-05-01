//
// obj_demo.c
// testing various sprite related things
//
// (20031003 - 20060924, Cearn)

#include <string.h>
#include <tonc.h>
#include "cat3.h"
#include "brin.h"


#define BG_COLOUR RGB15(26, 25, 23)

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

// testing a few sprite things
// D-pad: move 
// SELECT: switch palette
// START: toggle mapping mode
// A: horizontal flip
// B: vertical flip
// L & R shift starting tile
void obj_test()
{
	int x= 96, y= 32;
	u32 tid= 0, pb= 0;		// tile id, pal-bank

	OBJ_ATTR *cat3= &obj_buffer[0];
	obj_set_attr(cat3, 
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_32,					// 64x64p, 
		ATTR2_PALBANK(pb) | tid);		// palbank 0, tile 0

	// position sprite (redundant here; the _real_ position
	// is set further down
	obj_set_pos(cat3, x, y);

	while(1)
	{
		vid_vsync();
		key_poll();

		// move left/right
		x += 2*key_tri_horz();

		// move up/down
		y += 2*key_tri_vert();

		// increment/decrement starting tile with R/L
		tid += bit_tribool(key_hit(-1), KI_R, KI_L);

		// flip
		if(key_hit(KEY_A))	// horizontally
			cat3->attr1 ^= ATTR1_HFLIP;
		if(key_hit(KEY_B))	// vertically
			cat3->attr1 ^= ATTR1_VFLIP;
		
		// make it glow (via palette swapping)
		pb= key_is_down(KEY_SELECT) ? 1 : 0;

		// toggle mapping mode
		if(key_hit(KEY_START))
			REG_DISPCNT ^= DCNT_OBJ_1D;

		// Hey look, it's one of them build macros!
		cat3->attr2= ATTR2_BUILD(tid, pb, 0);
		obj_set_pos(cat3, x, y);

		oam_copy(oam_mem, obj_buffer, 1);	// only need to update one
	}
}

int main()
{
	// Places the glyphs of a 4bpp boxed metroid sprite 
	//   into LOW obj memory (cbb == 4)
	memcpy(&tile_mem[4][0], cat3Tiles, cat3TilesLen);
	memcpy(pal_obj_mem, cat3Pal, cat3PalLen);
	// pal_bg_mem[0] = BG_COLOUR;
	memcpy(pal_bg_mem, brinPal, brinPalLen);
    memcpy(&tile_mem[0][0], brinTiles, brinTilesLen);
    memcpy(&se_mem[30][0], brinMap, brinMapLen);


	oam_init(obj_buffer, 128);

	REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_64x32;
	REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D | DCNT_MODE0 | DCNT_BG0;

	obj_test();

	while(1);

	return 0;
}
