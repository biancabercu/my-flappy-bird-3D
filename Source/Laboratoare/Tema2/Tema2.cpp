#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;
Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

float orthoLeftT = -8.0f;
float orthoRightT = 8.0f;
float orthoUpT = 4.0f;
float orthoDownT = -4.5f;
float FOVtema = glm::radians(60.0f);
float LENGTHtema = 4.0f;

void Tema2::Init()
{
	renderCameraTarget = false;

	camera = new Tema::CameraPerson();
	camera->Set(glm::vec3(0, 2, 10), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	// Create a simple cube --lab 6
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 0, 1), glm::vec3(0.2, 0.8, 0.2)),
			VertexFormat(glm::vec3(1, -1,  1), glm::vec3(0, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
			VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(0, 0, 1), glm::vec3(0.7, 0.7, 0.1)),
			VertexFormat(glm::vec3(1,  1,  1),  glm::vec3(0, 0, 1), glm::vec3(0.7, 0.3, 0.7)),
			VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(0, 0, 1), glm::vec3(0.3, 0.5, 0.4)),
			VertexFormat(glm::vec3(1, -1, -1),  glm::vec3(0, 0, 1), glm::vec3(0.5, 0.2, 0.9)),
			VertexFormat(glm::vec3(-1,  1, -1),  glm::vec3(0, 0, 1), glm::vec3(0.7, 0.0, 0.7)),
			VertexFormat(glm::vec3(1,  1, -1),  glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
		};

		vector<unsigned short> indices =
		{
			0, 1, 2,		1, 3, 2,
			2, 3, 7,		2, 7, 6,
			1, 7, 3,		1, 5, 7,
			6, 7, 4,		7, 5, 4,
			0, 4, 1,		1, 4, 5,
			2, 6, 4,		0, 2, 4,
		};

		CreateMesh("cube", vertices, indices);
	}

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("plane");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("oil");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "oildrum.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("ShaderT");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}


	//Light & material properties
	{
		lightPosition = glm::vec3(0, 1, 1);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 30;
		materialKd = 1;
		materialKs = 1;
	}

	//GAME SETUP ---------------------------------
	//20 30..90 +100 pt offset si pt random points nr ala care vine 2 3 ..9 -2
	randomNumber = rand() % 5 + 2; //2 la 7
	points = randomNumber - 1; // 1 la 6
	obstacles = points / 2 + 1; // 0 1 1 2 2 3+  1???
	lastMove = false;
	planeRotation = 0;
	planeDirection_y = 0;
	planeDirection_x = 0;
	translatingLine = 0; // used for translating '
	translatingLine_secondSet = -2;
	curbeLine = 0;
	curbeLine_clouds = 0;
	angle = 0;
	isSpot = 0;
	flag = false;
	lives = 3;
	bonuses = 0;
	fuel = 0.5f;
	eliceRotate = 0;
	startPoints_firstSet = 0;
	startPoints_secondSet = 0;
	startObstacles = 0;
	lost = false;
	toActualize = false;
	end = false;
	lastPlaneDirection = 1.5f;
	lastPlaneDir_xz = 0;
	firstCamera = false;
	thirdCamera = false;
	//cout << "LA INCEPUT " << endl;
	//cout << "POINTS " << points << endl;
	//cout << "OBSTCL " << obstacles << endl;
	for (int i = 0; i < 6; i++) {
		obstacle_yOffsets[i] = (rand() % 4 + 2) * 0.1 + 1.1f;
		//cout << "YOFFST " << obstacle_yOffsets[i] << endl;
	}
	for (int i = 0; i < 12; i++) {
		translatingLine_clouds[i] = 0;
	}

	projectionMatrix = glm::perspective(FOVtema, window->props.aspectRatio, 0.01f, 200.0f);
}


Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
{
	unsigned int VAO = 0;
	// TODO: Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// TODO: Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// TODO: Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// TODO: Crete the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// TODO: Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}


void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
	if (firstCamera) {
		/*if(planeDirection_y>1)
		camera->Set(glm::vec3(0, 2, 10), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0));*/
	}

	if (lives == 0)
		lost = true;
	// Render the camera target. Useful for understanding where is the rotation point in Third-person camera movement
	if (renderCameraTarget)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}
	

	// WATER
	{
		angle += 1.5;
		if (angle == 180)
			angle = 0;

		for (int i = 0; i < 90; i+=20) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.9, 2));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(angle), glm::vec3(0, 0, 1));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(i), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
			RenderSimpleMesh(meshes["cube"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0, 1));
		}
		for (int i = 0; i < 90; i += 20) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.4, 0.85, 2));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(angle), glm::vec3(0, 0, 1));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(i), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
			RenderSimpleMesh(meshes["cube"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0, 1));
		}
		for (int i = 0; i < 90; i += 20) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.4, 0.8, 2));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(angle), glm::vec3(0, 0, 1));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(i), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
			RenderSimpleMesh(meshes["cube"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0, 1));
		}
		for (int i = 0; i < 90; i += 20) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.6, 0.75, 2));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(angle), glm::vec3(0, 0, 1));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(i), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
			RenderSimpleMesh(meshes["cube"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0, 1));
		}
		for (int i = 0; i < 90; i += 20) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.6, 0.75, 2));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(angle), glm::vec3(0, 0, 1));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(i), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
			RenderSimpleMesh(meshes["cube"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0, 1));
		}

	}


	////PENTRU REFERINTA la y=1
	//{
	//	glm::mat4 modelMatrix = glm::mat4(1);
	//	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
	//	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
	//	RenderSimpleMesh(meshes["sphere"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 0, 0));
	//}


	//PENTRU REFERINTA rotatie PLANE
	{
		//glm::mat4 modelMatrix = glm::mat4(1); //20 rad era bn 
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeDirection_y, 0));
	/*	planeRotation = planeDirection_y * 10 + 2;
		float direction =0;
		if (planeDirection_y > 1)
			 direction = -1;
		else if (planeDirection_y < 1)
			 direction = 1;*/

			 /*	modelMatrix = glm::rotate(modelMatrix, RADIANS(direction*planeRotation), glm::vec3(1, 0, -1));
				 modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f));
				 RenderSimpleMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(1, 0, 0));*/
	}

	if (lives == 0 || fuel <0.05) {
		lost = true;
		if (!lastMove) {
			lastMove = true;
			lastPlaneDirection = planeDirection_y;
		}
	}
	

	if (translatingLine > 8) {
		translatingLine = 0;
		toActualize = true;
	}

	if (!lost) {
		//PLANE
		{
			planeRotation = planeDirection_y * 10 + 2;
			float direction = 0;
			if (planeDirection_y > 1)
				direction = -1;
			else if (planeDirection_y < 1)
				direction = 1;

			//elice 
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f, planeDirection_y, 0));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(translatingLine * 1000), glm::vec3(1, 0, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.015f, 0.05f, 0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 1, 1));

			}

			//beak
			{//era 1.5 pt y 
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f, planeDirection_y, 0));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(direction * planeRotation), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 1, 0));
			}

			// head
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeDirection_y, 0));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(direction * planeRotation), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 0, 0));
			}
			// wing1
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeDirection_y, 0.1f));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0.8, 0));
			}
			// wing2
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeDirection_y, -0.1f));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(direction * planeRotation), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix), glm::vec3(0, 0.8, 0);
			}
		}

		//ALIVE OBJECTS
		{
			if (translatingLine > 8 || toActualize) {
				translatingLine = 0;
				curbeLine = 0;

				randomNumber = rand() % 5 + 2; //de la 2 la 7
				points = randomNumber - 1; //de la 1 la 6  adica 1 2 3 4 5 6
				obstacles = points / 2 + 1; // 0 1 0 2 2 3 +1 va fi 1 2 1 3 3 4

				startObstacles = 0;
				startPoints_firstSet = 0;
				startPoints_secondSet = 0;
				toActualize=false;
			}

			if (translatingLine < 4)
				curbeLine += 0.002;
			else
				curbeLine -= 0.002;

			//POINTS FIRST SET
			glm::mat4 modelMatrix = glm::mat4(1);
			for (int i = startPoints_firstSet; i < points; i++) {

				int rotateAxis = i % 2;
				int xAxis = rotateAxis - 1;
				int yAxis = rotateAxis;
				int zAxis = rotateAxis + 1;
				//points


				float xPoint = 4.0f - translatingLine + i * 0.1;
				float yPoint = 0.7f + curbeLine;

				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(xPoint, yPoint, 0));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(translatingLine * 100), glm::vec3(xAxis, yAxis, zAxis));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.07f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0, 1));

				//checking for bonuses
				float minDistanceY = (yPoint * 10) - (planeDirection_y * 10);
				if (xPoint< 0 && xPoint> -0.05f && minDistanceY<1 && minDistanceY>-2) {
					//won a bonus
					bonuses++;
					if (fuel < 0.5)
						fuel += 0.1;
					if (startPoints_firstSet < points)
						startPoints_firstSet++;
				}

			}

			//POINTS SECOND SET
			for (int i = startPoints_secondSet; i < points; i++) {

				int rotateAxis = i % 2;
				int xAxis = rotateAxis - 1;
				int yAxis = rotateAxis;
				int zAxis = rotateAxis + 1;
				//points


				float xPoint = 4.0f - translatingLine + i * 0.1 - 2;
				float yPoint = 0.7f + curbeLine;

				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(xPoint, yPoint, 0));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(translatingLine * 100), glm::vec3(xAxis, yAxis, zAxis));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.07f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0, 1));

				//checking for bonuses
				float minDistanceY = (yPoint * 10) - (planeDirection_y * 10);
				if (xPoint< 0 && xPoint> -0.05f && minDistanceY<1 && minDistanceY>-2) {
					//won a bonus
					bonuses++;
					if (fuel < 0.5)
						fuel += 0.1;
					if (startPoints_secondSet < points)
						startPoints_secondSet++;
				}

			}

			//OBSTACLES
			for (int i = startObstacles; i < obstacles; i++) {
				//obstacles
				if (translatingLine == 0) {
					obstacle_yOffsets[i] = (rand() % 5 + 2) * 0.1 + 1;// max 1.7 si min 1.2
				}
				float xObstacle = 4.0f - translatingLine + i * 0.5;
				float yObstacle = obstacle_yOffsets[i];
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(xObstacle, yObstacle, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
				RenderSimpleMesh(meshes["sphere"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 0, 0));

				//checking for collisions
				float minDistanceY = (yObstacle * 10) - (planeDirection_y * 10);
				if (xObstacle< 0 && xObstacle> -0.05f && minDistanceY<0.5f && minDistanceY>-1) {
					//BAM
					lives--;
					if (startObstacles < obstacles)
						startObstacles++;
				}
			}

			translatingLine += 0.01;
		}
	}
	else if (lost && !end) {
	//PLANE
		{
		 
			if (eliceRotate == 180)
				eliceRotate = 0;

			//{
			//	glm::mat4 modelMatrix = glm::mat4(1); //20 rad era bn 
			//	modelMatrix = glm::translate(modelMatrix, glm::vec3(lastPlaneDir_xz, lastPlaneDirection, lastPlaneDir_xz));
			//	modelMatrix = glm::rotate(modelMatrix, RADIANS(eliceRotate), glm::vec3(1, 0, -1));
			//	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f));
			//	RenderSimpleMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(1, 0, 0));
			//}


		//PLANE
		{
		
			//elice 
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f+ lastPlaneDir_xz, lastPlaneDirection, lastPlaneDir_xz));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(eliceRotate), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.015f, 0.05f, 0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 1, 1));

			}
			//beak
			{//era 1.5 pt y 
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f+ lastPlaneDir_xz, lastPlaneDirection, lastPlaneDir_xz));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(eliceRotate), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 1, 0));
			}

			// head
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(lastPlaneDir_xz, lastPlaneDirection, lastPlaneDir_xz));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(eliceRotate), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 0, 0));
			}
			// wing1
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(lastPlaneDir_xz, lastPlaneDirection, 0.1f+ lastPlaneDir_xz));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(eliceRotate), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(0, 0.8, 0));
			}
			// wing2
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(lastPlaneDir_xz, lastPlaneDirection, -0.1f+ lastPlaneDir_xz));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(eliceRotate), glm::vec3(1, 0, -1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.15f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix), glm::vec3(0, 0.8, 0);
			}
		}


			eliceRotate += 5;
			lastPlaneDirection -= 0.009;
			lastPlaneDir_xz += 0.001;
			
			if(lastPlaneDirection<0)
				end = true;
		}
	}
	translatingLine += 0.01;

	// always render lives
	{
		glm::mat4 modelMatrix;
		for (int i = 0; i < lives; i++) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.3 + i * 0.1, 1.9f, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
			RenderSimpleMesh(meshes["sphere"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 0, 0));;
		}
	}

	// always render fuel
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 1.9, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel, 0.1f, 0.15f));
		RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix), glm::vec3(1, 1, 1);

		if (fuel > 0)
			fuel -= 0.0005;
	}

	//CLOUDS
	{
		for (int j = 0; j < 9; j++) {

			float clouds_xCoord = 4.0f - translatingLine_clouds[j] + j * 0.5;

			if (clouds_xCoord < -4) {
				translatingLine_clouds[j] = 0;
				clouds_xCoord = 4.0f - translatingLine_clouds[j] + j * 0.5;
			}

			for (int i = 0; i < 3; i++) {
				int rotateAxis = i % 2;
				int xAxis = rotateAxis - 1;
				int yAxis = rotateAxis;
				int zAxis = rotateAxis + 1;
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(clouds_xCoord + i * 0.1, 1 + j % 2 * 0.5, -1));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(translatingLine * 100), glm::vec3(xAxis, yAxis, zAxis));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 1, 1));
			}
			for (int i = 0; i < 4; i++) {
				int rotateAxis = i % 2;
				int xAxis = rotateAxis - 1;
				int yAxis = rotateAxis;
				int zAxis = rotateAxis + 1;
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(clouds_xCoord + i * 0.1, 0.9f + j % 2 * 0.5, -1));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(translatingLine * 100), glm::vec3(xAxis, yAxis, zAxis));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderT"], modelMatrix, glm::vec3(1, 1, 1));
			}
			translatingLine_clouds[j] += 0.01;
		}
	}


	// Render the point light in the scene
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 4, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 4, 5));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 4, 3));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 4, 7));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
	}

	// Render ground
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -5));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(70), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f,0.2f,0.3f));
		RenderSimpleMesh(meshes["plane"], shaders["ShaderT"], modelMatrix, glm::vec3(0.2, 0.7, 0.9));

		 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -2));
		//modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.2f, 0.3f));
		RenderSimpleMesh(meshes["plane"], shaders["ShaderT"], modelMatrix, glm::vec3(0.2, 0.7, 0.9));

	}
}


