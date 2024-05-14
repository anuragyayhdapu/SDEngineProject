#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "ThirdParty/squirrel/RawNoise.hpp"

#include <cstdlib>



int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	unsigned int randomUInt = Get1dNoiseUint(m_position++, m_seed);
	return randomUInt % maxNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	int randNum = RollRandomIntLessThan(maxInclusive - minInclusive + 1);
	int randNumInRange = randNum + minInclusive;

	return randNumInRange;
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return Get1dNoiseZeroToOne(m_position++, m_seed);
}

float RandomNumberGenerator::RollRandomFloatLessThan(float maxNotInclusive)
{
	float scale = maxNotInclusive - 1;
	float randZeroToOne = RollRandomFloatZeroToOne();
	
	float randFloat = randZeroToOne * scale;
	return randFloat;
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	float range = maxInclusive - minInclusive;
	float randZeroToOne = RollRandomFloatZeroToOne();

	float randomInRange = minInclusive + randZeroToOne * range;
	return randomInRange;
}
