#include "mesh.h"

Mesh::Mesh()
    : normals(nullptr), VAO_(0), VBO_(0), EBO_(0), indices_(nullptr),
      vertices_(nullptr), indexCount_(0), vertexCount_(0), model_(1.0f)
{
}

template <typename T>
void copyArray(const T* source, T* destination, int size) {
    for (int i = 0; i < size; ++i) {
        destination[i] = source[i];
    }
}


template <typename T>
std::vector<T> mergeArrays(const std::vector<T>& oldArray, const std::vector<T>& newArray, int oldCount, int newCount) {
    std::vector<T> mergedArray;
	mergedArray.reserve(oldArray.size() + newArray.size()); // Оптимизируем выделение памяти
    size_t oldSize = oldArray.size();
    size_t newSize = newArray.size();

    size_t oldIndex = 0;
    size_t newIndex = 0;

    // Проходим по массивам, добавляя элементы по заданному правилу
    while (oldIndex < oldSize || newIndex < newSize) {
        // Добавляем элементы из старого массива
        for (int i = 0; i < oldCount && oldIndex < oldSize; ++i) {
            mergedArray.push_back(oldArray[oldIndex++]);
        }

        // Добавляем элементы из нового массива
        for (int i = 0; i < newCount && newIndex < newSize; ++i) {
            mergedArray.push_back(newArray[newIndex++]);
        }
    }
    return mergedArray;
}

void Mesh::createMesh(GLfloat *meshVertices, unsigned int *meshIndices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount_ = numOfIndices;
	vertexCount_ = numOfVertices;
	vertices_ = new GLfloat[vertexCount_];
	copyArray(meshVertices, vertices_, vertexCount_);
	indices_ = new GLuint[indexCount_];
	copyArray(meshIndices, indices_, indexCount_);

	glGenVertexArrays(1, &VAO_);
	glBindVertexArray(VAO_);

	glGenBuffers(1, &EBO_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_[0]) * indexCount_, indices_, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_[0]) * vertexCount_, vertices_, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices_[0]) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices_[0]) * 8, (void*)(sizeof(vertices_[0]) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices_[0]) * 8, (void*)(sizeof(vertices_[0]) * 5));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Mesh::createMesh(GLfloat *meshVertices, unsigned int *meshIndices, unsigned int numOfVertices, unsigned int numOfIndices, GLsizei stride, bool hasNormals)
{
	indexCount_ = numOfIndices;
	vertexCount_ = numOfVertices;
	vertices_ = new GLfloat[vertexCount_];
	copyArray(meshVertices, vertices_, vertexCount_);
	indices_ = new GLuint[indexCount_];
	copyArray(meshIndices, indices_, indexCount_);

	GLfloat* mergedVerticesArray;
	unsigned int vertexArraySize;
	if (!hasNormals)
	{
		normals = new GLfloat[vertexCount_*3/5];
		computeVertexNormals(5);

		std::vector<GLfloat> verticesVector(vertices_, vertices_ + numOfVertices);
		std::vector<GLfloat> normalsVector(normals, normals + vertexCount_*3/5);
		std::vector<GLfloat> mergedVertices = mergeArrays(verticesVector, normalsVector, 5, 3);
		mergedVerticesArray = new GLfloat[mergedVertices.size()];

		std::copy(mergedVertices.begin(), mergedVertices.end(), mergedVerticesArray);

		vertexArraySize = mergedVertices.size();
	}
	else {
		vertexArraySize = numOfVertices;
		mergedVerticesArray = new GLfloat[vertexArraySize];
		copyArray(vertices_, mergedVerticesArray, vertexArraySize);
	}

	// for (size_t i = 0; i < vertexArraySize; i++)
	// {
	// 	std::cout << mergedVerticesArray[i] << " ";
	// }
	// std::cout << vertexArraySize << std::endl;
	

    glCreateVertexArrays(1, &VAO_);
	glBindVertexArray(VAO_);

	glCreateBuffers(1, &VBO_);
	glNamedBufferData(VBO_, sizeof(mergedVerticesArray[0]) * vertexArraySize, mergedVerticesArray, GL_STATIC_DRAW);
	delete[] mergedVerticesArray;

	glCreateBuffers(1, &EBO_);
	glNamedBufferData(EBO_, sizeof(indices_[0]) * indexCount_, indices_, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO_, 0);
	glVertexArrayAttribBinding(VAO_, 0, 0);
	glVertexArrayAttribFormat(VAO_, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO_, 1);
	glVertexArrayAttribBinding(VAO_, 1, 0);
	glVertexArrayAttribFormat(VAO_, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT));

	glEnableVertexArrayAttrib(VAO_, 2);
	glVertexArrayAttribBinding(VAO_, 2, 0);
	glVertexArrayAttribFormat(VAO_, 2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT));

	glVertexArrayVertexBuffer(VAO_, 0, VBO_, 0, stride);
	glVertexArrayElementBuffer(VAO_, EBO_);

	glBindVertexArray(0);

	// another method for gl version less than 4
	// glGenVertexArrays(1, &VAO);
	// glBindVertexArray(VAO);

	// glGenBuffers(1, &EBO);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	// glGenBuffers(1, &VBO);
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(mergedVerticesArray[0]) * mergedVertices.size(), mergedVerticesArray, GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mergedVerticesArray[0]) * 8, 0);
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(mergedVerticesArray[0]) * 8, (void*)(sizeof(mergedVerticesArray[0]) * 3));
	// glEnableVertexAttribArray(1);
	// glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mergedVerticesArray[0]) * 8, (void*)(sizeof(mergedVerticesArray[0]) * 5));
	// glEnableVertexAttribArray(2);

	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// glBindVertexArray(0);
}

