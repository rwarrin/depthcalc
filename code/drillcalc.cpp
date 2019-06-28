#include "drillcalc.h"

#define PI 3.14159265358979323846264338f

internal real32
CalculateDrillDepth(drill_data *DrillData, real32 Diameter)
{
	real32 Result = 0.0f;
	real32 PointAngleRad = ((31.0f*PI)/180.0f);

	real32 HalfPointDiameter = DrillData->PointDiameter/2.0f;
	real32 TipHeight = HalfPointDiameter*tanf(PointAngleRad);

	real32 BodyAngleRad = ((30.0f*PI)/180.0f);
	real32 HalfBodyDiameter = ((Diameter - DrillData->PointDiameter)/2.0f);
	real32 BodyHeight = HalfBodyDiameter*(1.0f/tanf(BodyAngleRad));

	Result = TipHeight + BodyHeight + DrillData->PointLength;
	return(Result);
}

internal real32
CalculateDrillAngleDepth(drill_angle_data *AngleData, real32 DrillDiameter, real32 TipDiameter)
{
	real32 Result = 0.0f;
	real32 DrillAngleRad = (((AngleData->Angle/2.0f)*PI)/180.0f);

	real32 HalfBodyDiameter = DrillDiameter/2.0f;
	real32 BodyHeight = HalfBodyDiameter*(1.0f/tanf(DrillAngleRad));

	real32 TipHeight = 0.0f;
	if(TipDiameter != FLT_MIN)
	{
		real32 HalfTipDiameter = TipDiameter/2.0f;
		TipHeight = HalfTipDiameter*(1.0f/tanf(DrillAngleRad));
	}

	Result = BodyHeight - TipHeight;
	return(Result);
}
