const int MM_JP1_START = 0x40B670;
const int MM_US0_START = 0x40b140;
const int MM_JP0_START = 0x40B3B0;
const int MM_JP_GC_START = 0x3A5870;
const int MM_JP0_LINK_SIZE = 0x30;
const int MM_JP1_LINK_SIZE = 0x30;
const int MM_US0_LINK_SIZE = 0x10;

enum SolverTypes {
	SuccessiveActorSolver,
	RandomAssortment,
	Test,
	DFSRM,
	OneRandomPerm,
	nop
};

enum Version
{
	MM_US,
	MM_JP,
	MM_JP1,
	MM_JP_GC
};

enum SolverStep
{
	ALLOCATE = 0xddd0,
	DEALLOCATE = 0xddd1,
	CHANGE_ROOM = 0xddd2,
	SUPERSLIDE = 0xddd3,
	USE_PLANE = 0xddd4,
	LOAD_INITIAL_ROOM = 0xddd5,
	CARRY = 0xddd6,
	DROP = 0xddd7,
	SPAWNERS = 0xddd8
};