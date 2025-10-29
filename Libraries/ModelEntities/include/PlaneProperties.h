// @otlicense

#pragma once
#include <string>
#include <map>

class __declspec(dllexport)  PlaneProperties
{
public:
	enum NormalDescription
	{
		X,Y,Z,Free, UNKNOWN
	};

	const std::string GetPropertyNameNormal(void) const { return nameNormal; };
	const std::string GetPropertyNameCenterX(void) const { return nameCenterX; };
	const std::string GetPropertyNameCenterY(void) const { return nameCenterY; };
	const std::string GetPropertyNameCenterZ(void) const { return nameCenterZ; };
	
	const std::string GetPropertyNameNormalCooX(void) const { return nameNormalCooX; };
	const std::string GetPropertyNameNormalCooY(void) const { return nameNormalCooY; };
	const std::string GetPropertyNameNormalCooZ(void) const { return nameNormalCooZ; };

	std::string GetPropertyValueNormalX(void) const { return valueNormalX; };
	std::string GetPropertyValueNormalY(void) const { return valueNormalY; };
	std::string GetPropertyValueNormalZ(void) const { return valueNormalZ; };
	std::string GetPropertyValueNormalFree(void) const { return valueNormalFree;  };
	
	const NormalDescription GetNormalDescription(const std::string normal) const 
	{
		NormalDescription returnVal;
		normalDescriptionMapping.find(normal) == normalDescriptionMapping.end() ? returnVal = UNKNOWN : returnVal = normalDescriptionMapping.at(normal);
		return returnVal;
	};

private:
	const std::string nameNormal = "Normal";
	const std::string nameCenterX = "Center X";
	const std::string nameCenterY = "Center Y";
	const std::string nameCenterZ = "Center Z";
	const std::string nameNormalCooX = "Normal X";
	const std::string nameNormalCooY = "Normal Y";
	const std::string nameNormalCooZ = "Normal Z";
	
	const std::string valueNormalX = "X";
	const std::string valueNormalY = "Y";
	const std::string valueNormalZ = "Z";
	const std::string valueNormalFree = "Free";

	std::map<std::string, NormalDescription> normalDescriptionMapping{ {valueNormalX, X}, {valueNormalY, Y}, {valueNormalZ, Z}, {valueNormalFree, Free} };
};
