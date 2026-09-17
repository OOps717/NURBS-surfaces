#include "shader.h"


Shader::Shader()
    : vertexShader_(0), geometryShader_(0), fragmentShader_(0), shaderProgram_(0),
      uniformModel_(0), uniformProjection_(0), uniformView_(0), uniformEyePosition_(0),
      uniformSpecularIntensity_(0), uniformShininess_(0),
      uniformDirectionalLightTransform_(0), uniformDirectionalShadowMap_(0),
      uniformDirectionalLight_{}
{
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CreateAndCompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexSource = vertexString.c_str();
	const char* fragmentSource = fragmentString.c_str();

	CreateAndCompileShader(vertexSource, fragmentSource);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string geometryString = ReadFile(geometryLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexSource = vertexString.c_str();
	const char* geometrySource = geometryString.c_str();
	const char* fragmentSource = fragmentString.c_str();

	CreateAndCompileShader(vertexSource, geometrySource, fragmentSource);
}

void Shader::validate ()
{
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glValidateProgram(shaderProgram_);
	glGetProgramiv(shaderProgram_, GL_VALIDATE_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shaderProgram_, sizeof(eLog), NULL, eLog);
		std::cerr << "Error validating program: " << eLog << std::endl;

		glDeleteShader(vertexShader_);
		glDeleteShader(fragmentShader_);
		if (geometryShader_ != 0) {
			glDeleteShader(geometryShader_);
		}
		glDeleteProgram(shaderProgram_);
		shaderProgram_ = 0;
		return;
	}

}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Failed to read " << fileLocation << std::endl;
		throw std::runtime_error("Could not open the file");
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint Shader::AddShader(GLuint theProgram, const char* shaderSource, GLenum shaderType) {
    // Create Vertex Shader Object and get its reference
	GLuint shader = glCreateShader(shaderType);

    const GLchar* theCode[1];
	theCode[0] = shaderSource;

    GLint codeLength[1];
	codeLength[0] = strlen(shaderSource);

	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(shader, 1, &shaderSource, codeLength);
	// Compile the Vertex Shader into machine code
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Ошибка компиляции вершинного шейдера: " << infoLog << std::endl;
    }
	glAttachShader(theProgram, shader);
	return shader;
}

void Shader::CreateAndCompileShader(const char* vertexSource, const char* fragmentSource)
{
	shaderProgram_ = glCreateProgram();

	if (!shaderProgram_)
	{
		printf("Error creating shader program!\n");
		return;
	}

	vertexShader_ = AddShader(shaderProgram_, vertexSource, GL_VERTEX_SHADER);
	fragmentShader_ = AddShader(shaderProgram_, fragmentSource, GL_FRAGMENT_SHADER);

	CompileProgram();

  glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);
}

void Shader::CreateAndCompileShader(const char* vertexSource, const char* geometrySource, const char* fragmentSource)
{
	shaderProgram_ = glCreateProgram();

	if (!shaderProgram_)
	{
		printf("Error creating shader program!\n");
		return;
	}

	vertexShader_ = AddShader(shaderProgram_, vertexSource, GL_VERTEX_SHADER);
	geometryShader_ = AddShader(shaderProgram_, geometrySource, GL_GEOMETRY_SHADER);
	fragmentShader_ = AddShader(shaderProgram_, fragmentSource, GL_FRAGMENT_SHADER);

	CompileProgram();

	glDeleteShader(vertexShader_);
	glDeleteShader(geometryShader_);
	glDeleteShader(fragmentShader_);
}

void Shader::SetDirectionalLight(DirectionalLight * dLight)
{
	dLight->UseLight(uniformDirectionalLight_.uniformAmbientIntensity_, uniformDirectionalLight_.uniformColour_,
		uniformDirectionalLight_.uniformDiffuseIntensity_, uniformDirectionalLight_.uniformDirection_);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{
	glUniform1i(uniformDirectionalShadowMap_, textureUnit);
}

void Shader::SetDirectionalLightTransform(glm::mat4 * lTransform)
{
	glUniformMatrix4fv(uniformDirectionalLightTransform_, 1, GL_FALSE, glm::value_ptr(*lTransform));
}

void Shader::CompileProgram()
{
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderProgram_);
	glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &result);
	if (!result) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
		std::cerr << "Ошибка линковки шейдера: " << infoLog << std::endl;

		glDeleteShader(vertexShader_);
		glDeleteShader(fragmentShader_);
		if (geometryShader_ != 0) {  // Удаляем, если он использовался
			glDeleteShader(geometryShader_);
		}
		glDeleteProgram(shaderProgram_);
		shaderProgram_ = 0;
		return;
	}


	uniformProjection_ = glGetUniformLocation(shaderProgram_, "projection");
	uniformModel_ = glGetUniformLocation(shaderProgram_, "model");
	uniformView_ = glGetUniformLocation(shaderProgram_, "view");
	uniformEyePosition_ = glGetUniformLocation(shaderProgram_, "eyePosition");
	uniformDirectionalLight_.uniformColour_ = glGetUniformLocation(shaderProgram_, "directionalLight.base.colour");
	uniformDirectionalLight_.uniformAmbientIntensity_ = glGetUniformLocation(shaderProgram_, "directionalLight.base.ambientIntensity");
	uniformDirectionalLight_.uniformDiffuseIntensity_ = glGetUniformLocation(shaderProgram_, "directionalLight.base.diffuseIntensity");
	uniformDirectionalLight_.uniformDirection_ = glGetUniformLocation(shaderProgram_, "directionalLight.direction");
	uniformShininess_ = glGetUniformLocation(shaderProgram_, "material.shininess");
	uniformSpecularIntensity_ = glGetUniformLocation(shaderProgram_, "material.specularIntensity");

	uniformDirectionalLightTransform_ = glGetUniformLocation(shaderProgram_, "directionalLightTransform");
	uniformDirectionalShadowMap_ = glGetUniformLocation(shaderProgram_, "directionalShadowMap");

}

void Shader::UseShader()
{
	glUseProgram(shaderProgram_);
}

void Shader::ClearShader()
{
	if (shaderProgram_ != 0)
	{
		glDeleteProgram(shaderProgram_);
		shaderProgram_ = 0;
	}

	uniformModel_ = 0;
	uniformProjection_ = 0;
	uniformView_ = 0;
}

Shader::~Shader()
{
	ClearShader();
}
