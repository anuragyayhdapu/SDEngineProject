#pragma once

#include "Engine/Core/EngineCommon.hpp"

#include <string>
#include <vector>

// Read the contents of filename, as a binary file, to outBuffer.
int FileReadToBuffer(std::vector<uint8_t>& outbuffer, const std::string& filename);

// Read the contents of filename, as a binary file, to outBuffer
int FileReadToString(std::string& outString, const std::string& filename);