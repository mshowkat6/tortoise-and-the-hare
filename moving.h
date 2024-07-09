
//{{BLOCK(moving)

//======================================================================
//
//	moving, 256x512@4, 
//	+ palette 256 entries, not compressed
//	+ 452 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x64 
//	Total size: 512 + 14464 + 4096 = 19072
//
//	Time-stamp: 2023-12-08, 12:23:13
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MOVING_H
#define GRIT_MOVING_H

#define movingTilesLen 14464
extern const unsigned short movingTiles[7232];

#define movingMapLen 4096
extern const unsigned short movingMap[2048];

#define movingPalLen 512
extern const unsigned short movingPal[256];

#endif // GRIT_MOVING_H

//}}BLOCK(moving)
