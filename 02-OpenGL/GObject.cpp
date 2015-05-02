#include "GObject.h"
#include "stb_image.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

GObject::GObject()
{
	gTexture = 0;
	gBuffer = 0;
	gAttribute = 0;
//	nrOfVertices = 0;
	drawMode = 0;

	scaleMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	translationMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	rotationMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}
GObject::GObject(std::vector<Vertex> vertices, int drawMode, GLuint gTexture)
{
	this->vert = vertices;
	this->drawMode = drawMode;
	this->gTexture = gTexture;
//	nrOfVertices = vertices.size();
	gBuffer = 0;

	scaleMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	translationMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	rotationMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	for(int i = 0; i < vertices.size(); i++)
	{
		indices.push_back(i);
	}
	init();
}
GObject::GObject(std::string fileName, int drawMode, GLuint gTexture) {
	this->drawMode = drawMode;
	this->gTexture = gTexture;
	loadObjectFile(fileName);

	scaleMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	translationMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	rotationMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}
GObject::~GObject()
{
	//delete[] vert;
}

void GObject::scale(float x, float y, float z) //set scale matrix
{
	scaleMatrix[0][0] = x;
	scaleMatrix[1][1] = y;
	scaleMatrix[2][2] = z;
}
void GObject::rotate(float x, float y, float z) //set rotation matrix
{
	glm::mat4 xMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(x), -sin(x), 0.0f,
		0.0f, cos(x), cos(x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 yMatrix = glm::mat4(
		cos(y), 0.0f, -sin(y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(y), 0.0f, cos(y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 zMatrix = glm::mat4(
		cos(z), -sin(z), 0.0f, 0.0f,
		sin(z), cos(z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	rotationMatrix = ( xMatrix * yMatrix * zMatrix) * rotationMatrix;
}
void GObject::translate(float x, float y, float z) //set translation matrix
{
	translationMatrix[3][0] += x;
	translationMatrix[3][1] += y;
	translationMatrix[3][2] += z;
}

void GObject::loadObjectFile(std::string fileName)
{
	//vert = new std::vector<Vertex>[2];

	std::string line;
	std::ifstream myfile("Resource/" + fileName);
	if(myfile.is_open())
	{
		//std::vector<Vertex> vert;
		std::vector<Vertex> uv;

		std::string sub;
		int count = 0;
 		int state = 0;
		while(true) {
			if(!(getline(myfile, line))) break;
			if(line.size() < 5)
				continue;
			if(line[0] == 'v' && line[1] == ' ') // vertex pos
			{
				vert.push_back(Vertex());

				std::istringstream iss(line);
				std::string sub;
				iss >> sub; // discard 'v'
				iss >> sub;
				vert[count].x = std::stof(sub);
				iss >> sub;
				vert[count].y = std::stof(sub);
				iss >> sub;
				vert[count].z = std::stof(sub);
				vert[count].u = -1.0f;
				vert[count].v = -1.0f;
				count++;
			} else if(line[0] == 'v' && line[1] == 't') //UV cord
			{
				if(state != 1)
				{
					state = 1;
					count = 0;
				}
				uv.push_back(Vertex());
				std::istringstream iss(line);
				std::string sub;
				iss >> sub; // discard 'vt'
				iss >> sub;
				uv[count].u = std::stof(sub);
				iss >> sub;
				uv[count].v = std::stof(sub);
				count++;
			} else if(line[0] == 'f') // face
			{
				if(state != 2)
				{
					state = 2;
					count = 0;
				}
				std::istringstream iss(line);
				std::string sub;
				indices.push_back(0);
				indices.push_back(0);
				indices.push_back(0);
				//vert
				iss >> sub; // discard 'f'

				for(int n = 0; n < 3; n++)
				{
					std::string pos;
					iss >> pos; // vertex index
					iss >> sub; // uv index
					int indexVERT = std::stoi(pos) - 1;
					int indexUV = std::stoi(sub) - 1;
					if(vert[indexVERT].u < 0)
					{
						indices[count * 3 + n] = indexVERT; // set vertex index
						vert[indexVERT].u = uv[indexUV].u;
						vert[indexVERT].v = uv[indexUV].v;
					} else
					{
						int temp = indexVERT;
						indexVERT = vert.size();
						vert.push_back(Vertex());
						vert[indexVERT] = vert[temp];
						vert[indexVERT].u = uv[indexUV].u;
						vert[indexVERT].v = uv[indexUV].v;
						indices[count * 3 + n] = indexVERT;
					}
					iss >> sub; // normal index
					sub = "";
				}
				count++;
			}
		}
		nrOfVertices = count;
	}

	init();
}

void GObject::bindBuffers()
{
	glGenBuffers(1, &gBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(currentVert[0]) * indices.size(), &currentVert[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(IndexBufferId);

	glGenVertexArrays(1, &gAttribute);
	glBindVertexArray(gAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * nrOfVertices * 3, &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float)* 3));


}


void GObject::render(GLint uniLocation, GLuint shaderProgram)
{
	glm::mat4 worldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	glProgramUniformMatrix4fv(shaderProgram, uniLocation, 1, false, &worldMatrix[0][0]);

	glBindVertexArray(gAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, gBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);

	//reset animation frame
	if(animationState > 1.0f)
	{
		animationState = 0.0f;
	}
	//Change vertices. Needs optimization
	for(int i = 0; i < currentVert.size(); i++)
	{
		currentVert[i].x = vert[i].x * (1 - animationState) + vert2[i].x * animationState;
		currentVert[i].y = vert[i].y * (1 - animationState) + vert2[i].y * animationState;
		currentVert[i].z = vert[i].z * (1 - animationState) + vert2[i].z * animationState;
	}
	animationState += 0.01;

	glBufferData(GL_ARRAY_BUFFER, sizeof(currentVert[0])* indices.size(), &currentVert[0], GL_STATIC_DRAW);

	glDrawElements(drawMode, nrOfVertices * 3, GL_UNSIGNED_SHORT, 0);
}

void GObject::init()
{
	animationState = 0.0f;
	for(int i = 0; i < vert.size(); i++)
	{
		currentVert.push_back(vert[i]);
		vert2.push_back(vert[i]);
		vert2[i].x += 200;
		vert2[i].z += 200;
	}
	bindBuffers();
}

void GObject::setTexture(GLuint gTexture)
{
	this->gTexture = gTexture;
}
GLuint GObject::getTexture()
{
	return gTexture;
}
void GObject::setVertices(std::vector<Vertex> vertices)
{
	currentVert = vertices;
}
std::vector<Vertex> GObject::getVertices()
{
	return currentVert;
}

