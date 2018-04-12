/*
  ==============================================================================

    WrattDelay.cpp
    Created: 7 Jul 2017 1:45:54pm
    Author:  wrattchri1

  ==============================================================================
*/

#include "WrattDelay.h"
#include "Definitions.h"

namespace wratt_dsp {

	Delay::Delay()
		:
		feedback(0.5),
		delayLength(300),
		sampleRate(44100),
		maxDelayLen(44100),
		mix(0.3)
	{
		delayLine.reserve(msToSamps(maxDelayLen));
		for (int sample = 0; sample < msToSamps(maxDelayLen); sample++)
		{
			delayLine.push_back(0.0);
		}
		readPos = 0;
		delayLengthSamps = msToSamps(delayLength);
		writePos = readPos + delayLengthSamps - 1;
	}

	void Delay::setDelay(unsigned long p_delayLen)
	{
		if (p_delayLen < maxDelayLen)
		{
			delayLength = p_delayLen;
		}
		else
		{
			delayLength = maxDelayLen - 1;
		}
		delayLengthSamps = msToSamps(delayLength);
	}

	float Delay::tick(float p_input)
	{
		float output = delayLine[readPos];
		delayLine[writePos] = p_input + output * feedback;
		readPos++;

		if (readPos >= delayLengthSamps)
		{
			readPos -= delayLengthSamps;
		}
		writePos++;
		if (writePos >= delayLengthSamps)
		{
			writePos -= delayLengthSamps;
		}

		output = delayLine[readPos] * mix + p_input * (1.0f - mix);
		return output;
	}
}
