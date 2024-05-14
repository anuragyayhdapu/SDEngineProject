#pragma once

#include <filesystem>
#include <stdexcept>
#include <limits>


struct MemoryFileErrorState
{
	bool m_isGood = true;
	bool m_isBad = false;
	bool m_isFail = false;

	std::string m_errorDescription = "";
};


// class for reading an entire file into memory
// Provide access similar to vector (begin(), end(), data(), size(), at(), operator[], front(), back())
// Exception safe
// Usable as RAII
class MemoryFile
{
protected:
	uint8_t* m_data = nullptr;
	size_t m_size = 0;
	MemoryFileErrorState m_errorState;

public:
	typedef uint8_t* iterator;

	//----------------------------------------------------------------------------------------------
	// not allowed to default construct without a file name
	MemoryFile() = delete;

	//----------------------------------------------------------------------------------------------
	// takes filename, allocates memory size of file, reads file into allocated memory
	explicit MemoryFile(char const* fileName) noexcept
	{
		// allocate memory for file
		std::error_code fileSizeErrorCode;
		m_size = std::filesystem::file_size(fileName, fileSizeErrorCode);
		if (fileSizeErrorCode)
		{
			m_errorState.m_errorDescription = "Unable to get file size";
			m_errorState.m_isFail = true;
			m_errorState.m_isGood = false;

			return;
		}

		if (m_size > std::numeric_limits<std::size_t>::max())
		{
			m_errorState.m_errorDescription = "File too large to hold in memory";
			m_errorState.m_isFail = true;
			m_errorState.m_isGood = false;

			return;
		}

		// open file
		FILE* fileStream = nullptr;
		errno_t fileOpenErrorCode = fopen_s(&fileStream, fileName, "rb");
		if (fileOpenErrorCode != 0 || fileStream == nullptr)
		{
			m_errorState.m_isFail = true;
			m_errorState.m_isGood = false;
			m_errorState.m_errorDescription = "Failed to open file";

			return;
		}

		// read file into memory
		m_data = new(std::nothrow) uint8_t[m_size];
		if (m_data == nullptr)
		{
			m_errorState.m_isBad = true;
			m_errorState.m_isFail = true;
			m_errorState.m_isGood = false;
			m_errorState.m_errorDescription = "Memory allocation failed.";
		}
		else
		{
			size_t const fileReadReturnCode = fread(m_data, sizeof(uint8_t), m_size, fileStream);
			if (fileReadReturnCode != m_size)
			{
				delete[] m_data;
				m_data = nullptr;
				m_size = 0;

				m_errorState.m_isBad = true;
				m_errorState.m_isFail = true;
				m_errorState.m_isGood = false;
				m_errorState.m_errorDescription = "Failed to read file.";
			}
		}

		// close the file after getting the stream
		int fileCloseErrorCode = fclose(fileStream);
		if (fileCloseErrorCode != 0)
		{
			m_errorState.m_isFail = true;
			m_errorState.m_isGood = false;
			m_errorState.m_errorDescription = "Failed to close file.";
		}
	}


	//----------------------------------------------------------------------------------------------
	// copy constructor
	MemoryFile(MemoryFile const& other) noexcept
	{
		m_size = other.m_size;
		m_data = new uint8_t[m_size];
		std::copy(other.m_data, other.m_data + m_size, m_data);

		m_errorState = other.m_errorState;
	};

	//----------------------------------------------------------------------------------------------
	// copy assignment operator
	MemoryFile& operator=(const MemoryFile& other) noexcept
	{
		if (&other == this)
			return *this;

		delete[] m_data;

		m_size = other.m_size;
		m_data = new uint8_t[m_size];
		std::copy(other.m_data, other.m_data + m_size, m_data);

		return *this;
	}

	//----------------------------------------------------------------------------------------------
	// move constructor
	MemoryFile(MemoryFile&& other) noexcept
	{
		m_size = other.m_size;
		m_data = other.m_data;
		m_errorState = std::move(other.m_errorState);

		other.m_size = 0;
		other.m_data = nullptr;
	}

	//----------------------------------------------------------------------------------------------
	// move assignment operator
	MemoryFile& operator=(MemoryFile&& other) noexcept
	{
		if (&other == this)
			return *this;

		delete[] m_data;

		m_size = other.m_size;
		m_data = other.m_data;
		m_errorState = std::move(other.m_errorState);

		other.m_size = 0;
		other.m_data = nullptr;

		return *this;
	}

	//----------------------------------------------------------------------------------------------
	operator bool() const noexcept
	{
		if (m_errorState.m_isFail || m_errorState.m_isBad)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	//----------------------------------------------------------------------------------------------
	~MemoryFile() noexcept
	{
		delete[] m_data;
	}

	//----------------------------------------------------------------------------------------------
	uint8_t* data() noexcept
	{
		return m_data;
	}

	//----------------------------------------------------------------------------------------------
	size_t size() const noexcept
	{
		return m_size;
	}

	//----------------------------------------------------------------------------------------------
	bool isEmpty() const noexcept
	{
		return m_size == 0;
	}

	//----------------------------------------------------------------------------------------------
	iterator begin() noexcept
	{
		return m_data;
	}

	//----------------------------------------------------------------------------------------------
	iterator end() noexcept
	{
		return m_data + m_size;
	}

	//----------------------------------------------------------------------------------------------
	uint8_t& operator[](size_t index) noexcept
	{
		return m_data[index];
	}

	//----------------------------------------------------------------------------------------------
	uint8_t& at(size_t index) noexcept
	{
		return m_data[index];
	}

	//----------------------------------------------------------------------------------------------
	uint8_t& front() noexcept
	{
		return m_data[0];
	}

	//----------------------------------------------------------------------------------------------
	uint8_t& back() noexcept
	{
		return m_data[m_size - 1];
	}

	//----------------------------------------------------------------------------------------------
	MemoryFileErrorState GetMemoryFileState() const noexcept
	{
		return m_errorState;
	}

	//----------------------------------------------------------------------------------------------
	bool good()
	{
		return m_errorState.m_isGood;
	}

	//----------------------------------------------------------------------------------------------
	bool bad()
	{
		return m_errorState.m_isBad;
	}

	//----------------------------------------------------------------------------------------------
	bool fail()
	{
		return m_errorState.m_isFail;
	}
};