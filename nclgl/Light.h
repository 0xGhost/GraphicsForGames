#pragma once

#include "Vector4.h"
#include "Vector3.h"

enum LightType
{
	PointLight = 1,
	DirectionLight = 1 << 1,
	SpotLight = 1 << 2,
};

class Light { // point light
public:
	Light()
	{

	}

	Light(Vector3 position, Vector4 colour, Vector4 specularColour, LightType type = PointLight, float radius = 0, Vector3 direction = Vector3(0, -1, 0), float angle = 0.0f)
	{
		this->position = position;
		this->colour = colour;
		this->specularColour = specularColour;
		this->radius = radius;
		this->direction = direction;
		this->angle = angle;
		this->type = type;
	}

	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

	Vector4 GetSpecularColour() const { return specularColour; }
	void SetSpecularColour(Vector4 val) { specularColour = val; }

	Vector3 GetDirection() const { return direction; }
	void SetDirection(Vector3 d) { direction = d; }

	float GetAngle() const { return angle; }
	void SetAngle(float a) { angle = a; }

	LightType GetType() const { return type; }
	void SetLightType(LightType t) { type = t; }

protected:
	Vector3 position;
	Vector4 colour;
	Vector4 specularColour;
	float radius;
	Vector3 direction;
	float angle;
	LightType type;
};