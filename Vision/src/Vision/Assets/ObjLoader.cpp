#include "pch.hpp"

#include "Vision/Assets/ObjLoader.h"
#include "Vision/Platform/File.h"
#include "Vision/Core/Logger.h"

#include <cstdio>

namespace Vision
{
    static void ParseVertex(const std::string& vertexString,
                            const std::vector<glm::vec3>& positions,
                            const std::vector<glm::vec2>& uvs,
                            const std::vector<glm::vec3>& normals,
                            std::unordered_map<uint64, uint32>& vertexToIndex, 
                            std::vector<Vertex>& vertices,
                            std::vector<uint32>& indicies)
    {
        uint32 positionIndex;
        uint32 uvIndex;
        uint32 normalIndex;
        
        // Todo(Harlequin): assuming we have positions uvs normals otherwise we should generate them
        sscanf(vertexString.c_str(), "%d/%d/%d", &positionIndex, &uvIndex, &normalIndex);

        uint64 positionCount = positions.size();
        uint64 uvCount = uvs.size();
        uint64 vertexHash = positionIndex + (positionCount * uvIndex) + (normalIndex * positionCount * uvCount);

        uint32 vertexIndex;
        auto vertexIter = vertexToIndex.find(vertexHash);

        if (vertexIter == vertexToIndex.end())
        {
            Vertex vertex;
            vertex.Position = positions[positionIndex - 1];
            vertex.UV = uvs[uvIndex - 1];
            vertex.Normal = normals[normalIndex - 1];

            vertexIndex = vertices.size();
            vertices.push_back(vertex);

            vertexToIndex.emplace(vertexHash, vertexIndex);
        }
        else
        {
            vertexIndex = vertexIter->second;
        }

        indicies.push_back(vertexIndex);
    }

    bool LoadObjMesh(const char* filePath, Mesh *outMesh)
    {
        FileStream handle = File::Open(filePath, FileMode::Read);

        if (!handle.IsOpen())
        {
            VnCoreError("failed to load obj file: {0}.", filePath);
            return false;
        }

        std::string line;

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        std::unordered_map<uint64, uint32> vertexToIndex;

        std::vector<Vertex> vertices;
        std::vector<uint32> indicies;

        while (File::ReadLine(handle, line))
        {
            std::stringstream ss(line);

            std::string token;
            ss >> token;

            if (token == "v")
            {
                glm::vec3 position;
                ss >> position.x >> position.y >> position.z;
                positions.push_back(position);
            }
            else if (token == "vt")
            {
                glm::vec2 uv;
                ss >> uv.x >> uv.y;
                uvs.push_back(uv);
            }
            else if (token == "vn")
            {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            }
            else if (token == "f")
            {
                std::string vertexString;
                
                for (uint32 i = 0; i < 3; i++)
                {
                    ss >> vertexString;
                    ParseVertex(vertexString, positions, uvs, normals, vertexToIndex, vertices, indicies);
                }
            }
        }

        outMesh->Vertices = std::move(vertices);
        outMesh->Indicies = std::move(indicies);

        File::Close(handle);

        return true;
    }
}