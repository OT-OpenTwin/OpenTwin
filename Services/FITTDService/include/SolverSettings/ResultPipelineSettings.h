#pragma once
//Currently only 1D plot. Needs to be extended for different visualization types.
class ResultPipelineSettings
{
	int _coordinateX;
	int _coordinateY;
	int _coordinateZ;

public:
	ResultPipelineSettings(int coordinateX, int coordinateY, int coordinateZ): 
		_coordinateX(coordinateX), _coordinateY(coordinateY), _coordinateZ(coordinateZ) {};
};