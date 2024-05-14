#pragma once

#include <vector>

template< typename T >
void ClearAndDeleteEverything(std::vector<T*>& myVector)
{
	for (int index = 0; index < (int) myVector.size(); ++index)
	{
		delete myVector[index];
	}

	myVector.clear();
}


template< typename T >
void NUllOutAnyReferencesToThisObject(std::vector<T*>& myVector, T* objectToStopPointingTo)
{
	for (int index = 0; index < (int)myVector.size(); ++index)
	{
		if (myVector[index] == objectToStopPointingTo)
		{
			myVector[index] == nullptr;
		}
	}
}