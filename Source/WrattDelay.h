/*
  ==============================================================================

    WrattDelay.h
    Created: 7 Jul 2017 1:45:54pm
    Author:  wrattchri1

  ==============================================================================
*/

#pragma once
#include <vector>

namespace wratt_dsp {

	class Delay
	{
	public:
		Delay();
		float tick(float p_input);

		void setDelay(unsigned long p_delayLen);

		double inline getDelay() { return delayLength; }
		void inline setSampRate(long p_sampRate) { sampleRate = p_sampRate; }
		void inline setMix(float p_mix) { mix = p_mix; }
		float inline getMix() { return mix; }
		void inline setFeedback(float p_feedback) { feedback = p_feedback; }
		float inline getFeedback() { return feedback; }
		long inline getMaxDelay() { return maxDelayLen; }

	private:

		long inline msToSamps(double p_ms) { return static_cast<long>((p_ms / 1000) * sampleRate); }
		double inline sampsToMs(long p_samples) { return static_cast<double>(p_samples / sampleRate * 1000); }

		int					readPos;
		int					writePos;
		float				feedback;
		float				mix;
		unsigned long		delayLength;
		unsigned long		delayLengthSamps;
		long				sampleRate;
		long				maxDelayLen;
		std::vector<float>	delayLine;
	};
}
