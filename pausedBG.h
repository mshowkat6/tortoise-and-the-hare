
//{{BLOCK(pausedBG)

//======================================================================
//
//	pausedBG, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 108 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 3456 + 2048 = 6016
//
//	Time-stamp: 2023-12-08, 12:37:29
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_PAUSEDBG_H
#define GRIT_PAUSEDBG_H

#define pausedBGTilesLen 3456
extern const unsigned short pausedBGTiles[1728];

#define pausedBGMapLen 2048
extern const unsigned short pausedBGMap[1024];

#define pausedBGPalLen 512
extern const unsigned short pausedBGPal[256];

#endif // GRIT_PAUSEDBG_H

//}}BLOCK(pausedBG)
