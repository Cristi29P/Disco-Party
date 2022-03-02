#include "lab_m1/Tema3/Tema3.h"
#include <iostream>

using namespace m1;

Mesh* Tema3::CreateCone(const std::string& name, glm::vec3 bottomCenter, glm::vec3 topCoord, float radius, glm::vec3 color)
{
	std::vector<VertexFormat> vertices;

	unsigned int points = 50;
	float twicePI = 2.0f * (float)M_PI;

	vertices.push_back(VertexFormat(topCoord, color));

	for (unsigned int i = 0; i != points; i++) {
		float x_coord = radius * cos(i * twicePI / points);
		float y_coord = radius * sin(i * twicePI / points);
		vertices.push_back(VertexFormat(bottomCenter + glm::vec3(x_coord, 0.f, y_coord), color));
	}

	Mesh* cone = new Mesh(name);

	std::vector<unsigned int> indices;

	for (unsigned int i = 1; i <= points; i++) {
		if (i == points) {
			indices.push_back(1);
			indices.push_back(i);
			indices.push_back(0);
		}
		else {
			indices.push_back(i + 1);
			indices.push_back(i);
			indices.push_back(0);
		}
	}

	cone->SetDrawMode(GL_TRIANGLES);

	cone->InitFromData(vertices, indices);
	return cone;
}

float Tema3::GetRandomFloat(float min, float max) {
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<> distr(min, max);
	return (float)distr(eng);
}

glm::vec3 Tema3::GenerateRandomColor(float min, float max) {
	float x = GetRandomFloat(min, max);
	float y = GetRandomFloat(min, max);
	float z = GetRandomFloat(min, max);
	return glm::vec3(x, y, z);
}

glm::vec3 Tema3::GetNearestCellCoord(glm::vec3 coord) {
	float x_coord = coord.x;
	float z_coord = coord.z;
	float x_intpart, x_fractpart, z_intpart, z_fractpart;

	x_fractpart = std::modf(x_coord, &x_intpart);
	z_fractpart = std::modf(z_coord, &z_intpart);

	float clamped_x = x_intpart + ((x_fractpart < 0) ? -0.5f : 0.5f);
	float clamped_z = z_intpart + ((z_fractpart < 0) ? -0.5f : 0.5f);

	return glm::vec3(clamped_x, -0.005f, clamped_z);
}

std::vector<glm::vec3> Tema3::GetNeighbourCells(glm::vec3 cell) {
	std::vector<glm::vec3> neighbours;
	for (float x = cell.x - 1.f; x <= cell.x + 1.f; x += 1.f) {
		if (x >= -3.5f && x <= 3.5f) {
			for (float z = cell.z - 1.f; z <= cell.z + 1.f; z += 1.f) {
				if (z >= -3.5f && z <= 3.5f) {
					neighbours.push_back(glm::vec3(x, -0.005f, z));
				}
			}
		}
	}
	return neighbours;
}

void Tema3::InitCamera()
{
	camera = new SceneCamera(glm::vec3(0.f, 2.f, 2.f), glm::vec3(0.f, 2.f, 0.f), 2.f, glm::vec3(0.f, 1.f, 0.f),
		glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f));
}

void Tema3::InitWalls() {
	Walls wall1(glm::vec3(-4.005f, 3.f, 0.f), glm::vec3(0.01f, 6.f, 8.f)); // zid stanga
	Walls wall2(glm::vec3(0.f, 3.f, -4.005f), glm::vec3(8.f, 6.f, 0.01f)); // zid spate
	Walls wall3(glm::vec3(4.005f, 3.f, 0.f), glm::vec3(0.01f, 6.f, 8.f)); // zid dreapta
	Walls wall4(glm::vec3(0.f, 6.005f, 0.f), glm::vec3(8.f, 0.01f, 8.f)); // tavan

	walls.push_back(wall1);
	walls.push_back(wall2);
	walls.push_back(wall3);
	walls.push_back(wall4);
}

