#pragma once
#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "CameraTema2.h"

class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;

	Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

protected:
	Tema::CameraPerson* camera;
	glm::mat4 projectionMatrix;
	bool renderCameraTarget;


	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;

	float planeDirection_y;
	float planeDirection_x;

	float translatingLine; // used for translating 
	float translatingLine_secondSet;
	float translatingLine_clouds[12];
	float curbeLine_clouds;
	int randomNumber;// used for generating random points/obstacles


	int points; //number of points to win
	int points_secondSet;
	int obstacles;// number of obstacles
	float yOffsetObstacle; 
	float obstacle_yOffsets[6];  ////offets between obstacles on Y
	float curbeLine;
	float planeRotation;

	int isSpot ;
	bool flag ;
	int lives ;
	int bonuses;
	float fuel;
	float angle;
	int startPoints_firstSet;
	int startPoints_secondSet;
	int startObstacles;
	bool lost;
	float eliceRotate;
	bool toActualize;
	float lastPlaneDirection;
	float end;
	float lastPlaneDir_xz;
	bool lastMove;

	bool firstCamera;
	bool thirdCamera;
};
