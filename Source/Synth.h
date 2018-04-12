/*
  ==============================================================================

    Synth.h
    Created: 19 Jul 2017 11:13:01am
    Author:  wrattchri1

  ==============================================================================
*/

#pragma once

#ifdef __APPLE__ 

#include "STK/BlitSquare.h"
#include "STK/BlitSaw.h"
#include "STK/Noise.h"
#include "STK/ADSR.h"
#include "DSPFilters/Dsp.h"

#elif defined _WIN32 || defined _WIN64

#include "STK\BlitSquare.h"
#include "STK\BlitSaw.h"
#include "STK\Noise.h"
#include "STK\ADSR.h"
#include "DSPFilters\Dsp.h"

#endif

#include "PluginProcessor.h"

class OscilSound : public SynthesiserSound
{
public:
	OscilSound() {}
	bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
	bool appliesToChannel(int /*midiChannel*/) override { return true; }
	
};

class OscilVoice : public SynthesiserVoice
{
public:
	OscilVoice();
	bool canPlaySound(SynthesiserSound* sound) override;
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void OscilVoice::pitchWheelMoved(int newValue) override {} ;
	void OscilVoice::controllerMoved(int controllerNumber, int newValue) override {};
	void OscilVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
	{
		processBlock(outputBuffer, startSample, numSamples);
	}

private:
	void processBlock(AudioBuffer<float>& buffer, int startSample, int numSamples);
	float mapFilterParam(float p_adsrAmp, float p_startFreq, float p_endFreq);

	//need to use some polymorphism to pass different waveform as argument?
	stk::SineWave sineWave;
	stk::BlitSquare squareWave;
	stk::BlitSaw sawWave;
	stk::Noise noise;
	

	stk::ADSR adsr;
	stk::ADSR filterEnvelope;
	//Dsp::SimpleFilter <Dsp::Butterworth::LowPass <3>, 2> filter; //needs extra param for smoothing time
	//Dsp::Filter* f = new Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::LowPass <4>, 2>(1024);
	Dsp::Filter* f = new Dsp::FilterDesign<Dsp::Butterworth::Design::LowPass <4>, 2>;
	Dsp::Params filtParams;
	long paramIncrement = PARAM_UPDATE_SPEED;
	long sampleRate;
	float currentFreq;

	/*VOLUME_A_PARAM, VOLUME_D_PARAM, VOLUME_R_PARAM,
		FILTER_A_PARAM, FILTER_D_PARAM, FILTER_R_PARAM, VOLUME_S_PARAM, FILTER_S_PARAM,
		REVERB_PARAM, DELAY_PARAM, CHORUS_PARAM, COOL_EFFECT_PARAM,
		LFO_RATE_PARAM, LFO_AMP_PARAM, FILTER_CUTOFF_PARAM, FILTER_Q_PARAM, NUM_ALL_PARAMS*/
};
