
//{{BLOCK(finalInstruction)

//======================================================================
//
//	finalInstruction, 256x512@4, 
//	+ palette 256 entries, not compressed
//	+ 509 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x64 
//	Total size: 512 + 16288 + 4096 = 20896
//
//	Time-stamp: 2023-12-08, 11:50:58
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FINALINSTRUCTION_H
#define GRIT_FINALINSTRUCTION_H

#define finalInstructionTilesLen 16288
extern const unsigned short finalInstructionTiles[8144];

#define finalInstructionMapLen 4096
extern const unsigned short finalInstructionMap[2048];

#define finalInstructionPalLen 512
extern const unsigned short finalInstructionPal[256];

#endif // GRIT_FINALINSTRUCTION_H

//}}BLOCK(finalInstruction)
