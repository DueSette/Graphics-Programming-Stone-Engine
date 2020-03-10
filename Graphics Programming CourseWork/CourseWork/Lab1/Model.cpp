#include "Model.h"
#include <vector>

#pragma region ===================================  MESH IMPLEMENTATION =============================
Mesh::Mesh()
	:drawCount(NULL) {}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertexArrayObject); // delete arrays
}

void Mesh::draw()
{
	glBindVertexArray(vertexArrayObject); //binds vao and draws elements in it
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); //unbinds vao
}

void Mesh::loadModel(const std::string& filename)
{
	IndexedModel model = OBJModel(filename).ToIndexedModel();
	initWithModel(model);

	print(("Mesh initialised with model named: " + filename), DebugMessageTier::INFO);
}

void Mesh::initWithModel(const IndexedModel& model)
{
	drawCount = model.indices.size();

	glGenVertexArrays(1, &vertexArrayObject); //generate a vertex array and store it in the VAO
	glBindVertexArray(vertexArrayObject); //bind the VAO (any operation that works on a VAO will work on our bound VAO - binding)

	glGenBuffers(NUM_BUFFERS, vertexArrayBuffers); //generate our buffers based of our array of data/buffers - GLuint vertexArrayBuffers[NUM_BUFFERS];

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VERTEXBUFFER]); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU (determined by type)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //adds information about the vertex layout

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); //adds information about the vertex layout
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0); //adds information about the vertex layout

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[INDEX_VB]); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU

	glBindVertexArray(0); // unbind our VAO
}

void Mesh::initWithoutModel(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
	IndexedModel model;

	for (unsigned int i = 0; i < numVertices; i++)
	{
		model.positions.push_back(*vertices[i].getPosition());
		model.texCoords.push_back(*vertices[i].getTexCoord());
		model.normals.push_back(*vertices[i].getNormal());
	}

	for (unsigned int i = 0; i < numIndices; i++)
		model.indices.push_back(indices[i]);

	initWithModel(model);
}
#pragma endregion

#pragma region ===================================  TEXTURE IMPLEMENTATION =============================

Texture::Texture(const std::string& fileName)
{
	int width, height, numComponents; //width, height, and no of components of image
	unsigned char* imageData = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4); //load the image and store the data

	if (imageData == NULL)
		std::cerr << "texture load failed" << fileName << std::endl;

	glGenTextures(1, &textureHandler); // number of and address of textures
	glBindTexture(GL_TEXTURE_2D, textureHandler); //bind texture - define type 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap texture outside width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // wrap texture outside height

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification (texture is smaller than area)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnifcation (texture is larger)

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); //Target, Mipmapping Level, Pixel Format, Width, Height, Border Size, Input Format, Data Type of Texture, Image Data

	stbi_image_free(imageData);

	const std::string msg = "Texture " + fileName + " loaded";
	print(msg, DebugMessageTier::INFO);
}

void Texture::Bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31); /// check we are working with one of the 32 textures

	glActiveTexture(GL_TEXTURE0 + unit); //set active texture unit
	glBindTexture(GL_TEXTURE_2D, textureHandler); //type of and texture to bind to unit
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureHandler); // number of and address of textures
}
#pragma endregion

Collider::~Collider()
{
}
