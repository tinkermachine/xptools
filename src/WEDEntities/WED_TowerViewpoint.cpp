#include "WED_TowerViewpoint.h"

DEFINE_PERSISTENT(WED_TowerViewpoint)

WED_TowerViewpoint::WED_TowerViewpoint(WED_Archive * a, int i) : WED_GISPoint(a,i),
	height(this,"Height","WED_towerviewpoint","height",0)
{
}

WED_TowerViewpoint::~WED_TowerViewpoint()
{
}

void	WED_TowerViewpoint::SetHeight(double h)
{
	height = h;
}
