#pragma once

class VtkDriver;
class EntityVis2D3D;

class VtkDriverFactory
{
public:
	static VtkDriver *createDriver(EntityVis2D3D *visEntity);

};

