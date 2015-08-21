// HexCoord.h


typedef Vec3I HexCoord;
typedef Vec3 HexPos;

enum HexDir
{
	HEXDIR_E,
	HEXDIR_NE,
	HEXDIR_NW,
	HEXDIR_W,
	HEXDIR_SW,
	HEXDIR_SE,
};

int HexToIndex(HexCoord coord, Vec2I boardSize);
HexCoord HexPosRound(HexPos coord);

HexCoord HexDir(HexDir dir);
//HexPos HexPosDir(HexDir dir);

int HexDistance(HexCoord h0, HexCoord h1);
int HexLength(HexCoord h);
//float HexDistance(HexPos h0, HexPos h1);
//float HexLength(HexPos h);

HexCoord PixelToHex(Vec2I pixel, Vec2I tileSize);
Vec2I HexToPixel(HexCoord coord, Vec2I tileSize);
//Vec2I HexOffsetToPixel(Vec2I offset, Vec2I tileSize);
Vec2 HexOffsetToPixel(Vec2I offset, Vec2I tileSize);
HexCoord HexOffsetToCoord(Vec2I hexOffset);
