#pragma once
#include "MyVector.h"
#include <string>
#ifndef SUBDIVISION_H
#define SUBDIVISION_H
void Adaptive_Subdivision();
bool outputToOBJ(const std::string& filename,
    const std::vector<Cartesian3>& vertices,
    const std::vector<face>& faces);
#endif