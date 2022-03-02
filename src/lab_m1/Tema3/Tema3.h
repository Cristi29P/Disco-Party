#pragma once
#include "components/simple_scene.h"
#include "lab_m1/Tema3/SceneCamera.h"
#include "lab_m1/Tema3/Walls.h"
#include "lab_m1/Tema3/FloorTile.h"
#include "lab_m1/Tema3/FloorLight.h"
#include "lab_m1/Tema3/Dancer.h"
#include "lab_m1/Tema3/Spotlight.h"

#include <math.h>
#include <vector>
#include <random>
#include <unordered_map>
#include "glm/gtx/hash.hpp"

namespace m1 {
	class Tema3 : public gfxc::SimpleScene {
	public:
		void Init() override;

	private:
		Mesh* CreateCone(const std::string& name, glm::vec3 bottomCenter, glm::vec3 topCoord, float radius, glm::vec3 color);
		float GetRandomFloat(float min, float max);
		glm::vec3 GenerateRandomColor(float min, float max);
		glm::vec3 GetNearestCellCoord(glm::vec3 coord);
		std::vector<glm::vec3> GetNeighbourCells(glm::vec3 cell);

		void InitCamera();
		void InitWalls();
		void InitFloorTiles();
		void InitMeshes();
		void InitShaders();
		void InitWallLights();
		void InitSpotlights();
		void InitDancers();

		std::vector<glm::vec3> ReturnWallLights(glm::vec3 position, glm::vec3 scale);

		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderCone(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& cone_color);
		void RenderFloorLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& emitted_color, const std::vector<Spotlight>& spotlights, Texture2D* texture1);
		void RenderWallLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, std::vector<glm::vec3> lightPos, std::vector<glm::vec3> lightColor, const std::vector<Spotlight>& spotlights, Texture2D* texture1);
		void RenderDancerLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, std::vector<FloorLight> floorLights, const std::vector<Spotlight>& spotlights, Texture2D* texture1);
		void RenderGlobe(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1);
		void RenderCeiling(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1);

		Texture2D* CreateRandomSphere(unsigned int width, unsigned int height);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

		void DrawWalls();
		void DrawFloor();
		void DrawDancers();
		void DrawGlobe();
		void DrawSpotlights();
		void DrawScene();

		void UpdateSpotlights(float deltaTimeSeconds);
		void UpdateDancers(float deltaTimeSeconds);

	private:
		SceneCamera* camera = nullptr;

		std::vector<Dancer> dancers;
		std::vector<Walls> walls; // Zidurile laterale si tavanul
		std::vector<FloorTile> tiles; // Podeaua

		std::vector<FloorLight> floorLights; // Luminile din podea

		std::vector<Spotlight> spotlights; // reflectoarele

		// UPDATE MANUAL LA CULORILE DIN HARTA
		// mapare intre pozitia casutelor de pe podea si casutele in sine cu luminile aferente
		std::unordered_map<glm::vec3, std::pair<FloorTile, FloorLight>> position_tileLights;

		float maximumAngle = RADIANS(10.f);
		float spotAngle = RADIANS(20.f);
		float coneHeight = 6.25f;

		float Time = 0.f;

		Texture2D* globe_texture;
		glm::vec3 disco_position = glm::vec3(0.f, 5.f, 0.f);

		float floorSwitch = 1.f;
		float spotlightSwitch = 1.f;
		float discoSwitch = 1.f;
	};
}