glm::vec3 sumArray(const std::vector<glm::vec3>& array) {
    glm::vec3 sum(0.0f);
    for (const auto& vec : array) {
        sum += vec;
    }
    return sum;
}

GLfloat* Mesh::computeVertexNormals(GLint vertexPasser) {
	std::vector<std::vector<glm::vec3>> preVertNormals (vertexCount_ / vertexPasser);
    std::vector<float> calculatedNormals;

    for (int i = 0; i < indexCount_; i += 3) {
	 	glm::vec3 v0(vertices_[indices_[i] * vertexPasser], 
                     vertices_[indices_[i] * vertexPasser + 1], 
                     vertices_[indices_[i] * vertexPasser + 2]);

        glm::vec3 v1(vertices_[indices_[i + 1] * vertexPasser], 
                     vertices_[indices_[i + 1] * vertexPasser + 1], 
                     vertices_[indices_[i + 1] * vertexPasser + 2]);

        glm::vec3 v2(vertices_[indices_[i + 2] * vertexPasser], 
                     vertices_[indices_[i + 2] * vertexPasser + 1], 
                     vertices_[indices_[i + 2] * vertexPasser + 2]);

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		preVertNormals[indices_[i]].push_back(normal);
		preVertNormals[indices_[i + 1]].push_back(normal);
		preVertNormals[indices_[i + 2]].push_back(normal);
    }

	for (const auto& v : preVertNormals) {
        glm::vec3 normal = glm::normalize(sumArray(v)); 

        // Добавляем компоненты вектора в normals
        calculatedNormals.push_back(normal.x);
        calculatedNormals.push_back(normal.y);
        calculatedNormals.push_back(normal.z);
    }

    std::copy(calculatedNormals.begin(), calculatedNormals.end(), normals); // Copy elements from vector to array

    return normals;
}

void Mesh::renderMesh()
{
    glBindVertexArray(VAO_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::renderInstancedMesh(GLuint count)
{
	glBindVertexArray(VAO_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glDrawElementsInstanced(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, 0, count);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::rotateAboutAxe(float angle, glm::vec3 axe) 
{
	glm::vec3 curPosition = getPosition();
	model_ = glm::translate(model_, -curPosition);
	model_ = glm::rotate(model_, glm::radians(angle), axe);
	model_ = glm::translate(model_, curPosition);
}

glm::vec3 Mesh::getRotation () {
	glm::quat rotation = glm::quat_cast(glm::mat3(model_));
	glm::vec3 eulerAngles = glm::eulerAngles(rotation);
	eulerAngles = glm::degrees(eulerAngles);
	return eulerAngles;
}

void Mesh::clearMesh()
{
	if (EBO_ != 0)
	{
		glDeleteBuffers(1, &EBO_);
		EBO_ = 0;
	}

	if (VBO_ != 0)
	{
		glDeleteBuffers(1, &VBO_);
		VBO_ = 0;
	}

	if (VAO_ != 0)
	{
		glDeleteVertexArrays(1, &VAO_);
		VAO_ = 0;
	}

	if (normals != nullptr) {
		delete[] normals;
		normals = nullptr;
	}
	if (vertices_ != nullptr) {
		delete[] vertices_;
		vertices_ = nullptr;
	}
	if (indices_ != nullptr) {
		delete[] indices_;
		indices_ = nullptr;
	}

	indexCount_ = 0;
	vertexCount_ = 0;
}

Mesh::~Mesh()
{
	clearMesh();
}

