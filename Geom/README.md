# Geometry wars clone

## Configuration file:

The configuration file will have one line each specifying the window size, font format, player, bullet specification and enemy specifications. Each line will have the folloeing syntax:

Window W H FL FS
	- This line declares that the SFML Window must be constructed with Width W and height H, both integers. FL is the frame limit that the window will be set to, and FS will be an integer which specifies whether to display the application in full-sceen mode (1) or not (0).

Font F S R G B
	- This line defines the font which is to be used to draw text for this program
	Font File	F			std::string (it will have no spaces)
	Font Size	S			int
	RGB	Color	(R, G, B)	int, int, int

Player SR CR S FR FG FB OR OG OB OT V
	Shape Radius		SR			float
	Collision Radius	CR			float
	Speed				S			float
	Fill Color			FR, FG, FB	int, int, int
	Outline Color		OR, OG, OB	int, int, int
	Outlint thickness	OT			float
	Vertices			V			int

Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
	Shape Radius		SR			float
	Collision Radius	SR			float
	Min / Max Speed		SMIN, SMAX	float, float
	Outline Color		OR, OG, OB	int, int, int
	Outline Thickness	OT			float
	Max / Min Vertices	VMIN, VMAX	int, int
	Small Lifespan		L			int
	Spawn Interval		SP			int

Bullet SR CR S FR FG FB OR OG OB OT V L
	Shape Radius		SR			float
	Collision Radius	CR			float
	Speed				S			float
	Fill Color			FR, FG, FB	int, int, int
	Outline Color		OR, OG, OB	int, int, int
	Outlint thickness	OT			float
	Vertices			V			int
	Life Span			L			int