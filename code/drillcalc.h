#ifndef DRILLCALC_H

struct drill_data
{
	char *Name;
	real32 BodyDiameter;
	real32 PointDiameter;
	real32 PointLength;
	real32 Minimum;
	real32 Maximum;
};

struct drill_angle_data
{
	char *Name;
	real32 Angle;
};

#define DRILLCALC_H
#endif
