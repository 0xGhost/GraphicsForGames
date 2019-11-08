#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light(Vector3 position, Vector4 colour, Vector4 specularColour, float radius) {
		this->position = position;
		this->colour = colour;
		this->specularColour = specularColour;
		this->radius = radius;
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

protected:
	Vector3 position;
	Vector4 colour;
	Vector4 specularColour;
	float radius;
};