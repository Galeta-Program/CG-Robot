#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <fstream>

#include <glm/glm.hpp>

#include "OBJLoader.hpp"

bool LoadOBJ(
	const char * path, 
	std::vector<unsigned int>& outMaterialIndices,
	std::vector<std::string>& outMtls,
	std::vector<Vertex>& outArrangedVertex,
	std::vector<unsigned int>& outElementIndex
)
{
	printf( "Loading OBJ file %s...\n", path );

	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> textCoords;
	std::map<std::string, int> faceToIndex;

	FILE * file = fopen( path, "r" );
	if( file == NULL )
	{
		printf("Impossible to open the .obj file! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}

	int fcount = 0; // face count for material
	while( 1 )
	{
		char lineHeader[128];
		
		int result = fscanf( file, "%s", lineHeader );
		if (result == EOF)
			break; 
	
		if ( strcmp( lineHeader, "v" ) == 0 )
		{
			glm::vec3 vertex;
			fscanf( file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			vertices.emplace_back(vertex);
		}
		else if ( strcmp( lineHeader, "vt" ) == 0 )
		{
			glm::vec2 tCoord;
			fscanf( file, "%f %f\n", &tCoord.x, &tCoord.y );
			textCoords.emplace_back(tCoord);
		}
		else if ( strcmp(lineHeader, "vn" ) == 0)
		{
			glm::vec3 normal;
			fscanf( file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			normals.emplace_back(normal);
		}
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
			unsigned int vertexIndex[3], texCoordIndex[3], normalIndex[3];
			int matches = fscanf( file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &texCoordIndex[0], &normalIndex[0],
																	   &vertexIndex[1], &texCoordIndex[1], &normalIndex[1],
																	   &vertexIndex[2], &texCoordIndex[2], &normalIndex[2] );
			
			if ( matches != 9 ) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}

			std::string keys[3] = {
				std::to_string( vertexIndex[0]) + "/" + std::to_string(texCoordIndex[0]) + "/" + std::to_string(normalIndex[0] ),
				std::to_string( vertexIndex[1]) + "/" + std::to_string(texCoordIndex[1]) + "/" + std::to_string(normalIndex[1] ),
				std::to_string( vertexIndex[2]) + "/" + std::to_string(texCoordIndex[2]) + "/" + std::to_string(normalIndex[2] ),
			};

			Vertex tmpVertex[3]; // one face have three vertex
			for (int i = 0; i < 3; i++)
			{
				tmpVertex[i].v = vertices[vertexIndex[i] - 1];
				tmpVertex[i].t = textCoords[texCoordIndex[i] - 1];
				tmpVertex[i].n = normals[normalIndex[i] - 1];

				if (faceToIndex.find(keys[i]) == faceToIndex.end())
				{
					outArrangedVertex.emplace_back(tmpVertex[i]);
					faceToIndex[keys[i]] = outArrangedVertex.size() - 1;
				}
				
				outElementIndex.emplace_back(faceToIndex[keys[i]]);
			}

			fcount++;
		}
		else if(strcmp( lineHeader , "usemtl") == 0)
		{
			outMaterialIndices.push_back(fcount);	
			char material[50];
			fscanf(file, "%s", material);
			outMtls.push_back(material);
			fcount = 0;
		}
		else
		{
			char trash[1000];
			fgets(trash, 1000, file);
		}
	}

	outMaterialIndices.push_back(fcount);

	return true;
}

bool LoadMTL(
	const char* path,
	std::vector<glm::vec3>& Kd,
	std::vector<glm::vec3>& Ka,
	std::vector<glm::vec3>& Ks,
	std::vector<std::string>& out_name,
	std::vector<std::string>& map_Kd,
	std::vector<std::string>& map_Ns,
	std::vector<std::string>& map_refl,
	std::vector<std::string>& map_Ke,
	std::vector<std::string>& map_d,
	std::vector<std::string>& map_Bump
)
{
	FILE * file = fopen(path, "r");
	if( file == NULL )
	{
		printf("Impossible to open the .mtl file ! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}

	while( 1 )
	{
		char lineHeader[128];

		int result = fscanf( file, "%s", lineHeader );
		if (result == EOF)
			 break; 

		if ( strcmp( lineHeader , "newmtl") == 0 )
		{
			 char material[50];
			 fscanf( file, "%s", material );
			 out_name.push_back(material);
		}
		else if( strcmp( lineHeader ,"Kd" ) == 0 )
		{
			 glm::vec3 diffuse;
			 fscanf( file, "%f %f %f\n", &diffuse.x, &diffuse.y, &diffuse.z );
			 Kd.push_back(diffuse);
		}
		else if( strcmp( lineHeader ,"Ka" ) == 0 )
		{
			 glm::vec3 ambient;
			 fscanf( file, "%f %f %f\n", &ambient.x, &ambient.y, &ambient.z );
			 Ka.push_back(ambient);
		}
		else if( strcmp( lineHeader ,"Ks" ) == 0 )
		{
			 glm::vec3 specular;
			 fscanf( file, "%f %f %f\n", &specular.x, &specular.y, &specular.z);
			 Ks.push_back(specular);
		}
		else if( strcmp(lineHeader ,"map_Kd" ) == 0 )
		{
			 char buffer[256];
			 fscanf( file, " %[^\n]", buffer );
			 map_Kd.push_back(buffer);
			 Kd.push_back( glm::vec3( 1, 1, 1 ) ); 
		}
		else if ( strcmp( lineHeader, "map_Ns" ) == 0 )
		{
			char buffer[256];
			fscanf(file, " %[^\n]", buffer);
			map_Ns.push_back(buffer);
		}
		else if ( strcmp( lineHeader, "map_refl" ) == 0 )
		{
			char buffer[256];
			fscanf(file, " %[^\n]", buffer);
			map_refl.push_back(buffer);
		}
		else if ( strcmp( lineHeader, "map_Ke" ) == 0 )
		{
			char buffer[256];
			fscanf(file, " %[^\n]", buffer);
			map_Ke.push_back(buffer);
		}
		else if ( strcmp( lineHeader, "map_d" ) == 0 )
		{
			char buffer[256];
			fscanf(file, " %[^\n]", buffer);
			map_d.push_back(buffer);
		}
		else if ( strcmp( lineHeader, "map_Bump" ) == 0 )
		{
			char buffer[256];

			fscanf(file, "%s", &buffer);
			fscanf(file, "%s", &buffer);

			fscanf(file, " %[^\n]", buffer);
			map_Bump.push_back(buffer);
		}
		else
		{
			 char trash[1000];
			 fgets(trash, 1000, file);
		}
	}
	return true;
}