std::vector<glm::vec3> Tema3::ReturnWallLights(glm::vec3 position, glm::vec3 scale) {
	std::vector<glm::vec3> result;
	if (scale.z == 0.01f) { // zid spate
		for (float x = -3.5f; x <= 3.5f; x += 1.f) {
			result.push_back(glm::vec3(x, -0.005f, -3.5f));
		}
	}
	else { // ziduri stanga/dreapta
		float temp_x = position.x < 0 ? -3.5f : 3.5f;
		for (float z = -3.5f; z <= 3.5f; z += 1.f) {
			result.push_back(glm::vec3(temp_x, -0.005f, z));
		}
	}
	return result;
}

void Tema3::InitWallLights() {
	for (int i = 0; i < 3; i++) { // de la 0 la 2 sunt peretii
		walls.at(i).lightFromFloor = ReturnWallLights(walls.at(i).position, walls.at(i).scale);
	}
}

void Tema3::InitFloorTiles() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			glm::vec3 randomColor = GenerateRandomColor(0.f, 1.f);
			FloorTile tile(glm::vec3(i - 3.5f, -0.005f, j - 3.5f), glm::vec3(1.f, 0.01f, 1.f), randomColor);
			FloorLight light(glm::vec3(i - 3.5f, 0.f, j - 3.5f), randomColor);
			tiles.push_back(tile);
			floorLights.push_back(light);
			position_tileLights.insert({ glm::vec3(i - 3.5f, -0.005f, j - 3.5f), std::make_pair(tiles.back(), floorLights.back()) });
		}
	}
}

void Tema3::InitMeshes() {
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* cone = CreateCone("cone", glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), 1.f, glm::vec3(1.f, 0.f, 0.f));
		AddMeshToList(cone);
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;

		globe_texture = CreateRandomSphere(16, 16);
	}
}

