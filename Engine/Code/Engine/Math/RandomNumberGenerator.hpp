#pragma once

class RandomNumberGenerator
{
public:
	int RollRandomIntLessThan(int maxNotInclusive);
	int RollRandomIntInRange(int minInclusive, int maxInclusive);
	float RollRandomFloatZeroToOne();
	float RollRandomFloatLessThan(float maxNotInclusive);
	float RollRandomFloatInRange(float minInclusive, float maxInclusive);

	void SetSeed(unsigned int newSeed) { m_seed = newSeed; }

private:
	unsigned int m_seed = 0;
	int m_position = 0;
};