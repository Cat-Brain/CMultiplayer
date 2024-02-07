#include "Resources.h"

typedef struct
{
	uint vbo, ebo, vao,
		indexCount;
} Mesh;

Mesh CreateMesh(float* vertData, uint vertCount, uint* indexData, uint indexCount)
{
	Mesh result = { 0, 0, 0, indexCount };

	glGenVertexArrays(1, &result.vao);
	glGenBuffers(1, &result.vbo);
	glGenBuffers(1, &result.ebo);

	glBindVertexArray(result.vao);

	glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(float), vertData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indexData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	return result;
}

void DrawMesh(Mesh mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
}

void DestroyMesh(Mesh mesh)
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ebo);
	glDeleteVertexArrays(1, &mesh.vao);
}

int meshFiles[] =
{
	QUAD_FILE
};

Mesh meshes[1];

#define QuadMesh (meshes[0])

void LoadMeshes()
{
	for (int i = 0; i < sizeof(meshes) / sizeof(Mesh); i++)
	{
		// Open, read, and close resource:
		Resource meshResource = OpenResource(meshFiles[i], TEXT_FILE);
		
		char* copiedData = malloc(strlen(meshResource.ptr));
		if (copiedData == NULL)
		{
			printf("Data allocation failed!");
			continue;
		}
		strcpy(copiedData, meshResource.ptr);
		CloseResource(meshResource);

		// Try to seperate the vertex data from the index data:
		char* vertStart = strchr(copiedData, 'V');
		char* indStart = strchr(copiedData, 'I');
		if (vertStart == NULL)
		{
			printf("File did not contain the character \"V\"!");
			continue;
		}
		if (indStart == NULL)
		{
			printf("File did not contain the character \"I\"!");
			continue;
		}
		
		// Remove some formatting (UNSAFE):
		vertStart[0] = '\0';
		vertStart += 3;
		indStart[0] = '\0';
		indStart += 3;

		vertStart[strlen(vertStart) - 1] = '\0';
		indStart[strlen(indStart) - 1] = '\0';


		// Find the amount of vertices and indices. Also replaces " " and "\n" with "\0":
		uint vertCount = 1;
		for (char* vertStart2 = vertStart; *vertStart2; vertStart2++)
		{
			if (*vertStart2 == '\n')
			{
				vertCount++;
				*vertStart2 = '\0';
			}
			else if (*vertStart2 == ' ')
				*vertStart2 = '\0';
		}

		uint triCount = 1;
		for (char* indStart2 = indStart; *indStart2; indStart2++)
		{
			if (*indStart2 == '\n')
			{
				triCount++;
				*indStart2 = '\0';
			}
			else if (*indStart2 == ' ')
				*indStart2 = '\0';
		}

		// They should be in the total amount of pieces of data not the actual amount:
		vertCount *= 2;
		triCount *= 3;

		// Allocate the new data:
		float* vertData = calloc(vertCount, sizeof(float));
		uint* indData = calloc(triCount, sizeof(uint));
		if (vertData == NULL || indData == NULL)
		{
			printf("Data allocation failed!");
			continue;
		}

		// Read the data from the formatted string:
		char* vertStart2 = vertStart;
		for (uint i = 0; i < vertCount; i++)
		{
			vertData[i] = (float)atof(vertStart2);
			vertStart2 = strchr(vertStart2, '\0') + 1;
		}
		char* indStart2 = indStart;
		for (uint i = 0; i < triCount; i++)
		{
			indData[i] = (uint)atoi(indStart2);
			indStart2 = strchr(indStart2, '\0') + 1;
		}

		// MEMORY LEAKS COPIED DATA!!!
		//free(copiedData);

		// Create a mesh from the finished data:
		meshes[i] = CreateMesh(vertData, vertCount, indData, triCount);

		free(vertData);
		free(indData);
	}
}