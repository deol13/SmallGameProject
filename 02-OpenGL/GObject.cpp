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
	this->vert.push_back(vertices);
	this->drawMode = drawMode;
	this->gTexture = gTexture;
//	nrOfVertices = vertices.size();
	gBuffer = 0;
	for(int i = 0; i < vertices.size(); i++)
	{
		indices.push_back(i);
	}
	init();
}
GObject::GObject(std::string fileName, int drawMode, GLuint gTexture) {
	this->drawMode = drawMode;
	this->gTexture = gTexture;
	loadObjectFile(fileName, 0);
	init();
}

GObject::GObject(std::string* fileNames, int nrOfKeyFrames, GLuint texture)
{
	this->drawMode = GL_TRIANGLES;
	this->gTexture = texture;
	for( int i = 0; i < nrOfKeyFrames; i++ ) 
	{
		loadObjectFile(fileNames[i], i);
	}
	init();
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
		0.0f, sin(x), cos(x), 0.0f,
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
void GObject::setRotation(float x, float y, float z)
{
	glm::mat4 xMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(x), -sin(x), 0.0f,
		0.0f, sin(x), cos(x), 0.0f,
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
	rotationMatrix = (xMatrix * yMatrix * zMatrix);
}
void GObject::translate(float x, float y, float z) //set translation matrix
{
	translationMatrix[3][0] += x;
	translationMatrix[3][1] += y;
	translationMatrix[3][2] += z;
}

void GObject::loadObjectFile(std::string fileName, int keyFrame)
{
	vert.push_back(std::vector<Vertex>());

	std::string line;

	std::ifstream myfile("Resource/" + fileName);
	//long size = myfile.tellg();
	//myfile.seekg(0, std::ios::beg);
	//char* fileBuffer = new char[size];
	//myfile.read(fileBuffer, size);
	//vert[keyFrame].reserve(myfile.tellg() * 0.05f);
	//indices.reserve(myfile.tellg() * 0.05f);
	//myfile = std::ifstream("Resource/" + fileName);
	if(myfile.is_open())
	{
		//std::vector<Vertex> vert;
		std::vector<Vertex> uv;

		std::string sub;
		int vertCount = 0;
		int texCount = 0;
		int faceCount = 0;
 		int state = 0;

		//long charCount = 0;
		//while(charCount < size)
		//{
		//	if(fileBuffer[charCount] == 'v')
		//	{

		//	}

		//	charCount++;
		//}

		while(true) {
			if(!(getline(myfile, line))) break;
			if(line.size() < 5)
				continue;
			if(line[0] == 'v' && line[1] == ' ') // vertex pos
			{
				vert[keyFrame].push_back(Vertex());

				std::istringstream iss(line);
				std::string sub;
				iss >> sub; // discard 'v'
				iss >> sub;
				vert[keyFrame][vertCount].x = std::stof(sub);
				iss >> sub;
				vert[keyFrame][vertCount].y = std::stof(sub);
				iss >> sub;
				vert[keyFrame][vertCount].z = std::stof(sub);
				vert[keyFrame][vertCount].u = -1.0f;
				vert[keyFrame][vertCount].v = -1.0f;
				vertCount++;
			} else if(line[0] == 'v' && line[1] == 't') //UV cord
			{
				if(state != 1)
				{
					state = 1;
					//count = 0;
				}
				uv.push_back(Vertex());
				std::istringstream iss(line);
				std::string sub;
				iss >> sub; // discard 'vt'
				iss >> sub;
				uv[texCount].u = std::stof(sub);
				iss >> sub;
				uv[texCount].v = std::stof(sub);
				texCount++;
			} else if(line[0] == 'f') // face
			{
				if(state != 2)
				{
					state = 2;
					//count = 0;
				}
				std::istringstream iss(line);
				std::string sub;
				if(keyFrame == 0)
				{
					indices.push_back(0);
					indices.push_back(0);
					indices.push_back(0);
				}

				//vert
				iss >> sub; // discard 'f'

				for(int n = 0; n < 3; n++)
				{
					std::string pos;
					iss >> pos; // vertex index
					iss >> sub; // uv index
					int indexVERT = std::stoi(pos) - 1;
					int indexUV = std::stoi(sub) - 1;
					if(vert[keyFrame][indexVERT].u < 0)
					{
						indices[faceCount * 3 + n] = indexVERT; // set vertex index
						vert[keyFrame][indexVERT].u = uv[indexUV].u;
						vert[keyFrame][indexVERT].v = uv[indexUV].v;
					} 
					else if(vert[keyFrame][indexVERT].u == uv[indexUV].u && vert[keyFrame][indexVERT].v == uv[indexUV].v)			//Avoid duplicate vertices
					{
						indices[faceCount * 3 + n] = indexVERT;
					}else
					{
						int temp = indexVERT;
						indexVERT = vert[keyFrame].size();
						vert[keyFrame].push_back(Vertex());
						vert[keyFrame][indexVERT] = vert[keyFrame][temp];
						vert[keyFrame][indexVERT].u = uv[indexUV].u;
						vert[keyFrame][indexVERT].v = uv[indexUV].v;
						indices[faceCount * 3 + n] = indexVERT;
					}
					iss >> sub; // normal index
					sub = "";
				}
				faceCount++;
			}
		}
		GLushort indexTest = indices[indices.size()-1];
		nrOfVertices = faceCount;
	}
}

void GObject::bindBuffers()
{
	glGenBuffers(1, &gBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(currentVert[0]) * currentVert.size(), &currentVert[0], GL_STATIC_DRAW);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(currentVert[0])* currentVert.size(), &currentVert[0], GL_STATIC_DRAW);
	glDrawElements(drawMode, nrOfVertices * 3, GL_UNSIGNED_SHORT, 0);
}

void GObject::init()
{
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
	animationState = 0.0f;
	for(int i = 0; i < vert[0].size(); i++)
	{
		currentVert.push_back(vert[0][i]);
	}
	Vertex temp = currentVert[currentVert.size() - 1];
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

void GObject::setAnimationState(float state)
{
	animationState = state;
}

void GObject::animate(int stopState)
{
	int floorFrame = (int) animationState;
	int naiveFrame = floorFrame + 1;						//ceilFrame, not counting for looping back
	int ceilFrame = naiveFrame%(stopState  + 1);
	//Change vertices. Needs optimization
	for(int i = 0; i < currentVert.size(); i++)
	{
		currentVert[i].x = vert[floorFrame][i].x * (naiveFrame - animationState) + vert[ceilFrame][i].x * (animationState - floorFrame);
		currentVert[i].y = vert[floorFrame][i].y * (naiveFrame - animationState) + vert[ceilFrame][i].y * (animationState - floorFrame);
		currentVert[i].z = vert[floorFrame][i].z * (naiveFrame - animationState) + vert[ceilFrame][i].z * (animationState - floorFrame);
	}
	animationState += 0.1;
	if(animationState > stopState + 1)
	{
		animationState = 0.0f;
	}
}

int GObject::getNrOfVertices()
{
	return nrOfVertices;
}

GLfloat* GObject::returnWorldPosMat()
{
	return &translationMatrix[0][0];
}