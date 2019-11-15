#include "HeightMap.h"

HeightMap::HeightMap(std::string name) {

	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	colours = new Vector4[numVertices];
	indices = new GLuint[numIndices];

	if (name.empty())
	{
		GenerateHeightMap();
		return;
	}
	std::ifstream file(name.c_str(), ios::binary);
	if (!file) 
	{
		return;
	}
	
	unsigned char* data = new unsigned char[numVertices];
	file.read((char*)data, numVertices * sizeof(unsigned char));
	
	file.close();
	GenerateHeightMap(data);
}



void HeightMap::Draw()
{
	Mesh::Draw();
}

void HeightMap::GenerateHeightMap(unsigned char* data)
{

	for (int x = 0; x < RAW_WIDTH; ++x) {
		for (int z = 0; z < RAW_HEIGHT; ++z) {
			int offset = (x * RAW_WIDTH) + z;

			vertices[offset] = Vector3(
				x * HEIGHTMAP_X, data==nullptr? 0 : data[offset] * HEIGHTMAP_Y, z * HEIGHTMAP_Z);
			textureCoords[offset] = Vector2(
				x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
			//cout << (int)data[offset] << endl;
			//colours[offset] = Vector4(1,1,1, 1.0f);
		}
	}
	numIndices = 0;

	for (int x = 0; x < RAW_WIDTH - 1; ++x) {
		for (int z = 0; z < RAW_HEIGHT - 1; ++z) {
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();
}
