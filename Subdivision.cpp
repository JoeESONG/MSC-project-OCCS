#include "MyVector.h"
#include"Subdivision.h"
#include <io.h>
#include <iostream>
#include "MyVector.h"
#include <shobjidl.h> 
#include"Commontypes.h"
#include"Render.h"
#include <unordered_map>
#include"FileRead.h"
#include <fstream>
#include <direct.h>   
#include <windows.h>
#include<string.h>
Cartesian3 computeNewRulePosition(int vertexIndex, float curvature, const std::vector<Cartesian3>& vertices, const std::vector<face>& faces) {
	std::vector<Cartesian3> neighbors;

	for (const face& f : faces) {
		for (int i = 0; i < f.points.size(); i++) {
			if (f.points[i] == vertexIndex + 1) {
				int nextVertexIndex = f.points[(i + 1) % f.points.size()] - 1;
				neighbors.push_back(vertices[nextVertexIndex]);
			}
		}
	}

	Cartesian3 averagePosition;
	for (const Cartesian3& neighbor : neighbors) {
		averagePosition += neighbor;
	}
	averagePosition = averagePosition / static_cast<float>(neighbors.size());

	//计算新的位置，新的位置是当前顶点的位置和相邻顶点的平均位置的插值
	// 曲率较大的顶点会更接近相邻顶点的平均位置
	Cartesian3 newPosition = vertices[vertexIndex].mul(1 - curvature).add(averagePosition.mul(curvature));

	return newPosition;
}
std::vector<float> computeCurvatures(const std::vector<Cartesian3>& vertices, const std::vector<face>& faces) {
	std::vector<float> curvatures(vertices.size(), 0.0f);
	std::vector<int> neighborCounts(vertices.size(), 0);

	for (const face& f : faces) {
		for (int i = 0; i < f.points.size(); i++) {
			int index1 = f.points[i] - 1;
			int index2 = f.points[(i + 1) % f.points.size()] - 1;

			float distance = vertices[index1].minus(vertices[index2]).magnitude();


			curvatures[index1] += distance;
			neighborCounts[index1]++;

			curvatures[index2] += distance;
			neighborCounts[index2]++;
		}
	}

	for (int i = 0; i < vertices.size(); i++) {
		if (neighborCounts[i] > 0) {
			curvatures[i] /= neighborCounts[i];
		}
	}

	float minCurvature = *std::min_element(curvatures.begin(), curvatures.end());
	float maxCurvature = *std::max_element(curvatures.begin(), curvatures.end());

	for (float& curvature : curvatures) {
		curvature = (curvature - minCurvature) / (maxCurvature - minCurvature);
	}

	return curvatures;
}



