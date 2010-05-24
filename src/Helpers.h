#ifndef _H_HELPERS_
#define _H_HELPERS_

class IngInit
{
public:
	enum IngType
	{
		BOTTOM_BUN, MEAT, LETTUCE, RED_STUFF, YELLOW_STUFF, TOP_BUN

		/*        The red stuff is a slice of tomato, and the yellow stuff
			is cheese.  This was not known at the time when this
			enumeration was defined...
		*/
	};

	int xInitTile, yInitTile, yTargetTile, rank;
	IngType type;
};

struct IntPair
{
	int first, second;
};


struct IntQuad
{
	int first, second, third, fourth;
};



#endif
