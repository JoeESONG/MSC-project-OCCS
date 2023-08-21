#define NOMINMAX
#include <windows.h>
#include "FileRead.h"
#include <io.h>
#include <iostream>
#include "MyVector.h"

#include <shobjidl.h> 
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include"Commontypes.h"
std::vector<face> processFace(const face& inputFace) {
	std::vector<face> processedFaces;

	int numberOfPoints = inputFace.points.size();

	if (numberOfPoints < 3) {
		std::cerr << "Error: Face has less than 3 points.\n";
		return processedFaces;
	}
	else if (numberOfPoints == 3) {
		// Convert the triangle into a quad.
		face quad = inputFace;
		// Just duplicate one point to make it a quad.
		quad.points.push_back(inputFace.points[1]);
		processedFaces.push_back(quad);
		return processedFaces;
	}
	else if (numberOfPoints == 4) {
		processedFaces.push_back(inputFace);
		return processedFaces;
	}
	else {
		// For faces with more than 4 points.
		for (int i = 1; i <= numberOfPoints - 3; i += 2) {
			face quad;
			quad.points.push_back(inputFace.points[0]);
			quad.points.push_back(inputFace.points[i]);
			quad.points.push_back(inputFace.points[i + 1]);
			quad.points.push_back(inputFace.points[i + 2]);
			processedFaces.push_back(quad);
		}
	}
	return processedFaces;
}

char pathName[255] = { " " }; //select the object
void openFileDialog(char* pathName)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Set file dialog options
			FILEOPENDIALOGOPTIONS options;
			pFileOpen->GetOptions(&options);
			pFileOpen->SetOptions(options | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST);

			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Assign the file path to pathName variable.
					if (SUCCEEDED(hr))
					{
						wcstombs_s(nullptr, pathName, 255, pszFilePath, 255);
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
}
//read the obj file and save the vertices data
struct CompareVec3
{
	bool operator()(const aiVector3D& a, const aiVector3D& b) const
	{
		return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
	}
};
void initVertices()
{
	openFileDialog(pathName);
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(pathName, aiProcessPreset_TargetRealtime_Quality & ~aiProcess_Triangulate);

	if (!scene)
	{
		std::cout << "Failed to load model: " << importer.GetErrorString() << std::endl;
		return;
	}

	vertices.clear();
	faces.clear();

	// Use a map to avoid duplicate vertices
	std::map<aiVector3D, int, CompareVec3> vertexMap;

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			const aiFace& ai_face = mesh->mFaces[j];
			std::vector<int> points;
			for (unsigned int k = 0; k < ai_face.mNumIndices; k++)
			{
				aiVector3D pos = mesh->mVertices[ai_face.mIndices[k]];

				// If this vertex is not in vertexMap, add it
				if (vertexMap.find(pos) == vertexMap.end())
				{
					vertexMap[pos] = vertices.size() + 1; // .obj indices start from 1
					vertices.push_back(Cartesian3(pos.x, pos.y, pos.z));
				}

				points.push_back(vertexMap[pos]);
			}
			face face(points);
			faces.push_back(face);
		}
	}
	//  Process the faces
	std::vector<face> processedFaces;
	for (const auto& inputFace : faces) {
		std::vector<face> newFaces = processFace(inputFace);
		processedFaces.insert(processedFaces.end(), newFaces.begin(), newFaces.end());
	}

	// Replace the original faces with the processed faces
	faces = processedFaces;
	std::cout << "Number of vertices: " << vertices.size() << std::endl;
	std::cout << "Number of faces: " << faces.size() << std::endl;
}