void Adaptive_Subdivision(){
	
	//  Compute the parameter
	normals.clear();
	normals.reserve(vertices.size());


	//  Build a map for faster lookups
	std::map<int, std::vector<int>> vertexFaceMap;
	for (int j = 0; j < faces.size(); j++) {
		for (int k = 0; k < faces[j].points.size(); k++) {
			vertexFaceMap[faces[j].points[k]].push_back(j);
		}
	}
	//  Calculate vertex normal
	normals.clear();
	normals.reserve(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		std::vector<Cartesian3> normalVector;  // Create a vector to save all the face normals connected with each vertex
		for (const auto& j : vertexFaceMap[i + 1]) {
			// Calculate the normals for each pair of vectors in the face
			int numPoints = faces[j].points.size();
			for (int k = 0; k < numPoints; k++) {
				Cartesian3 normal1 = vertices[faces[j].points[k % numPoints] - 1] - vertices[faces[j].points[(k + 1) % numPoints] - 1];
				Cartesian3 normal2 = vertices[faces[j].points[(k + 1) % numPoints] - 1] - vertices[faces[j].points[(k + 2) % numPoints] - 1];
				Cartesian3 normal = normal1.cross(normal2);
				float length = normal.magnitude();
				if (length != 0.0f) {  // Check for zero length
					normal /= length;  // Unit normal
					normalVector.push_back(normal);  // Save it in the vector
				}
			}
		}
		Cartesian3 avgNormal;  // Create the vertex normal
		for (const auto& normal : normalVector) {
			avgNormal += normal;
		}
		float length2 = avgNormal.magnitude();
		if (length2 != 0.0f) {  // Check for zero length
			avgNormal /= length2;  // Unit the normal
		}
		normals.push_back(avgNormal);  // Save the vertex normal
	}
	int oldVerticesCount = vertices.size();
	newFaces.clear();

	vertices.reserve(oldVerticesCount + faces.size());

	//face points
	for (int i = 0; i < faces.size(); i++)
	{
		//sum up all vertices coodinates in a quad, and divide it using the number of the vertices
		Cartesian3 facePoint;
		for (int j = 0; j < faces[i].points.size(); j++)
			facePoint += vertices[faces[i].points[j] - 1];
		facePoint /= faces[i].points.size();
		vertices.push_back(facePoint);
		faces[i].numberOfPoint = vertices.size();
	}


	std::map<std::pair<int, int>, int> vertexMap;


	// Save all lines
	for (int i = 0; i < faces.size(); i++)
	{
		for (int j = 0; j < faces[i].lines.size(); j++)
		{
			faces[i].lines[j]->faces.clear();
			faces[i].lines[j]->faces.push_back(faces[i].numberOfPoint);

			for (int i1 = 0; i1 < faces.size(); i1++)
			{
				if (i1 == i)
					continue;
				for (int j1 = 0; j1 < faces[i1].lines.size(); j1++)
				{
					if (((faces[i].lines[j]->start == faces[i1].lines[j1]->start) && (faces[i].lines[j]->end == faces[i1].lines[j1]->end)) ||
						((faces[i].lines[j]->start == faces[i1].lines[j1]->end) && (faces[i].lines[j]->end == faces[i1].lines[j1]->start)))
					{
						faces[i].lines[j]->faces.push_back(faces[i1].numberOfPoint);
					}
				}
			}
		}
	}

	// Edge points
	std::vector<std::pair<edge*, int>> nvv;
	for (int i = 0; i < faces.size(); i++)
	{
		for (int j = 0; j < faces[i].lines.size(); j++)
		{
			Cartesian3 tmp;
			Cartesian3 inputVertice1 = vertices[faces[i].lines[j]->start - 1];
			Cartesian3 inputVertice2 = vertices[faces[i].lines[j]->end - 1];
			Cartesian3 edgePoint = inputVertice1 + inputVertice2;
			edgePoint = edgePoint * 0.5;
			for (int k = 0; k < faces[i].lines[j]->faces.size(); k++)
			{
				tmp += vertices[faces[i].lines[j]->faces[k] - 1];
			}
			if (faces.size() > 1)
			{
				tmp = tmp * (1.f / faces[i].lines[j]->faces.size());
				tmp = tmp + edgePoint;
				tmp = tmp * 0.5;
			}
			else
			{
				tmp = edgePoint;
			}
			bool skip = false;
			for (int x = 0; x < nvv.size(); x++)
			{
				if (((nvv[x].first->start == faces[i].lines[j]->start) && (nvv[x].first->end == faces[i].lines[j]->end)) ||
					((nvv[x].first->start == faces[i].lines[j]->end) && (nvv[x].first->end == faces[i].lines[j]->start)))
				{
					skip = true;
					faces[i].lines[j]->point_count = nvv[x].second;
				}
			}
			if (!skip)
			{
				vertices.push_back(tmp);
				faces[i].lines[j]->point_count = vertices.size();
				nvv.push_back(std::pair<edge*, int>(faces[i].lines[j], vertices.size()));
			}
		}
	}

	// Create a new faces vector
	for (int i = 0; i < faces.size(); i++)
	{
		std::vector<int> ps;
		ps.push_back(faces[i].lines[0]->start);
		ps.push_back(faces[i].lines[0]->point_count);
		ps.push_back(faces[i].numberOfPoint);
		ps.push_back(faces[i].lines[faces[i].lines.size() - 1]->point_count);
		newFaces.push_back(ps);
		ps.clear();

		ps.push_back(faces[i].lines[0]->point_count);
		ps.push_back(faces[i].lines[0]->end);
		ps.push_back(faces[i].lines[1]->point_count);
		ps.push_back(faces[i].numberOfPoint);
		newFaces.push_back(ps);
		ps.clear();

		if (faces[i].lines.size() > 3)
		{
			ps.push_back(faces[i].lines[3]->point_count);
			ps.push_back(faces[i].numberOfPoint);
			ps.push_back(faces[i].lines[2]->point_count);
			ps.push_back(faces[i].lines[2]->end);
			newFaces.push_back(ps);
			ps.clear();
		}

		ps.push_back(faces[i].numberOfPoint);
		ps.push_back(faces[i].lines[1]->point_count);
		ps.push_back(faces[i].lines[1]->end);
		ps.push_back(faces[i].lines[2]->point_count);
		newFaces.push_back(ps);
	}


	std::vector<Cartesian3> Sum(oldVerticesCount);

	for (int i = 0; i < oldVerticesCount; i++) {
		int degree = 0;

		// Compute the degree of the vertex
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].points.size(); k++) {
				if ((i + 1) == faces[j].points[k]) {
					degree++;
				}
			}
		}

		Cartesian3 qSum;
		Cartesian3 rSum;
		std::vector<Cartesian3> qVector;
		std::vector<Cartesian3> rVector;

		// Compute Q and R vectors for the vertex
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].points.size(); k++) {
				if ((i + 1) == faces[j].points[k]) {
					Cartesian3 qSet;
					Cartesian3 rSet;

					for (int x = 0; x < faces[j].points.size(); x++) {
						qSet += vertices[faces[j].points[x] - 1];
					}
					qSet /= faces[j].points.size();
					qVector.push_back(qSet);

					rSet = vertices[faces[j].points[k % faces[j].points.size()] - 1] + vertices[faces[j].points[(k + 1) % faces[j].points.size()] - 1];
					rSet *= 0.5;
					rVector.push_back(rSet);
				}
			}
		}
		// Compute the sum of Q and R vectors
		for (int j = 0; j < qVector.size(); j++) {
			qSum += qVector[j];
		}
		qSum /= qVector.size();

		for (int j = 0; j < rVector.size(); j++) {
			rSum += rVector[j];
		}
		rSum /= rVector.size();
