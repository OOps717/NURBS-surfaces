#include <cmath>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "Light/directionalLight.h"
#include "material.h"
#include "Computational/nurbs.h"

Window mainWindow(1366, 768);
Camera camera;

Shader directionalShadowShader;

DirectionalLight light;

// Material dullMaterial;

std::vector<Mesh*> meshList;
Shader sceneShader;
Shader controlPointShader;
GLuint controlPointVAO = 0, controlPointVBO = 0;
GLsizei controlPointCount = 0;
GLint uniformObjectColour = -1;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0;
GLuint uniformSpecularIntensity = 0, uniformShininess = 0;

GLfloat deltaTime = 0.0f, lastTime = 0.0f;

void CreateObjects()
{

	// Quadratic surface: the raised centre control point creates a smooth hill.
	const std::vector<double> knots{0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	const std::vector<std::vector<Point3D>> controlPoints{
		{{-10.0, 0.0, -10.0}, {0.0, 0.0, -10.0}, {10.0, 0.0, -10.0}},
		{{-10.0, 0.0,   0.0}, {0.0, 24.0,  0.0}, {10.0, 0.0,   0.0}},
		{{-10.0, 0.0,  10.0}, {0.0, 0.0,  10.0}, {10.0, 0.0,  10.0}}
	};

	// Small triangle spheres centred on the control points.
	constexpr float controlPointRadius = 0.3f;
	constexpr int stacks = 12, slices = 20;
	constexpr float pi = 3.14159265358979323846f;
	std::vector<GLfloat> pointVertices;
	auto normalAt = [=](int stack, int slice) {
		const float latitude = pi * stack / stacks;
		const float longitude = 2.0f * pi * slice / slices;
		return glm::vec3(std::sin(latitude) * std::cos(longitude),
		                 std::cos(latitude), std::sin(latitude) * std::sin(longitude));
	};
	for (const auto& row : controlPoints) {
		for (const auto& point : row) {
			const glm::vec3 centre(point.x, point.y, point.z);
			auto appendVertex = [&](glm::vec3 normal) {
				const glm::vec3 position = centre + controlPointRadius * normal;
				pointVertices.insert(pointVertices.end(), {
					position.x, position.y, position.z, normal.x, normal.y, normal.z
				});
			};
			for (int i = 0; i < stacks; ++i) {
				for (int j = 0; j < slices; ++j) {
					const auto a = normalAt(i, j), b = normalAt(i + 1, j);
					const auto c = normalAt(i, j + 1), d = normalAt(i + 1, j + 1);
					if (i > 0) { appendVertex(a); appendVertex(c); appendVertex(b); }
					if (i + 1 < stacks) { appendVertex(c); appendVertex(d); appendVertex(b); }
				}
			}
		}
	}
	controlPointCount = static_cast<GLsizei>(pointVertices.size() / 6);
	glGenVertexArrays(1, &controlPointVAO);
	glGenBuffers(1, &controlPointVBO);
	glBindVertexArray(controlPointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, controlPointVBO);
	glBufferData(GL_ARRAY_BUFFER, pointVertices.size() * sizeof(GLfloat), pointVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	const std::vector<std::vector<double>> weights(3, std::vector<double>(3, 1.0));

	NURBSSurface surface(3, 3, 2, 2, knots, knots, controlPoints, weights);
	SurfaceMeshData nurbsMesh = surface.generateMesh(40, 40);

	for (std::size_t i = 0; i + 4 < nurbsMesh.vertices.size(); i += 5) {
		std::cout << "x=" << nurbsMesh.vertices[i]
						<< ", y=" << nurbsMesh.vertices[i + 1]
						<< ", z=" << nurbsMesh.vertices[i + 2]
						<< ", u=" << nurbsMesh.vertices[i + 3]
						<< ", v=" << nurbsMesh.vertices[i + 4] << '\n';
	}

	Mesh *nurbsSurfaceMesh = new Mesh();
	// obj3->createMesh(floorVertices, floorIndices, 20, 6, sizeof(GLfloat) * 8);
	nurbsSurfaceMesh->createMesh(
    nurbsMesh.vertices.data(),
    nurbsMesh.indices.data(),
    static_cast<unsigned int>(nurbsMesh.vertices.size()),
    static_cast<unsigned int>(nurbsMesh.indices.size()),
    sizeof(GLfloat) * 8
	);
	meshList.push_back(nurbsSurfaceMesh);
}

void CreateShaders()
{
	controlPointShader.CreateFromFiles("Shaders/controlPoints.vert", "Shaders/controlPoints.frag");
	sceneShader.CreateFromFiles("Shaders/shader.vert", "Shaders/shader.frag");
	uniformObjectColour = glGetUniformLocation(sceneShader.GetShaderProgram(), "objectColour");
	directionalShadowShader.CreateFromFiles("Shaders/directionalShadowMap.vert", "Shaders/directionalShadowMap.frag");
}

void renderScene()
{
	// meshList[0]->reinitializeModel();
	// meshList[0]->translate(0.0f, 0.0f, -0.5f);
	// meshList[0]->scale(0.4f, 0.4f, 0.4f);
	// glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(meshList[0]->getModelMatrix()));
	// meshList[0]->renderMesh();

	// meshList[1]->reinitializeModel();
	// meshList[1]->translate(0.0f, 1.0f, -2.5f);
	// glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(meshList[1]->getModelMatrix()));
	// meshList[1]->renderMesh();

	meshList[0]->reinitializeModel();
	meshList[0]->translate(0.0f, -4.0f, 0.0f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(meshList[0]->getModelMatrix()));
	meshList[0]->renderMesh();
}

void directionalShadowMapPass(DirectionalLight* dLight)
{
	glViewport(0, 0, dLight->GetShadowMap()->GetShadowWidth(), dLight->GetShadowMap()->GetShadowHeight());

	glm::mat4 lightTransform = dLight->CalculateLightTransform();
	dLight->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	directionalShadowShader.UseShader();
	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

	directionalShadowShader.validate();
	renderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

	// Clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sceneShader.UseShader();
	glUniform3f(uniformObjectColour, 0.55f, 0.65f, 0.72f);

	uniformModel = sceneShader.GetModelLocation();
	uniformProjection = sceneShader.GetProjectionLocation();
	uniformView = sceneShader.GetViewLocation();
	uniformEyePosition = sceneShader.GetEyePositionLocation();
	uniformSpecularIntensity = sceneShader.GetSpecularIntensityLocation();
	uniformShininess = sceneShader.GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	sceneShader.SetDirectionalLight(&light);

	glm::mat4 lightTransform = light.CalculateLightTransform();
	sceneShader.SetDirectionalLightTransform(&lightTransform);

	light.GetShadowMap()->Read(GL_TEXTURE2);

	sceneShader.SetDirectionalShadowMap(2);

	renderScene();

	controlPointShader.UseShader();
	const glm::mat4 model = meshList[0]->getModelMatrix();
	glUniformMatrix4fv(controlPointShader.GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(controlPointShader.GetViewLocation(), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(controlPointShader.GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glBindVertexArray(controlPointVAO);
	glDrawArrays(GL_TRIANGLES, 0, controlPointCount);
	glBindVertexArray(0);
}

int main()
{
	if (mainWindow.Initialise() != 0) return 1;

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(32.0f, 24.0f, 38.0f), glm::vec3(0.0f, 1.0f, 0.0f), -130.1f, -17.9f, 5.0f, 0.5f);

	light = DirectionalLight(2048, 2048,
								1.0f, 1.0f, 1.0f,
								0.1f, 0.3f,
								0.0f, -5.0f, -7.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	lastTime = static_cast<GLfloat>(glfwGetTime());

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glCullFace(GL_FRONT);
		directionalShadowMapPass(&light);
		glCullFace(GL_BACK);
		glm::mat4 view = camera.calculateViewMatrix();
		renderPass(projection, view);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	for (Mesh* mesh : meshList) delete mesh;
	meshList.clear();
	glDeleteBuffers(1, &controlPointVBO);
	glDeleteVertexArrays(1, &controlPointVAO);
	controlPointShader.ClearShader();
	sceneShader.ClearShader();
	directionalShadowShader.ClearShader();
	return 0;
}
