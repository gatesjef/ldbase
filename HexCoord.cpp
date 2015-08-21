// HexCoord.cpp

#include "Precompile.h"
#include "HexCoord.h"

HexCoord HEX_DIRECTIONS[] = {
	{ 1, -1,  0 },
	{ 1,  0, -1 },
	{ 0,  1, -1 },
	{-1,  1,  0 },
	{-1,  0,  1 },
	{ 0, -1,  1 },
};


Vec3I RoundEl(Vec3 v)
{
	Vec3I retval;
	retval.x = FloatRoundToInt(v.x);
	retval.y = FloatRoundToInt(v.y);
	retval.z = FloatRoundToInt(v.z);
	return retval;
}

Vec3I AbsEl(Vec3I v)
{
	Vec3I retval;
	retval.x = abs(v.x);
	retval.y = abs(v.y);
	retval.z = abs(v.z);
	return retval;
}

Vec3 AbsEl(Vec3 v)
{
	Vec3 retval;
	retval.x = Abs(v.x);
	retval.y = Abs(v.y);
	retval.z = Abs(v.z);
	return retval;
}

float SumEl(Vec3 v)
{
	return v.x+v.y+v.z;
}

int SumEl(Vec3I v)
{
	return v.x+v.y+v.z;
}

int HexLength(HexCoord h)
{
	HexCoord absH = AbsEl(h);
	int sum = SumEl(absH);
	return sum / 2;
}

float HexLength(HexPos h)
{
	HexPos absH = AbsEl(h);
	float sum = SumEl(absH);
	return sum / 2;
}

float HexDistance(HexPos h0, HexPos h1)
{
	return HexLength(h0 - h1);
}

int HexDistance(HexCoord h0, HexCoord h1)
{
	return HexLength(h0 - h1);
}

HexCoord HexRange(HexCoord center, int radius)
{
	for(int dx = -radius; dx <= radius; dx++)
	{
		int ymin = Max(-radius, -dx - radius);
		int ymax = Min(radius, -dx + radius);
		for(int dy = ymin; dy <= ymax; ++dy)
		{
			int dz = -dx-dy;
			//yield center + V3I(dx, dy, dz);
			if (1) return center + V3I(dx, dy, dz);
		}
	}

	XHalt();
}

Vec2I HexOffsetToPos(Vec2I hexOffset, Vec2I tileSize)
{
	XAssert((tileSize.y / 4) * 4 == tileSize.y);
	XAssert((tileSize.x / 2) * 2 == tileSize.x);

	Vec2I retval;
	retval.x = hexOffset.x * tileSize.x + (hexOffset.y&1) * tileSize.x/2;
	retval.y = hexOffset.y * tileSize.y * 3 / 4;
	return retval;
}

HexCoord HexOffsetToCoord(Vec2I hexOffset)
{
	int col = hexOffset.x;
	int row = hexOffset.y;

	HexCoord retval;
	retval.x = col - (row - (row&1)) / 2;
	retval.z = row;
	retval.y = -retval.x-retval.z;
	return retval;
}

Vec2 HexOffsetToPixel(Vec2I offset, Vec2I tileSize)
{
	return V2(HexOffsetToPos(offset, tileSize));
}

Vec2I PixelToHexOffset(Vec2I pixel, Vec2I tileSize)
{
	////7 x (8,4)
	//14 x (16,8)
	//28 x (32,16)
	//56 x (64,32)

	//0,0 is the LR center pixel of the 0,0 hex
	//

	XAssert((tileSize.y / 4) * 4 == tileSize.y);	// height must be a multiple of 4 (or we could pass in the tile_y_stride)
	XAssert((tileSize.x / 2) * 2 == tileSize.x);	// width must be a multiple of 2   
	int rowThird = tileSize.y / 4;
	int rowHeight = rowThird * 3;
	int colHalf = tileSize.x / 2;
	int colWidth = colHalf * 2;

	int guess_row = pixel.y / rowHeight;
	int guess_col = pixel.x / colWidth;

	Vec2I guess_point[3];
	if (guess_row & 1)
	{
		guess_point[0] = V2I(guess_col,   guess_row);
		guess_point[1] = V2I(guess_col,   guess_row+1);
		guess_point[2] = V2I(guess_col+1, guess_row+1);
	}
	else
	{
		guess_point[0] = V2I(guess_col, guess_row);
		guess_point[1] = V2I(guess_col+1, guess_row);
		guess_point[2] = V2I(guess_col, guess_row+1);
	}

	Vec2I center[3];
	center[0] = HexOffsetToPos(guess_point[0], tileSize);
	center[1] = HexOffsetToPos(guess_point[1], tileSize);
	center[2] = HexOffsetToPos(guess_point[2], tileSize);

	int dist2[3];
	dist2[0] = SqrDist(center[0], pixel);
	dist2[1] = SqrDist(center[1], pixel);
	dist2[2] = SqrDist(center[2], pixel);

	int best;
	if (dist2[0] < dist2[1] && dist2[0] < dist2[2])
	{
		best = 0;
	}
	else if (dist2[1] < dist2[2])
	{
		best = 1;
	}
	else
	{
		best = 2;
	}

	return guess_point[best];
}