void Tema3::InitShaders() {
	{
		Shader* shader = new Shader("FloorShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FloorShaderVertex.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FloorShaderFragment.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("WallShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "WallShaderVertex.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "WallShaderFragment.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("DancerShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "DancerShaderVertex.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "DancerShaderFragment.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("ConeShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "ConeShaderVertex.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "ConeShaderFragment.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("GlobeShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "GlobeShaderVertex.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "GlobeShaderFragment.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("CeilingShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "CeilingShaderVertex.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "CeilingShaderFragment.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Tema3::InitDancers() {
	for (int i = 0; i < 6; i++) {
		glm::vec3 randPos = GenerateRandomColor(-3.f, 3.f);
		randPos.y = 0.5f;

		glm::vec3 randDest = GenerateRandomColor(-3.5f, 3.5f);
		randDest.y = 0.f;
		Dancer dancer(randPos, glm::vec3(0.5f, 1.f, 0.5f), randDest, GetRandomFloat(0.3f, 2.f));
		dancers.push_back(dancer);
	}
}

void Tema3::InitSpotlights() {
	Spotlight spotlight_1(glm::vec3(2.f, 0.f, 2.f), glm::vec3(tan(spotAngle), 1, tan(spotAngle)) * coneHeight,
		GenerateRandomColor(0.f, 1.f), glm::vec3(0, -1, 0), spotAngle, glm::vec3(1.f, 0.f, 0.f));
	Spotlight spotlight_2(glm::vec3(2.f, 0.f, -2.f), glm::vec3(tan(spotAngle), 1, tan(spotAngle)) * coneHeight,
		GenerateRandomColor(0.f, 1.f), glm::vec3(0, -1, 0), spotAngle, glm::vec3(0.f, 0.f, 1.f));
	Spotlight spotlight_3(glm::vec3(-2.f, 0.f, -2.f), glm::vec3(tan(spotAngle), 1, tan(spotAngle)) * coneHeight,
		GenerateRandomColor(0.f, 1.f), glm::vec3(0, -1, 0), spotAngle, glm::vec3(1.f, 0.f, 0.f));
	Spotlight spotlight_4(glm::vec3(-2.f, 0.f, 2.f), glm::vec3(tan(spotAngle), 1, tan(spotAngle)) * coneHeight,
		GenerateRandomColor(0.f, 1.f), glm::vec3(0, -1, 0), spotAngle, glm::vec3(0.f, 0.f, 1.f));
	spotlights.push_back(spotlight_1);
	spotlights.push_back(spotlight_2);
	spotlights.push_back(spotlight_3);
	spotlights.push_back(spotlight_4);
}

void Tema3::Init()
{
	InitCamera();
	InitWalls();
	InitWallLights();
	InitFloorTiles();
	InitDancers();
	InitSpotlights();
	InitMeshes();
	InitShaders();
}

void Tema3::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema3::DrawWalls() {
	for (int i = 0; i < 3; i++) {
		std::vector<glm::vec3> lightColors;
		for (auto x : walls.at(i).lightFromFloor) {
			lightColors.push_back(position_tileLights.at(x).second.light_color);
		}

		RenderWallLight(meshes["box"], shaders["WallShader"], walls.at(i).m_modelMatrix, walls.at(i).lightFromFloor, lightColors, spotlights, globe_texture);
	}

	RenderCeiling(meshes["box"], shaders["CeilingShader"], walls.at(3).m_modelMatrix, globe_texture);
}

void Tema3::DrawFloor() {
	for (const auto& tile : tiles) {
		RenderFloorLight(meshes["box"], shaders["FloorShader"], tile.m_modelMatrix, tile.emissive_color, spotlights, globe_texture);
	}
}

void Tema3::DrawDancers() {
	for (const auto& dancer : dancers) {
		std::vector<glm::vec3> localTiles = GetNeighbourCells(GetNearestCellCoord(dancer.position));
		std::vector<FloorLight> floorLights;

		for (const auto& x : localTiles) {
			floorLights.push_back(position_tileLights.at(x).second);
		}
		RenderDancerLight(meshes["box"], shaders["DancerShader"], dancer.m_modelMatrix, floorLights, spotlights, globe_texture);
	}
}

void Tema3::DrawGlobe() {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, disco_position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2.f));
	RenderGlobe(meshes["sphere"], shaders["GlobeShader"], modelMatrix, globe_texture);
}

void Tema3::DrawSpotlights() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (const auto& spotlight : spotlights) {
		RenderCone(meshes["cone"], shaders["ConeShader"], spotlight.m_modelMatrix, spotlight.color);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void Tema3::DrawScene() {
	DrawWalls();
	DrawFloor();
	DrawDancers();
	DrawGlobe();
	DrawSpotlights();
}

void Tema3::UpdateDancers(float deltaTimeSeconds) {
	for (auto& dancer : dancers) {
		if (glm::distance(dancer.position, dancer.temp_destination) < 0.2f ||
			abs(dancer.initialPosition.x + dancer.translateX + deltaTimeSeconds * dancer.temp_direction.x * dancer.speed) >= 3.5f ||
			abs(dancer.initialPosition.z + dancer.translateZ + deltaTimeSeconds * dancer.temp_direction.z * dancer.speed) >= 3.5f) {
			dancer.temp_destination = GenerateRandomColor(-3.5f, 3.5f);
			dancer.temp_destination.y = 0;
			dancer.temp_direction = glm::normalize(dancer.temp_destination - dancer.position);
		}
		dancer.Update(deltaTimeSeconds);
	}
}

void Tema3::UpdateSpotlights(float deltaTimeSeconds) {
	if (0.2f * deltaTimeSeconds <= maximumAngle) {
		for (auto& spotlight : spotlights) {
			if (abs(spotlight.cummulatedAngle) >= maximumAngle) {
				spotlight.cummulatedAngle = maximumAngle * spotlight.semnTermen;
				spotlight.semnTermen *= -1;
			}

			spotlight.Update(0.2f * deltaTimeSeconds * spotlight.semnTermen);
			if (spotlight.axisOfRotation == glm::vec3(1.0f, 0.f, 0.f)) {
				spotlight.lightDirection = glm::rotate(glm::mat4(1), 0.2f * deltaTimeSeconds * spotlight.semnTermen, glm::vec3(1, 0, 0)) * glm::vec4(spotlight.lightDirection, 0.f);
				spotlight.lightDirection = glm::normalize(spotlight.lightDirection);
			}
			else {
				spotlight.lightDirection = glm::rotate(glm::mat4(1), 0.2f * deltaTimeSeconds * spotlight.semnTermen, glm::vec3(0, 0, 1)) * glm::vec4(spotlight.lightDirection, 0.f);
				spotlight.lightDirection = glm::normalize(spotlight.lightDirection);
			}
		}
	}
}

void Tema3::Update(float deltaTimeSeconds)
{
	Time = (float)Engine::GetElapsedTime();

	UpdateDancers(deltaTimeSeconds);
	UpdateSpotlights(deltaTimeSeconds);
	DrawScene();
}

void Tema3::FrameEnd()
{
}

void Tema3::RenderCone(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& cone_color) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->projectionMatrix;
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Send emitted color
	glUniform3f(glGetUniformLocation(shader->program, "cone_color"), cone_color.x, cone_color.y, cone_color.z);

	// Switches
	glUniform1f(glGetUniformLocation(shader->program, "spotlightSwitch"), spotlightSwitch);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderFloorLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& emitted_color, const std::vector<Spotlight>& spotlights, Texture2D* texture) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->projectionMatrix;
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Send eye position
	glm::vec3 eyePosition = camera->position;
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Send emitted color
	glUniform3f(glGetUniformLocation(shader->program, "emissive_component"), emitted_color.x, emitted_color.y, emitted_color.z);

	// Send spot angle
	glUniform1f(glGetUniformLocation(shader->program, "spot_angle"), spotAngle);

	// Send material constants
	glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), 50);

	glUniform1f(glGetUniformLocation(shader->program, "material_kd"), 0.8f);

	glUniform1f(glGetUniformLocation(shader->program, "material_ks"), 0.1f);

	glm::vec3 pointLightPos[4];
	glm::vec3 pointLightColor[4];
	glm::vec3 pointLightDirection[4];

	for (int i = 0; i < 4; i++) {
		pointLightPos[i] = glm::vec3(spotlights[i].position.x, 6.f, spotlights[i].position.z);
		pointLightColor[i] = spotlights[i].color;
		pointLightDirection[i] = spotlights[i].lightDirection;
	}

	GLuint location = glGetUniformLocation(shader->program, "pointLightPos");
	glUniform3fv(location, 4, glm::value_ptr(pointLightPos[0]));

	location = glGetUniformLocation(shader->program, "pointLightColor");
	glUniform3fv(location, 4, glm::value_ptr(pointLightColor[0]));

	location = glGetUniformLocation(shader->program, "pointLightDirection");
	glUniform3fv(location, 4, glm::value_ptr(pointLightDirection[0]));

	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "Time"), Time);

	glUniform3f(glGetUniformLocation(shader->program, "globe_position"), disco_position.x, disco_position.y, disco_position.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Switches
	glUniform1f(glGetUniformLocation(shader->program, "floorSwitch"), floorSwitch);
	glUniform1f(glGetUniformLocation(shader->program, "spotlightSwitch"), spotlightSwitch);
	glUniform1f(glGetUniformLocation(shader->program, "discoSwitch"), discoSwitch);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderWallLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, std::vector<glm::vec3> lightPos, std::vector<glm::vec3> lightColor, const std::vector<Spotlight>& spotlights, Texture2D* texture) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->projectionMatrix;
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::vec3 eyePosition = camera->position;
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	glm::vec3 pointLightPos[8];
	glm::vec3 pointLightColor[8];

	for (int i = 0; i < 8; i++) {
		pointLightPos[i] = lightPos[i];
		pointLightColor[i] = lightColor[i];
	}

	GLuint location = glGetUniformLocation(shader->program, "pointLightPos");
	glUniform3fv(location, 8, glm::value_ptr(pointLightPos[0]));

	location = glGetUniformLocation(shader->program, "pointLightColor");
	glUniform3fv(location, 8, glm::value_ptr(pointLightColor[0]));

	// Send material properties
	glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), 30);

	glUniform1f(glGetUniformLocation(shader->program, "material_kd"), 0.8f);

	glUniform1f(glGetUniformLocation(shader->program, "material_ks"), 0.8f);

	// Send spot angle
	glUniform1f(glGetUniformLocation(shader->program, "spotAngle"), spotAngle);

	glm::vec3 spotLightDirection[4];
	glm::vec3 spotLightPos[4];
	glm::vec3 spotLightColor[4];

	for (int i = 0; i < 4; i++) {
		spotLightPos[i] = glm::vec3(spotlights[i].position.x, 6.f, spotlights[i].position.z);
		spotLightColor[i] = spotlights[i].color;
		spotLightDirection[i] = spotlights[i].lightDirection;
	}

	location = glGetUniformLocation(shader->program, "spotLightDirection");
	glUniform3fv(location, 4, glm::value_ptr(spotLightDirection[0]));

	location = glGetUniformLocation(shader->program, "spotLightPos");
	glUniform3fv(location, 4, glm::value_ptr(spotLightPos[0]));

	location = glGetUniformLocation(shader->program, "spotLightColor");
	glUniform3fv(location, 4, glm::value_ptr(spotLightColor[0]));

	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "Time"), Time);

	glUniform3f(glGetUniformLocation(shader->program, "globe_position"), disco_position.x, disco_position.y, disco_position.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Switches
	glUniform1f(glGetUniformLocation(shader->program, "floorSwitch"), floorSwitch);
	glUniform1f(glGetUniformLocation(shader->program, "spotlightSwitch"), spotlightSwitch);
	glUniform1f(glGetUniformLocation(shader->program, "discoSwitch"), discoSwitch);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderDancerLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, std::vector<FloorLight> floorLights, const std::vector<Spotlight>& spotlights, Texture2D* texture) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->projectionMatrix;
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::vec3 eyePosition = camera->position;
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	glm::vec3 pointLightPos[9];
	glm::vec3 pointLightColor[9];

	glUniform1i(glGetUniformLocation(shader->program, "number_of_lights"), (int)floorLights.size());

	for (int i = 0; i < (int)floorLights.size(); i++) {
		pointLightPos[i] = floorLights[i].position;
		pointLightColor[i] = floorLights[i].light_color;
	}

	GLuint location = glGetUniformLocation(shader->program, "pointLightPos");
	glUniform3fv(location, 9, glm::value_ptr(pointLightPos[0]));

	location = glGetUniformLocation(shader->program, "pointLightColor");
	glUniform3fv(location, 9, glm::value_ptr(pointLightColor[0]));

	// Send spot angle
	glUniform1f(glGetUniformLocation(shader->program, "spot_angle"), spotAngle);

	// Spotlight uniforms
	glm::vec3 spotLightDirection[4];
	glm::vec3 spotLightPos[4];
	glm::vec3 spotLightColor[4];

	for (int i = 0; i < 4; i++) {
		spotLightPos[i] = glm::vec3(spotlights[i].position.x, 6.f, spotlights[i].position.z);
		spotLightColor[i] = spotlights[i].color;
		spotLightDirection[i] = spotlights[i].lightDirection;
	}

	location = glGetUniformLocation(shader->program, "spotLightDirection");
	glUniform3fv(location, 4, glm::value_ptr(spotLightDirection[0]));

	location = glGetUniformLocation(shader->program, "spotLightPos");
	glUniform3fv(location, 4, glm::value_ptr(spotLightPos[0]));

	location = glGetUniformLocation(shader->program, "spotLightColor");
	glUniform3fv(location, 4, glm::value_ptr(spotLightColor[0]));

	// Send material properties
	glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), 20);

	glUniform1f(glGetUniformLocation(shader->program, "material_kd"), 1.2f);

	glUniform1f(glGetUniformLocation(shader->program, "material_ks"), 0.2f);

	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "Time"), Time);

	glUniform3f(glGetUniformLocation(shader->program, "globe_position"), disco_position.x, disco_position.y, disco_position.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Switches
	glUniform1f(glGetUniformLocation(shader->program, "floorSwitch"), floorSwitch);
	glUniform1f(glGetUniformLocation(shader->program, "spotlightSwitch"), spotlightSwitch);
	glUniform1f(glGetUniformLocation(shader->program, "discoSwitch"), discoSwitch);

	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderCeiling(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->projectionMatrix;
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::vec3 eyePosition = camera->position;
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), 50);

	glUniform1f(glGetUniformLocation(shader->program, "material_kd"), 0.8f);

	glUniform1f(glGetUniformLocation(shader->program, "material_ks"), 0.1f);

	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "Time"), (GLfloat)Engine::GetElapsedTime());

	glm::vec3 globe_position = disco_position;

	glUniform3f(glGetUniformLocation(shader->program, "globe_position"), globe_position.x, globe_position.y, globe_position.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Switches
	glUniform1f(glGetUniformLocation(shader->program, "discoSwitch"), discoSwitch);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderGlobe(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = camera->projectionMatrix;
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform1f(glGetUniformLocation(shader->GetProgramID(), "Time"), (GLfloat)Engine::GetElapsedTime());

	glm::vec3 globe_position = disco_position;

	glUniform3f(glGetUniformLocation(shader->program, "globe_position"), globe_position.x, globe_position.y, globe_position.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Switches
	glUniform1f(glGetUniformLocation(shader->program, "discoSwitch"), discoSwitch);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

Texture2D* Tema3::CreateRandomSphere(unsigned int width, unsigned int height)
{
	GLuint textureID = 0;
	unsigned int channels = 3;
	unsigned int size = width * height * channels;
	unsigned char* data = new unsigned char[size];

	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {
			data[(i * width + j) * 3 + 0] = (char)(rand() % 255);
			data[(i * width + j) * 3 + 1] = (char)(rand() % 255);
			data[(i * width + j) * 3 + 2] = (char)(rand() % 255);
		}
	}

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	if (GLEW_EXT_texture_filter_anisotropic) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	CheckOpenGLError();

	// Use glTexImage2D to set the texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	CheckOpenGLError();

	// Save the texture into a wrapper Texture2D class for using easier later during rendering phase
	Texture2D* texture = new Texture2D();
	texture->Init(textureID, width, height, channels);

	SAFE_FREE_ARRAY(data);
	return texture;
}

void Tema3::OnInputUpdate(float deltaTime, int mods)
{
	float cameraSpeed = 3.0f;
	if (window->KeyHold(GLFW_KEY_W)) {
		camera->MoveForward(cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		camera->TranslateRight(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_S)) {
		camera->MoveForward(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_D)) {
		camera->TranslateRight(cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_Q)) {
		camera->TranslateUpward(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_E)) {
		camera->TranslateUpward(cameraSpeed * deltaTime);
	}
}

void Tema3::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_P) {
		floorSwitch = (floorSwitch == 1.f) ? 0.f : 1.f;
	}

	if (key == GLFW_KEY_R) {
		spotlightSwitch = (spotlightSwitch == 1.f) ? 0.f : 1.f;
	}

	if (key == GLFW_KEY_G) {
		discoSwitch = (discoSwitch == 1.f) ? 0.f : 1.f;
	}
}

void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
		camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
	}
}