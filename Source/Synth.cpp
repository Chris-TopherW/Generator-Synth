/*
==============================================================================

Synth.cpp
Created: 19 Jul 2017 11:13:01am
Author:  wrattchri1

==============================================================================
*/

#include "Synth.h"

//==============================================================================

OscilVoice::OscilVoice()
{
	sampleRate = this->getSampleRate();
	sineWave.setSampleRate(sampleRate);
	squareWave.setSampleRate(sampleRate);
	sawWave.setSampleRate(sampleRate);
	noise.setSampleRate(sampleRate);
	adsr.setSampleRate(sampleRate);

	adsr.setAllTimes(	ParameterHolder::inst().parameters[VOLUME_A_PARAM].load(),
						ParameterHolder::inst().parameters[VOLUME_D_PARAM].load(),
						ParameterHolder::inst().parameters[VOLUME_S_PARAM].load(),
						ParameterHolder::inst().parameters[VOLUME_R_PARAM].load()
	);
	filterEnvelope.setAllTimes(ParameterHolder::inst().parameters[FILTER_A_PARAM].load(),
						ParameterHolder::inst().parameters[FILTER_D_PARAM].load(),
						ParameterHolder::inst().parameters[FILTER_S_PARAM].load(),
						ParameterHolder::inst().parameters[FILTER_R_PARAM].load()
	);

	filtParams[0] = sampleRate;
	filtParams[1] = 4; // order
	filtParams[2] = ParameterHolder::inst().parameters[FILTER_START_PARAM].load(); //cutoff
	f->setParams(filtParams);
}

bool OscilVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<OscilSound*> (sound) != nullptr;
}

void OscilVoice::startNote(int midiNoteNumber, float velocity,
	SynthesiserSound* sound,
	int currentPitchWheelPosition)
{
	adsr.setAllTimes(	ParameterHolder::inst().parameters[VOLUME_A_PARAM].load(),
						ParameterHolder::inst().parameters[VOLUME_D_PARAM].load(),
						ParameterHolder::inst().parameters[VOLUME_S_PARAM].load(),
						ParameterHolder::inst().parameters[VOLUME_R_PARAM].load()
	);
	filterEnvelope.setAllTimes(ParameterHolder::inst().parameters[FILTER_A_PARAM].load(),
						ParameterHolder::inst().parameters[FILTER_D_PARAM].load(),
						ParameterHolder::inst().parameters[FILTER_S_PARAM].load(),
						ParameterHolder::inst().parameters[FILTER_R_PARAM].load()
	);

	adsr.keyOn();
	filterEnvelope.keyOn();

	currentFreq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
	sineWave.setFrequency(currentFreq);
	squareWave.setFrequency(currentFreq);
	sawWave.setFrequency(currentFreq);
}

void OscilVoice::stopNote(float velocity, bool allowTailOff)
{
	adsr.keyOff();
	filterEnvelope.keyOff();
}

void OscilVoice::processBlock(AudioBuffer<float>& buffer, int startSample, int numSamples)
{
	const int numChannels = buffer.getNumChannels();
	float param;
	float filterStartFreq = ParameterHolder::inst().parameters[FILTER_START_PARAM].load();
	float filterEndFreq = ParameterHolder::inst().parameters[FILTER_END_PARAM].load();
	int currentOsc = ParameterHolder::inst().currentOsc;

	for (int sample = 0; sample < numSamples; sample++)
	{
		float curOutput;
		//need to get this into polymorphic structure?
		switch (currentOsc)
		{
		case OSC_SINE:
			curOutput = sineWave.tick();
			break;
		case OSC_SQUARE:
			curOutput = squareWave.tick();
			break;
		case OSC_SAW:
			curOutput = sawWave.tick();
			break;
		case OSC_NOISE:
			curOutput = noise.tick();
			break;
		}
		curOutput *= adsr.tick();
		//filter. need to run tick every sample but not change filter freq. Could implement lerp... No needs to be special lerp filter for that
		param = filterEnvelope.tick();

		//change paramaters here- not dependant on block size, is dependant on SR...
		//moving to modulo for checks took 7% off my CPU time!!
		if (paramIncrement == 0)
		{
			param = mapFilterParam(param, filterStartFreq, filterEndFreq) + currentFreq;
			//check needed to avoid filter shutting down plugin with argument above nyquist
			if (param > 20000)		param = 20000;
			else if(param <= 20)	param = 20;
			f->setParam(2, param);
			paramIncrement = PARAM_UPDATE_SPEED;
		}
		else
		{
			paramIncrement--;
		}

		//some gross stuff to deal with DSP lib formatting.
		float* curSampArray[PLUGIN_CHANNELS];
		for (int channel = numChannels; --channel >= 0;) curSampArray[channel] = &curOutput;
		f->process(1, curSampArray);

		for (int channel = 0; channel < numChannels; channel++)
		{
			buffer.addSample(channel, startSample, curOutput);
		}
		++startSample;
	}
}

//returns the frequency mapped between the start and end freqs
float OscilVoice::mapFilterParam(float p_adsrAmp, float p_startFreq, float p_endFreq)
{
	float freqRange = 19980.0;
	//find min and max and scale them into 0-1
	float minVal = p_startFreq <= p_endFreq ? p_startFreq / freqRange : p_endFreq / freqRange;
	float maxVal = p_startFreq >= p_endFreq ? p_startFreq / freqRange : p_endFreq / freqRange;
	float difference = maxVal - minVal;

	//Set output based on whether the start or end is higher pitched
	if (p_startFreq < p_endFreq)
	{
		return (p_adsrAmp * difference + minVal) * freqRange;
	}
	else if (p_startFreq > p_endFreq)
	{
		return (maxVal - (p_adsrAmp * difference)) * freqRange;
	}
	else
		return p_startFreq; //no difference
}