//for evaluation
		std::cout << "Vertex " << i + 1 << ":" << std::endl;
		std::cout << "Q: (" << qSum.x << ", " << qSum.y << ", " << qSum.z << ")" << std::endl;
		std::cout << "R: (" << rSum.x << ", " << rSum.y << ", " << rSum.z << ")" << std::endl;
		std::cout << "-----------------------" << std::endl;
		// Compute the final vertex position based on degree
		//Formula for different degree
		if (degree == 3) {
			Sum[i] = qSum * (1.0f / 3.0f) + rSum * (2.0f / 3.0f);
		}
		else if (degree == 4) {
			Sum[i] = qSum * (1.0f / 4.0f) + rSum * (1.0f / 2.0f) + vertices[i] * (1.0f / 4.0f);
		}
		else if (degree == 5) {
			Sum[i] = qSum * (1.0f / 5.0f) + rSum * (2.0f / 5.0f) + vertices[i] * (2.0f / 5.0f);
		}
		else if (degree == 6) {
			Sum[i] = qSum * (1.0f / 6.0f) + rSum * (1.0f / 3.0f) + vertices[i] * (1.0f / 2.0f);
		}
		else if (degree == 7) {
			Sum[i] = qSum * (1.0f / 7.0f) + rSum * (2.0f / 7.0f) + vertices[i] * (4.0f / 7.0f);
		}
		else if (degree == 8) {
			Sum[i] = qSum * (1.0f / 8.0f) + rSum * (1.0f / 4.0f) + vertices[i] * (5.0f / 8.0f);
		}
		else if (degree == 9) {
			Sum[i] = qSum * (1.0f / 9.0f) + rSum * (2.0f / 9.0f) + vertices[i] * (7.0f / 9.0f);
		}
		else if (degree == 10) {
			Sum[i] = qSum * (1.0f / 10.0f) + rSum * (1.0f / 5.0f) + vertices[i] * (7.0f / 10.0f);
		}
	}
	std::vector<float> curvatures = computeCurvatures(vertices, faces);

	for (int i = 0; i < oldVerticesCount; i++) {
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].points.size(); k++) {
				if ((i + 1) == faces[j].points[k]) {
					int otherVec = faces[j].points[(k + 1) % faces[j].points.size()] - 1;

					// Compute the dot product of normals
					float result = normals[i].x * normals[otherVec].x + normals[i].y * normals[otherVec].y + normals[i].z * normals[otherVec].z;
					extern float Fa;

					// Compute the new vertex position
					Cartesian3 newPosition = computeNewRulePosition(i, curvatures[i], vertices, faces);
					// Update the vertex position based on the dot product
					if (Fa == 0) {
						vertices[i] = Sum[i];
					}
					else if (result > Fa || result < -Fa) {
						vertices[i] = newPosition;
					}
				}
			}
		}
	}
	faces = newFaces; // Update the face vector
//for evaluation
	std::cout << "Number of vertices: " << vertices.size() << std::endl;
	std::cout << "Number of faces: " << faces.size() << std::endl;
	// Print out the vertex information
	for (int i = 0; i < vertices.size(); i++) {
		const auto& vertex = vertices[i];
		std::cout << "vertex" << (i + 1) << ": "
			<< vertex.x << " "
			<< vertex.y << " "
			<< vertex.z << std::endl;
	}
	glfwSwapBuffers(window); 
	glfwPollEvents();
}
bool outputToOBJ(const std::string& filename,
	const std::vector<Cartesian3>& vertices,
	const std::vector<face>& faces) {
	std::ofstream file(filename);

	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
		return false;
	}

	// Output vertices
	for (const auto& vertex : vertices) {
		file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
	}

	// Output faces
	for (const auto& f : faces) {
		file << "f ";
		for (const auto& point : f.points) {
			file << point << " "; // assuming 1-based index in obj
		}
		file << "\n";
	}

	file.close();
	return true;
}



