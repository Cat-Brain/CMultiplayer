#include "Settings.h"

struct
{
	GLFWwindow* window;
	uint width, height;
	float screenRatio;
} renderer;


bool InitRenderer()
{
#pragma region Very eary stuff
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	renderer.width = WINDOW_DEFAULT_WIDTH;
	renderer.height = WINDOW_DEFAULT_HEIGHT;
	renderer.screenRatio = (float)renderer.width / (float)renderer.height;
	
	renderer.window = glfwCreateWindow(renderer.width, renderer.height, WINDOW_NAME, NULL, NULL);
	if (renderer.window == NULL)
	{
		printf("Failed to create GLFW window\n");
		return false;
	}
	glfwMakeContextCurrent(renderer.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return false;
	}

	glViewport(0, 0, renderer.width, renderer.height);
#pragma endregion

	/*glfwSetFramebufferSizeCallback(renderer.window, framebuffer_size_callback);
	glfwSetScrollCallback(renderer.window, scroll_callback);
	glfwSetWindowMaximizeCallback(renderer.window, window_maximize_callback);
	glfwSetKeyCallback(renderer.window, key_callback);*/

	if (settings.maximized)
		glfwMaximizeWindow(renderer.window);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	glfwSetInputMode(renderer.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	glfwSetCursor(renderer.window, cursor);

	
	LoadMeshes();

	CompileShaders();
	/*
	uint textureType = std::get<0>(textures);
	for (auto& [texture, location, spriteCount] : std::get<1>(textures))
		texture = Texture(location, textureType, spriteCount);

	glUseProgram(circleShader);
	glUniform1i(glGetUniformLocation(circleShader, "stippleTexture"), 8);
	glUniform1i(glGetUniformLocation(circleShader, "stippleTextureWidth"), stippleTexture.width);

	quad = Mesh({ 0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f }, { 0, 1, 2, 0, 2, 3 });
	screenSpaceQuad = Mesh({ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f }, { 0, 1, 2, 0, 2, 3 });
	line = Mesh({ 1.0f, 0.0f, 0.0f, 1.0f }, { 0, 1 }, GL_LINES);
	dot = Mesh({ 0.0f, 0.0f }, { 0 }, GL_POINTS);
	rightTriangle = Mesh({ -1.f, 0.0f,  0.0f, 1.f,  1.f, 0.0f }, { 0, 1, 2 });

	cube = Mesh({ -1.f, -1.f, -1.f,  -1.f, -1.f, 1.f,  -1.f, 1.f, -1.f,  -1.f, 1.f, 1.f,
		1.f, -1.f, -1.f,  1.f, -1.f, 1.f,  1.f, 1.f, -1.f,  1.f, 1.f, 1.f }, { 1, 3, 7, 1, 7, 5,
		0, 2, 3, 0, 3, 1,  0, 1, 5, 0, 5, 4,  2, 6, 7, 2, 7, 3,  6, 4, 5, 6, 5, 7,  0, 4, 6, 0, 6, 2 },
		GL_TRIANGLES, GL_STATIC_DRAW, 3);

	if (settings.lastVersion != version)
	{
		settings.hasLoadedModels = false;
		settings.Write();
	}
	if (!settings.hasLoadedModels)
	{
		for (auto& [model, location, name] : mesh2s)
		{
			Resource resource = Resource(location, OBJ_FILE);
			std::ofstream stream;
			stream.open(name + ".obj");
			stream.write(static_cast<const char*>(resource.ptr), resource.size);
		}
		settings.hasLoadedModels = true;
		settings.Write();
	}
	for (auto& [model, location, name] : mesh2s)
	{
		fastObjMesh* mesh = fast_obj_read((name + ".obj").c_str());

		vector<float> positions;
		for (uint i = 3; i < mesh->position_count * 3; i++)
			positions.push_back(mesh->positions[i]);
		vector<float> normals;
		for (uint i = 3; i < mesh->normal_count * 3; i++)
			normals.push_back(mesh->normals[i]);

		vector<uint> indices;
		for (uint i = 0; i * 3 < mesh->index_count; i++)
		{
			indices.push_back(mesh->indices[i * 3 + 2].p - 1);
			indices.push_back(mesh->indices[i * 3 + 1].p - 1);
			indices.push_back(mesh->indices[i * 3].p - 1);
			indices.push_back(mesh->indices[i * 3 + 2].n - 1);
			indices.push_back(mesh->indices[i * 3 + 1].n - 1);
			indices.push_back(mesh->indices[i * 3].n - 1);
		}
		fast_obj_destroy(mesh);

		vector<float> data;
		for (int i = 0; i * 6 < indices.size(); i++)
		{
			data.push_back(positions[indices[i * 6] * 3]);
			data.push_back(positions[indices[i * 6] * 3 + 1]);
			data.push_back(positions[indices[i * 6] * 3 + 2]);
			data.push_back(normals[indices[i * 6 + 3] * 3]);
			data.push_back(normals[indices[i * 6 + 3] * 3 + 1]);
			data.push_back(normals[indices[i * 6 + 3] * 3 + 2]);

			data.push_back(positions[indices[i * 6 + 1] * 3]);
			data.push_back(positions[indices[i * 6 + 1] * 3 + 1]);
			data.push_back(positions[indices[i * 6 + 1] * 3 + 2]);
			data.push_back(normals[indices[i * 6 + 4] * 3]);
			data.push_back(normals[indices[i * 6 + 4] * 3 + 1]);
			data.push_back(normals[indices[i * 6 + 4] * 3 + 2]);

			data.push_back(positions[indices[i * 6 + 2] * 3]);
			data.push_back(positions[indices[i * 6 + 2] * 3 + 1]);
			data.push_back(positions[indices[i * 6 + 2] * 3 + 2]);
			data.push_back(normals[indices[i * 6 + 5] * 3]);
			data.push_back(normals[indices[i * 6 + 5] * 3 + 1]);
			data.push_back(normals[indices[i * 6 + 5] * 3 + 2]);
		}
		*model = Mesh2(data, GL_TRIANGLES, GL_STATIC_DRAW, 3);
	}

	glGenBuffers(1, &instanceVBO);
	glBindVertexArray(cube.vao);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

	mainScreen = make_unique<DeferredFramebuffer>(trueScreenHeight, GL_RGB, true);
	shadowMap = make_unique<Framebuffer>(trueScreenHeight, GL_RGB16F, true);
	framebuffers = { mainScreen.get(), shadowMap.get() };

	Resource defaultFont = Resource(PIXELOID_SANS, FONT_FILE);
	font = Font(static_cast<FT_Byte*>(defaultFont.ptr), static_cast<FT_Long>(defaultFont.size), 128);

	currentFramebuffer = MAINSCREEN;
	UseFramebuffer();

	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	if (settings.lastVersion != version)
	{
		settings.lastVersion = version;
		settings.Write();
	}*/

	return true;
}

void UpdateRenderer()
{
	glfwSwapInterval(settings.vSync ? 1 : 0);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(DefaultShader);
	glUniform2f(glGetUniformLocation(DefaultShader, "position"), 0.f, 0.f);
	glUniform2f(glGetUniformLocation(DefaultShader, "scale"), 1.f, 1.f);
	glUniform4f(glGetUniformLocation(DefaultShader, "color"), 0.5f, 0.5f, 1.f, 1.f);

	DrawMesh(QuadMesh);

	glfwSwapBuffers(renderer.window);
	glfwPollEvents();
}

void DestroyRenderer()
{
	glfwTerminate();
}