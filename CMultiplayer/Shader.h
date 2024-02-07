#include "Mesh.h"

typedef uint ShaderProgram;

ShaderProgram CreateShader(char* vertexShaderSource, char* fragmentShaderSource, const char* shaderName) // Shader name is entirely in here for debugging.
{
	printf("Generating %s\n", shaderName);
	uint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}
	uint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	ShaderProgram shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n%s\n", infoLog);
	}
	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

const int vertShaderSources[] = {
	DEFAULT_VERT
};

const int fragShaderSources[] = {
	DEFAULT_FRAG
};

const char* shaderNames[] = {
	"Default Shader"
};

ShaderProgram shaderPrograms[1] = { 0 };

#define DefaultShader (shaderPrograms[0])

void CompileShaders()
{
	for (int i = 0; i < sizeof(vertShaderSources) / sizeof(int); i++)
	{
		Resource vertResource = OpenResource(vertShaderSources[i], TEXT_FILE);
		Resource fragResource = OpenResource(fragShaderSources[i], TEXT_FILE);

		shaderPrograms[i] = CreateShader(vertResource.ptr, fragResource.ptr, shaderNames[i]);

		CloseResource(vertResource);
		CloseResource(fragResource);
	}
}