#pragma once
#include "OGLRenderer.h"

enum MeshBuffer
{
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh();
	~Mesh();
	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad(Vector3 position[4] = nullptr);
	static Mesh* GenerateCube();


	void *colorPtr;
	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }
	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap() { return bumpTexture; }

protected:
	void BufferData();

	void GenerateNormals();
	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3& a, const Vector3& b,
		const Vector3& c, const Vector2& ta,
		const Vector2& tb, const Vector2& tc);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	GLuint numIndices;
	unsigned int* indices;
	GLuint texture;
	Vector2* textureCoords;
	Vector3* vertices;
	Vector4* colours;
	Vector3* normals;
	Vector3* tangents;
	GLuint bumpTexture;
};

