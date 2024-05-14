#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <errno.h>


int FileReadToBuffer(std::vector<uint8_t>& outbuffer, const std::string& filename)
{
	FILE* fileStream = nullptr;
	char const* mode = "r";

	errno_t error;
	error = fopen_s(&fileStream, filename.c_str(), mode);

	if (error)
	{
		if (error == EACCES)
		{
			ERROR_RECOVERABLE(
				Stringf("Unable to open file. Permission denied. "
					"The file's permission setting doesn't allow the specified access. "
					"file: %s", filename.c_str()) );
		}
		else if (error == EFAULT)
		{
			ERROR_RECOVERABLE(
				Stringf("Unable to open file. Bad addres. "
					"file: %s", filename.c_str()));
		}
		else if (error == EIO)
		{
			ERROR_RECOVERABLE(
				Stringf("I/O error. "
					"file: %s", filename.c_str()));
		}
		else if (error == ENAMETOOLONG)
		{
			ERROR_RECOVERABLE(
				Stringf("Unable to open file. Filename too long. "
					"file: %s", filename.c_str()));
		}
		else if (error == ENOENT)
		{
			ERROR_RECOVERABLE(
				Stringf("No such file or directory. "
					"The specified file or directory doesn't exist or can't be found. "
					"file: %s", filename.c_str()));
		}
		else if (error == ENOTTY)
		{
			ERROR_RECOVERABLE(
				Stringf("Unable to open file. "
					"Inappropriate I/O control operation. "
					"file: %s", filename.c_str()));
		}
	}
	else // File opened for read successfully
	{
		if (fileStream != nullptr)
		{
			// read contents of file into a file buffer
			int character = fgetc(fileStream);
			while (character != EOF)
			{
				outbuffer.push_back((uint8_t)character);

				character = fgetc(fileStream);
			}

			// close file
			fclose(fileStream);

			return true;
		}
	}

	return false;
}

int FileReadToString(std::string& outString, const std::string& filename)
{
	std::vector<uint8_t> buffer;
	if (FileReadToBuffer(buffer, filename))
	{
		for (size_t bufferIndex = 0; bufferIndex < buffer.size(); bufferIndex++)
		{
			uint8_t character = buffer[bufferIndex];
			outString.push_back(character);
		}

		return true;
	}

	return false;
}