HexCoord PixelToHex(Vec2I pixel, Vec2I tileSize)
{
	Vec2I offset = PixelToHexOffset(pixel, tileSize);
	return HexOffsetToCoord(offset);
}


	//XAssert((tileSize.y / 4) * 4 == tileSize.y);
	//int rowThird = tileSize.y / 4;
	//int rowHeight = rowThird * 3;

	//XAssert((tileSize.x / 2) * 2 == tileSize.x);
	//int colHalf = tileSize.x / 2;

	//int whichColHalf = pixel.x / colHalf;
	//int whichRow = pixel.y / rowHeight;
	//int whichRowThird = pixel.y / rowThird;

	//int rise = whichRowThird;
	//int run = whichColHalf;

	//bool flipped = (whichColHalf&1)^(whichRow&1);

	//BBox
	// whichRowThird * rowThird <= x < (whichRowThird+1) * rowThird
	// whichColHalf  * colHalf <= y < (whichColHalf+1)  * colHalf

	//bress from (near corner, far corner + 1y);


	//switch(whichRowThird-whichRow*3)
	//{
	//case 0:
	//	int row = whichRow:
	//case 1:
	//	if (testNear)
	//	{
	//		row = whichRow;
	//		col = .
	//	}
	//	else
	//	{
	//		row = whichRow+1:
	//	}
	//case 2:
	//	int row = whichRow+1:

	//}

	//int col = 

	// even whichColHalf means slop from 


	//int row = pixel.y / rowHeight;
	//int rowR = pixel.y % rowHeight;
	//int col = pixel.x / tileSize.x;
	//int colR = pixel.x % tileSize.x;


	//if (rowR < rowThird)
	//{
	//	// row is good!
	//}
	//else if (rowR < rowThird*2)
	//{
	//	// on border

	//}
	//else 
	//{
	//	// actually in next row!
	//	row++;
	//}



	//if (colR > colHalf)
	//{
	//	col++;
	//}




//}

//Vec2 HexToCartesian(HexPos pos)
//{
//	Vec2 retval;
//	retval.x = ROOT_3 * (pos.x + pos.z/2);
//	retval.y = 1.5f * pos.z;
//	return retval;
//}
//
//HexPos CartesianToHex(Vec2 pos)
//{
//	HexPos retval;
//	retval.x = pos.x * (ROOT_3 - pos.y)/3.0f;
//	retval.z = pos.y * 2.0f/3.0f;
//	retval.y = -retval.x-retval.z;
//	return retval;
//}

HexCoord HexRound(HexPos pos)
{
	HexCoord retval;
	retval = RoundEl(pos);

	float dx = Abs(retval.x - pos.x);
	float dy = Abs(retval.y - pos.y);
	float dz = Abs(retval.z - pos.z);

	if (dx > dy && dx > dz)
	{
		retval.x = -retval.y - retval.z;
	}
	else if (dy > dz)
	{
		retval.y = -retval.x - retval.z;
	}
	else
	{
		retval.z = -retval.x - retval.y;
	}

	return retval;
}

int HexCoordToIndex(HexCoord coord, Vec2I boardSize)
{
	// odd-r horizontal layout
	int row = coord.x + (coord.z - (coord.z&1)) / 2;
	int col = coord.z;

	return boardSize.x * col + row;
}

//HexCoord HexPosRound(HexPos coord)
//{
//
//}

