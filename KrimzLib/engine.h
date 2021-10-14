#pragma once
#include <functional>
#include <string>
#include <sstream>
#include <windows.h>
#include "KrimzLib/types.h"
#include "KrimzLib/constant.h"


namespace kl
{
	class engine
	{
	public:
		// Engine properties
		double fpsLimit = -1;
		double deltaTime = 0;
		double gravity = 4;
		colorf background = {};
		camera engineCamera = {};

		// Outside functions that user defines
		std::function<void(void)> EngineStart = []() {};
		std::function<void(char)> EngineInput = [](char input) {};
		std::function<void(void)> EngineUpdate = []() {};

		// Creates the engine
		void Start(int width, int height, const wchar_t* name, double fov = 60)
		{
			engineWindow.WindowStart = [&]()
			{
				/* Enable 3D and depth buffer */
				opengl::Enabled3D(fov, engineWindow.GetWidth(), engineWindow.GetHeight());

				/* Enable textures */
				opengl::EnableTextures();

				/* Game start function call */
				EngineStart();

				/* First stopwatch reset */
				engineTime.StopwatchReset();
			};

			engineWindow.WindowUpdate = [&]()
			{
				/* Game input */
				EngineInput((char)engineWindow.KEY);

				/* Game logic */
				EngineUpdate();

				/* Apply physics */
				ObjectPhysics();

				/* Buffer clearing */
				opengl::ClearBuffers(background);

				/* Update camera rotation and position */
				opengl::UpdateCamera(engineCamera.position, engineCamera.rotation);

				/* Render all game triangles */
				for (int i = 0; i < engineObjects.size(); i++)
				{
					if (engineObjects[i].visible)
					{
						opengl::RenderTriangles(engineObjects[i].triangles, engineObjects[i].position, engineObjects[i].rotation, engineObjects[i].size, engineObjects[i].texture);
					}
				}

				/* Swap front and back frame buffers */
				opengl::FlipBuffers(engineWindow.GetHDC());

				/* Delta time calculation */
				double wantedFrameTime = 1 / fpsLimit;
				do {
					deltaTime = engineTime.StopwatchElapsed();
				} while (deltaTime < wantedFrameTime);
				engineTime.StopwatchReset();

				/* Display the FPS */
				engineWindow.SetTitle(int(1 / deltaTime));
			};

			engineWindow.WindowEnd = [&]()
			{

			};

			engineWindow.Start(width, height, name, false, true, true);
		}
		void Stop()
		{
			engineWindow.Stop();
		}
		~engine()
		{
			this->Stop();
		}

		// Adds a new game object if the name doesn't already exist
		gameobject* NewGameObject(std::string objectName, texture textureID = 0)
		{
			for (int i = 0; i < engineObjects.size(); i++)
			{
				if (engineObjects[i].name == objectName)
				{
					printf("Game object \"%s\" already exists!\n", objectName.c_str());
					console::WaitFor(' ', true);
					exit(69);
				}
			}
			engineObjects.push_back({ objectName, textureID });
			return &engineObjects.back();
		}
		gameobject* NewGameObject(std::string objectName, std::wstring filePath, texture textureID)
		{
			for (int i = 0; i < engineObjects.size(); i++)
			{
				if (engineObjects[i].name == objectName)
				{
					printf("Game object \"%s\" already exists!\n", objectName.c_str());
					console::WaitFor(' ', true);
					exit(69);
				}
			}

			// Load file
			std::stringstream ss = std::stringstream(file::ReadText(filePath));

			// Parse object data
			std::string fileLine;
			std::vector<vec3> xyzCoords;
			std::vector<vec2> uvCoords;
			std::vector<std::vector<point>> fileTriangles;
			while (std::getline(ss, fileLine))
			{
				std::istringstream iss(fileLine);
				std::string linePart;
				iss >> linePart;
				if (linePart == "v")
				{
					vec3 tempVertex = {};
					int spaceCoordCounter = 0;
					while (iss) {
						iss >> linePart;
						if (spaceCoordCounter == 0)
						{
							tempVertex.x = stod(linePart);
						}
						else if (spaceCoordCounter == 1)
						{
							tempVertex.y = stod(linePart);
						}
						else if (spaceCoordCounter == 2)
						{
							tempVertex.z = stod(linePart);
						}
						spaceCoordCounter++;
					}
					xyzCoords.push_back(tempVertex);
				}
				else if (linePart == "vt")
				{
					vec2 tempVertex = {};
					int textureCoordCounter = 0;
					while (iss)
					{
						iss >> linePart;
						if (textureCoordCounter == 0)
						{
							tempVertex.x = stod(linePart);
						}
						else if (textureCoordCounter == 1)
						{
							tempVertex.y = stod(linePart);
						}
						textureCoordCounter++;
					}
					uvCoords.push_back(tempVertex);
				}
				else if (linePart == "f")
				{
					std::vector<point> tempTriangle(3);
					int vertexCounter = 0;
					while (iss && vertexCounter < 3)
					{
						iss >> linePart;
						for (int i = 0; i < 2; i++)
						{
							size_t slashPosition = linePart.find('/');
							std::string dataAsString = linePart.substr(0, slashPosition);
							if (i == 0)
							{
								tempTriangle[vertexCounter].x = stoi(dataAsString) - 1;
							}
							else if (i == 1)
							{
								tempTriangle[vertexCounter].y = stoi(dataAsString) - 1;
							}
							linePart = linePart.substr(slashPosition + 1);
						}
						vertexCounter++;
					}
					fileTriangles.push_back(tempTriangle);
				}
			}

			// Create the game object with data
			gameobject tempObject = { objectName };
			for (int i = 0; i < fileTriangles.size(); i++)
			{
				tempObject.triangles.push_back({{
					{
					xyzCoords[fileTriangles[i][0].x].x,
					xyzCoords[fileTriangles[i][0].x].y,
					xyzCoords[fileTriangles[i][0].x].z,
					uvCoords[fileTriangles[i][0].y].x,
					uvCoords[fileTriangles[i][0].y].y
					},
					{
					xyzCoords[fileTriangles[i][1].x].x,
					xyzCoords[fileTriangles[i][1].x].y,
					xyzCoords[fileTriangles[i][1].x].z,
					uvCoords[fileTriangles[i][1].y].x,
					uvCoords[fileTriangles[i][1].y].y
					},
					{
					xyzCoords[fileTriangles[i][2].x].x,
					xyzCoords[fileTriangles[i][2].x].y,
					xyzCoords[fileTriangles[i][2].x].z,
					uvCoords[fileTriangles[i][2].y].x,
					uvCoords[fileTriangles[i][2].y].y
					}
				}, true });
			}
			tempObject.texture = textureID;
			
			// Load the game object to the engine
			engineObjects.push_back(tempObject);
			return &engineObjects.back();
		}

