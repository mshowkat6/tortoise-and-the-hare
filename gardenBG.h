
//{{BLOCK(gardenBG)

//======================================================================
//
//	gardenBG, 512x512@4, 
//	+ palette 256 entries, not compressed
//	+ 942 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x64 
//	Total size: 512 + 30144 + 8192 = 38848
//
//	Time-stamp: 2023-12-07, 11:17:53
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_GARDENBG_H
#define GRIT_GARDENBG_H

#define gardenBGTilesLen 30144
extern const unsigned short gardenBGTiles[15072];

#define gardenBGMapLen 8192
extern const unsigned short gardenBGMap[4096];

#define gardenBGPalLen 512
extern const unsigned short gardenBGPal[256];

#endif // GRIT_GARDENBG_H

//}}BLOCK(gardenBG)
