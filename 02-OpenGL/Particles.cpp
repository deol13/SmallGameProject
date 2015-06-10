#include "Particles.h"

using namespace glm;
#define MAX_PARTICLES 10

Particles::Particles()
{
	sizeOfPL = 0;
	sizeOfPA = 0;
	particlesArray = nullptr;
	particlesLife = nullptr;
}

Particles::~Particles()
{
	if (particlesArray != nullptr)
		delete[] particlesArray;
	glDeleteBuffers(1, &paticleVBO);
}

void Particles::createPaticleData(float startPosX, float startPosZ)  //Second shader storage buffer, stores particle data.
{
	Particle* tmp = nullptr;
	Particle* ptr = nullptr;
	//Get the current particle data from buffer.
	if (sizeOfPL > 0)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, paticleVBO);
		ptr = (Particle*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		tmp = ptr;

		if (particlesArray != nullptr)
		{
			delete[] particlesArray;
			particlesArray = nullptr;
		}
	}

	//Save the current particle data into the new particlesArray[]
	sizeOfPL++;
	particlesArray = new Particles::Particle[MAX_PARTICLES * sizeOfPL];
	for (int i = 0; i < sizeOfPA; i++)
	{
		particlesArray[i] = tmp[i];
	}

	//Create the new Particle data,
	createNewParticles(startPosX, startPosZ, ptr);

	//Unmap the particle data.
	if (ptr)
	{
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		ptr = nullptr;
	}

	//Bind over the old buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, paticleVBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particles::Particle)*sizeOfPA, &particlesArray[0], GL_STATIC_READ);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, paticleVBO);

	ptr = nullptr;
	tmp = nullptr;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
}

void Particles::createNewParticles(float startPosX, float startPosZ, Particle* ptr)
{
	int* fTmp = nullptr;
	vec3 velocity;
	vec3 rPos;
	float pLife = 60;

	if (particlesLife != nullptr)
	{
		fTmp = particlesLife;
		particlesLife = nullptr;
	}
	particlesLife = new int[MAX_PARTICLES * sizeOfPL];

	if (fTmp != nullptr)
	{
		for (int i = 0; i < MAX_PARTICLES * (sizeOfPL - 1); i++)
		{
			particlesLife[i] = fTmp[i];
		}
	}
	for (int i = sizeOfPA; i < MAX_PARTICLES * sizeOfPL; i++)
	{
		particlesLife[i] = pLife;
	}

	//Calc velocity
	velocity = vec3(0.0f, 0.0f, -0.5f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(-0.35f, 0.0f, -0.5f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(-0.5f, 0.0f, -0.2f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(-0.5f, 0.0f, 0.2f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(-0.5f, 0.0f, 0.35f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(-0.5f, 0.0f, 0.0f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(0.35f, 0.0f, 0.5f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(0.2f, 0.0f, 0.5f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(0.5f, 0.0f, -0.2f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	velocity = vec3(0.5f, 0.0f, -0.35f);
	rPos = vec3(startPosX, 5.0f, startPosZ);
	particlesArray[sizeOfPA] = Particle(rPos, velocity, pLife);
	sizeOfPA++;

	if (fTmp != nullptr)
	{
		delete[] fTmp;
		fTmp = nullptr;
	}
}

void Particles::removeParticleData() //FIXA
{
	Particle* ptr = nullptr;
	int tmpCounter = 0;

	if (sizeOfPA == 0)
	{}
	else if (sizeOfPA == MAX_PARTICLES)
	{
		sizeOfPL = 0;
		sizeOfPA = 0;
		particlesLife[0] = 0;

		delete[] particlesArray;
		particlesArray = nullptr;
	}
	else
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, paticleVBO);
		ptr = (Particle*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

		for (int i = MAX_PARTICLES; i < sizeOfPA; i++)
		{
			particlesArray[tmpCounter] = ptr[i];
			tmpCounter++;
		}
		sizeOfPA -= MAX_PARTICLES;

		if (ptr)
		{
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			ptr = nullptr;
		}

		tmpCounter = 0;
		counter--;
		sizeOfPL--;
		for (int i = 0; i < sizeOfPL; i++)
			particlesLife[i] = particlesLife[i + 1];

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, paticleVBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particles::Particle)*sizeOfPA, &particlesArray[0], GL_STATIC_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, paticleVBO);
	}
}