void Tema2::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	//spotlight
	int isSpot_location = glGetUniformLocation(shader->program, "isSpot");
	glUniform1i(isSpot_location, isSpot);


	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	//// TODO: Set eye position (camera position) uniform
	//glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	//int eye_position = glGetUniformLocation(shader->program, "eye_position");
	//glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// TODO: Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			// TODO : translate the camera forward
			camera->TranslateForward(deltaTime * cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// TODO : translate the camera to the left
			camera->TranslateRight(deltaTime * cameraSpeed * (-1));
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// TODO : translate the camera backwards
			camera->TranslateForward(deltaTime * cameraSpeed * (-1));
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// TODO : translate the camera to the right
			camera->TranslateRight(deltaTime * cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// TODO : translate the camera down
			camera->TranslateUpword(deltaTime * cameraSpeed * (-1));;
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// TODO : translate the camera up
			camera->TranslateUpword(deltaTime * cameraSpeed);
		}
	}
}

void Tema2::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
	}
	if (key==GLFW_KEY_C) {
		if (firstCamera == false)
			firstCamera = true;
		else {
			thirdCamera = true;
			firstCamera = false;
		}
			
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (firstCamera) {
			renderCameraTarget = false;
			// TODO : rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
			camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
		}

		if (thirdCamera) {
			renderCameraTarget = true;
			// TODO : rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
			camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
		}

	}
	//avionul care urmareste mouse-ul??
	{
		//float sensivityOX = 0.007f;
		float sensivityOY = 0.0065f;

		planeDirection_y = -sensivityOY * mouseY + 3;
	}
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
