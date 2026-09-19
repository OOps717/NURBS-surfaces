#include <limits>
#include <memory>
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
#include "Computational/raycasting.h"
#include "Primitives/sphere.h"

Window mainWindow(1366, 768);
Camera camera;

Shader directionalShadowShader;

DirectionalLight light;

Mesh nurbsSurfaceMesh;
std::unique_ptr<NURBSSurface> surface;
int controlPointColumns = 0;
bool dragging = false;
glm::vec3 dragPlanePoint(0.0f), dragPlaneNormal(0.0f), dragOffset(0.0f);
Shader sceneShader;
Shader controlPointShader;
std::vector<std::unique_ptr<Sphere>> controlPointSpheres;
GLint uniformObjectColour = -1;
GLint uniformControlColour = -1;
int selectedControlPoint = -1;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0;
GLuint uniformSpecularIntensity = 0, uniformShininess = 0;

GLfloat deltaTime = 0.0f, lastTime = 0.0f;

// Rebuild the small mesh only when a control point changes.
void updateSurfaceMesh()
{
	auto data = surface->generateMesh(40, 40);
	nurbsSurfaceMesh.clearMesh();
	nurbsSurfaceMesh.createMesh(data.vertices.data(), data.indices.data(),
			static_cast<unsigned int>(data.vertices.size()),
			static_cast<unsigned int>(data.indices.size()), 8 * sizeof(GLfloat));
}

