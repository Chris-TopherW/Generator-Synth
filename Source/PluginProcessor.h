/*
  ==============================================================================
   
   Chris Wratt, 2017

  ==============================================================================
*/

#pragma once

#include "Definitions.h"
#include "WrattDelay.h"

#ifdef __APPLE__
#include "DSPFilters/Dsp.h"
#include "STK/FreeVerb.h"
#include "STK/Chorus.h"
#include "STK/SineWave.h"

#elif defined _WIN32 || defined _WIN64
#include "DSPFilters\Dsp.h"
#include "STK\FreeVerb.h"
#include "STK\Chorus.h"
#include "STK\SineWave.h"

#endif

//==============================================================================
/**
    As the name suggest, this class does the actual audio processing.
*/
class JuceDemoPluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    JuceDemoPluginAudioProcessor();
    ~JuceDemoPluginAudioProcessor();

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

    //==============================================================================
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
        jassert (! isUsingDoublePrecision());
        process (buffer, midiMessages);
    }

    //==============================================================================
    bool hasEditor() const override                                             { return true; }
    AudioProcessorEditor* createEditor() override;

    //==============================================================================
    const String getName() const override                                       { return JucePlugin_Name; }

    bool acceptsMidi() const override                                           { return true; }
    bool producesMidi() const override                                          { return true; }

    double getTailLengthSeconds() const override                                { return 0.0; }

    //==============================================================================
    int getNumPrograms() override                                               { return 0; }
    int getCurrentProgram() override                                            { return 0; }
    void setCurrentProgram (int /*index*/) override                             {}
    const String getProgramName (int /*index*/) override                        { return String(); }
    void changeProgramName (int /*index*/, const String& /*name*/) override     {}

    //==============================================================================
    void getStateInformation (MemoryBlock&) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // this is kept up to date with the midi messages that arrive, and the UI component
    // registers with it so it can represent the incoming messages
    MidiKeyboardState keyboardState;

    // this keeps a copy of the last set of time info that was acquired during an audio
    // callback - the UI component will read this and display it.
    AudioPlayHead::CurrentPositionInfo lastPosInfo;

    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth, lastUIHeight;

    // Our parameters
    AudioParameterFloat* gainParam;

private:
    //==============================================================================
    void process (AudioBuffer<float>& buffer, MidiBuffer& midiMessages);

	Synthesiser synth;
	std::vector<stk::FreeVerb> reverb;
	std::vector<stk::Chorus> chorus;
	std::vector<wratt_dsp::Delay> delay;
	stk::SineWave lfoSine;

    void initialiseSynth();
	void setFxParams(std::vector<stk::Chorus>&/*, std::vector<wratt_dsp::Delay>&*/, std::vector<stk::FreeVerb>&);
    void updateCurrentTimeInfoFromHost();
    static BusesProperties getBusesProperties();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceDemoPluginAudioProcessor)
};

class ParameterHolder
{
private:
	ParameterHolder();
public:
	std::array<std::atomic<float>, NUM_ALL_PARAMS> parameters;
	int currentOsc = OSC_SAW;

	static ParameterHolder & inst() {
		static ParameterHolder params;
		return params;
	}
};

//ParameterHolder::inst().parameters[0];