		// Removes a game object with the given name
		void DeleteGameObject(std::string objectName)
		{
			for (int i = 0; i < engineObjects.size(); i++)
			{
				if (engineObjects[i].name == objectName)
				{
					engineObjects.erase(engineObjects.begin() + i);
					return;
				}
			}
			printf("Game object \"%s\" doesn't exist!\n", objectName.c_str());
			console::WaitFor(' ', true);
			exit(69);
		}

		// Returns a reference to a wanted game object
		gameobject* GetGameObject(std::string objectName)
		{
			for (int i = 0; i < engineObjects.size(); i++)
			{
				if (engineObjects[i].name == objectName)
				{
					return &engineObjects[i];
				}
			}
			printf("Game object \"%s\" doesn't exist!\n", objectName.c_str());
			console::WaitFor(' ', true);
			exit(69);
		}

		// Adds a new texture to the engine memory
		texture NewTexture(bitmap& textureData)
		{
			texture createdID = 0;
			glGenTextures(1, &createdID);
			glBindTexture(GL_TEXTURE_2D, createdID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData.GetWidth(), textureData.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.GetPixelData());
			glGenerateMipmap(GL_TEXTURE_2D);
			return createdID;
		}
		texture NewTexture(bitmap&& textureData)
		{
			texture createdID = 0;
			glGenTextures(1, &createdID);
			glBindTexture(GL_TEXTURE_2D, createdID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData.GetWidth(), textureData.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.GetPixelData());
			glGenerateMipmap(GL_TEXTURE_2D);
			return createdID;
		}

	private:
		// Misc
		window engineWindow = window();

		// Time
		time engineTime = time();

		// Objects
		std::vector<gameobject> engineObjects = {};

		// Computing object physics 
		void ObjectPhysics()
		{
			for (int i = 0; i < engineObjects.size(); i++)
			{
				if (engineObjects[i].physics)
				{
					// Applying gravity
					engineObjects[i].velocity.y -= gravity * engineObjects[i].gravityMulti * deltaTime;

					// Applying velocity
					engineObjects[i].position.x += engineObjects[i].velocity.x * deltaTime;
					engineObjects[i].position.y += engineObjects[i].velocity.y * deltaTime;
					engineObjects[i].position.z += engineObjects[i].velocity.z * deltaTime;

					// Applying angular momentum
					engineObjects[i].rotation.x += engineObjects[i].angularMo.x * deltaTime;
					engineObjects[i].rotation.y += engineObjects[i].angularMo.y * deltaTime;
					engineObjects[i].rotation.z += engineObjects[i].angularMo.z * deltaTime;
				}
			}
		}
	};
}