void CreateObjects()
{

	// Quadratic surface: the raised centre control point creates a smooth hill
	const std::vector<double> knotsU{0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
	const std::vector<double> knotsV{0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	const std::vector<std::vector<Point3D>> controlPoints{
		{{-10.0, 0.0, -10.0}, {0.0, 0.0, -10.0}, {10.0, 0.0, -10.0}},
		{{-10.0, 0.0,   0.0}, {0.0, 24.0,  0.0}, {10.0, 0.0,   0.0}},
		{{-10.0, 0.0,  10.0}, {0.0, -15.0,  10.0}, {10.0, 0.0,  10.0}},
		{{-10.0, 0.0,  20.0}, {0.0, 0.0,  20.0}, {10.0, 0.0,  20.0}}
	};

	constexpr float controlPointRadius = 0.3f;
	for (const auto& row : controlPoints) {
		for (const auto& point : row) {
			auto sphere = std::make_unique<Sphere>(20, 12, controlPointRadius);
			sphere->CreateSphere();
			sphere->translate(static_cast<float>(point.x), static_cast<float>(point.y), static_cast<float>(point.z));
			controlPointSpheres.push_back(std::move(sphere));
		}
	}

	const std::vector<std::vector<double>> weights(4, std::vector<double>(3, 1.0));

	controlPointColumns = static_cast<int>(controlPoints.front().size());
	surface = std::make_unique<NURBSSurface>(4, 3, 3, 2, knotsU, knotsV, controlPoints, weights);
	updateSurfaceMesh();
	nurbsSurfaceMesh.translate(0.0f, -4.0f, 0.0f);
}

void CreateShaders()
{
	controlPointShader.CreateFromFiles("Shaders/controlPoints.vert", "Shaders/controlPoints.frag");
	uniformControlColour = glGetUniformLocation(controlPointShader.GetShaderProgram(), "markerColour");
	sceneShader.CreateFromFiles("Shaders/shader.vert", "Shaders/shader.frag");
	uniformObjectColour = glGetUniformLocation(sceneShader.GetShaderProgram(), "objectColour");
	directionalShadowShader.CreateFromFiles("Shaders/directionalShadowMap.vert", "Shaders/directionalShadowMap.frag");
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
	nurbsSurfaceMesh.reinitializeModel();
	nurbsSurfaceMesh.translate(0.0f, -4.0f, 0.0f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(nurbsSurfaceMesh.getModelMatrix()));



	nurbsSurfaceMesh.renderMesh();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

	// Clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	controlPointShader.UseShader();
	glUniformMatrix4fv(controlPointShader.GetViewLocation(), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(controlPointShader.GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	for (std::size_t i = 0; i < controlPointSpheres.size(); ++i) {
		const auto& sphere = controlPointSpheres[i];
		const glm::vec3 colour = static_cast<int>(i) == selectedControlPoint
					? glm::vec3(0.2f, 1.0f, 0.35f) : glm::vec3(1.0f, 0.65f, 0.15f);
		glUniform3fv(uniformControlColour, 1, glm::value_ptr(colour));
		const glm::mat4 model = nurbsSurfaceMesh.getModelMatrix() * sphere->getModelMatrix();
		glUniformMatrix4fv(controlPointShader.GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
		sphere->renderMesh();
	}

	sceneShader.UseShader();
	glUniform3f(uniformObjectColour, 0.55f, 0.65f, 0.72f);

	uniformModel = sceneShader.GetModelLocation();
	uniformProjection = sceneShader.GetProjectionLocation();
	uniformView = sceneShader.GetViewLocation();
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	uniformEyePosition = sceneShader.GetEyePositionLocation();
	uniformSpecularIntensity = sceneShader.GetSpecularIntensityLocation();
	uniformShininess = sceneShader.GetShininessLocation();

	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	sceneShader.SetDirectionalLight(&light);

	glm::mat4 lightTransform = light.CalculateLightTransform();
	sceneShader.SetDirectionalLightTransform(&lightTransform);

	light.GetShadowMap()->Read(GL_TEXTURE2);

	sceneShader.SetDirectionalShadowMap(2);

	nurbsSurfaceMesh.reinitializeModel();
	nurbsSurfaceMesh.translate(0.0f, -4.0f, 0.0f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(nurbsSurfaceMesh.getModelMatrix()));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	nurbsSurfaceMesh.renderMesh();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

int main()
{
	if (mainWindow.Initialise() != 0) return 1;

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(32.0f, 24.0f, 38.0f), glm::vec3(0.0f, 1.0f, 0.0f), -130.1f, -17.9f, 15.5f, 0.5f);

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
		mainWindow.refreshBufferSize();
		int windowWidth, windowHeight;
		mainWindow.getWindowSize(windowWidth, windowHeight);
		if (windowWidth <= 0 || windowHeight <= 0 || mainWindow.getBufferWidth() <= 0 || mainWindow.getBufferHeight() <= 0)
				continue;
		projection = glm::perspective(glm::radians(45.0f),
				static_cast<float>(mainWindow.getBufferWidth()) / mainWindow.getBufferHeight(), 0.1f, 100.0f);
		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glm::mat4 view = camera.calculateViewMatrix();
		double mouseX, mouseY;
		if (!mainWindow.isLeftMousePressed()) dragging = false;

		if (mainWindow.consumeLeftClick(mouseX, mouseY)) {
			dragging = false;
			Raycasting ray(mouseX, mouseY, windowWidth, windowHeight, projection, view);

			selectedControlPoint = -1;
			float nearest = std::numeric_limits<float>::max();
			for (std::size_t i = 0; i < controlPointSpheres.size(); ++i) {
				const auto& sphere = controlPointSpheres[i];
				float distance;
				if (ray.intersectSphere(nurbsSurfaceMesh.getModelMatrix() * sphere->getModelMatrix(),
															sphere->getRadius(), distance) && distance < nearest) {
					nearest = distance;
					selectedControlPoint = static_cast<int>(i);
				}
			}

			if (selectedControlPoint >= 0 && mainWindow.isLeftMousePressed()) {
				// Control Point in world space, with drag plane normal along the camera direction
				dragPlanePoint = glm::vec3(nurbsSurfaceMesh.getModelMatrix()
						* controlPointSpheres[selectedControlPoint]->getModelMatrix()[3]);
				dragPlaneNormal = camera.getCameraDirection();

				glm::vec3 hit;
				ray.intersectPlane(dragPlanePoint, dragPlaneNormal, hit);
				dragOffset = dragPlanePoint - hit;
				dragging = true;

			}
		}

		if (dragging) {
				mainWindow.getCursorPosition(mouseX, mouseY);
				Raycasting ray(mouseX, mouseY, windowWidth, windowHeight, projection, view);
				glm::vec3 hit;
				ray.intersectPlane(dragPlanePoint, dragPlaneNormal, hit);
				const glm::vec3 position = glm::vec3(glm::inverse(nurbsSurfaceMesh.getModelMatrix())
						* glm::vec4(hit + dragOffset, 1.0f));
				auto& sphere = controlPointSpheres[selectedControlPoint];
				const glm::vec3 delta = position - sphere->getPosition();
				if (glm::dot(delta, delta) > 1e-8f) {
						surface->setControlPoint(selectedControlPoint / controlPointColumns,
								selectedControlPoint % controlPointColumns, {position.x, position.y, position.z});
						sphere->reinitializeModel();
						sphere->translate(position.x, position.y, position.z);
						updateSurfaceMesh();
				}

		}
		glCullFace(GL_FRONT);
		directionalShadowMapPass(&light);
		glCullFace(GL_BACK);
		renderPass(projection, view);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	nurbsSurfaceMesh.clearMesh();
	controlPointSpheres.clear();
	controlPointShader.ClearShader();
	sceneShader.ClearShader();
	directionalShadowShader.ClearShader();
	return 0;
}
