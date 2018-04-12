/*
  ==============================================================================

   Chris Wratt, 2017

  ==============================================================================
*/

#ifdef __APPLE__ 

#include "PluginProcessor.h"
#include "PluginEditor.h"

#elif defined _WIN32 || defined _WIN64

#include "PluginProcessor.h"
#include "PluginEditor.h"

#endif

#include "Synth.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter();

//==============================================================================
JuceDemoPluginAudioProcessor::JuceDemoPluginAudioProcessor()
    #ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	)
#endif
      , lastUIWidth (400),
      lastUIHeight (200),
      gainParam (nullptr)
{
    lastPosInfo.resetToDefault();

	stk::FreeVerb verbToPush;
	stk::Chorus chorusToPush;
	//wratt_dsp::Delay delayToPush;

	for (int channel = 0; channel < PLUGIN_CHANNELS; channel++)
	{
		reverb.push_back(verbToPush);
		chorus.push_back(chorusToPush);
		//delay.push_back(delayToPush);
	}
	setFxParams(chorus, reverb);
    
	initialiseSynth();
}

void JuceDemoPluginAudioProcessor::setFxParams(std::vector<stk::Chorus>& chorus/*, 
											   std::vector<wratt_dsp::Delay>& delay*/,
											   std::vector<stk::FreeVerb>& reverb)
{

	ParameterHolder::inst().parameters[VOLUME_A_PARAM].store(volumeDefaultA / 1000.0);
	ParameterHolder::inst().parameters[VOLUME_D_PARAM].store(volumeDefaultD / 1000.0);
	ParameterHolder::inst().parameters[VOLUME_S_PARAM].store(volumeDefaultS / 1000.0);
	ParameterHolder::inst().parameters[VOLUME_R_PARAM].store(volumeDefaultR / 1000.0);

	ParameterHolder::inst().parameters[FILTER_A_PARAM].store(filterDefaultA / 1000.0);
	ParameterHolder::inst().parameters[FILTER_D_PARAM].store(filterDefaultD / 1000.0);
	ParameterHolder::inst().parameters[FILTER_S_PARAM].store(filterDefaultS / 1000.0);
	ParameterHolder::inst().parameters[FILTER_R_PARAM].store(filterDefaultR / 1000.0);

	ParameterHolder::inst().parameters[FILTER_START_PARAM].store(filterStartDefault);
	ParameterHolder::inst().parameters[FILTER_END_PARAM].store(filterEndDefault);

	ParameterHolder::inst().currentOsc = defaultOsc;
}

JuceDemoPluginAudioProcessor::~JuceDemoPluginAudioProcessor()
{
}

void JuceDemoPluginAudioProcessor::initialiseSynth()
{
    const int numVoices = defaultNumVoices;

    // Add some voices...
    for (int i = numVoices; --i >= 0;)
        synth.addVoice (new OscilVoice());

    // ..and give the synth a sound to play
    synth.addSound (new OscilSound());
}

//==============================================================================
#ifndef JucePlugin_PreferredChannelConfigurations
bool JuceDemoPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

AudioProcessor::BusesProperties JuceDemoPluginAudioProcessor::getBusesProperties()
{
    return BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true);
}


//==============================================================================
void JuceDemoPluginAudioProcessor::prepareToPlay (double newSampleRate, int /*samplesPerBlock*/)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate (newSampleRate);
    keyboardState.reset();
    reset();
}

void JuceDemoPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

void JuceDemoPluginAudioProcessor::reset()
{
}

void JuceDemoPluginAudioProcessor::process (AudioBuffer<float>& buffer,
                                            MidiBuffer& midiMessages)
{
	//if (midiMessages.getNumEvents() > 0)
	//{
	//	std::vector<unsigned char> mes;
	//	for (int i = 0; i < midiMessages.data.size(); i++)
	//	{
	//		mes.push_back(midiMessages.data[i]);
	//	}
	//	int i = 0;
	//}
    int numSamples = buffer.getNumSamples();
	int numChannels = buffer.getNumChannels();

    // and now get our synth to process these midi events and generate its output.
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);

	float chorusMix = ParameterHolder::inst().parameters[CHORUS_MIX_PARAM].load();
	float chorusModAmt = ParameterHolder::inst().parameters[CHORUS_MODAMT_PARAM].load();
	float chorusModSpeed = ParameterHolder::inst().parameters[CHORUS_MOD_SPEED_PARAM].load();
	float reverbMix = ParameterHolder::inst().parameters[REVERB_MIX_PARAM].load();
	float reverbLen = ParameterHolder::inst().parameters[REVERB_LEN_PARAM].load();
	float delayMix = ParameterHolder::inst().parameters[DELAY_MIX_PARAM].load();
	float delayLen = ParameterHolder::inst().parameters[DELAY_LEN_PARAM].load();
	float delayFeedback = ParameterHolder::inst().parameters[DELAY_FEEDBACK_PARAM].load();
	float lfoRate = ParameterHolder::inst().parameters[LFO_RATE_PARAM];
	lfoSine.setFrequency(lfoRate);
	float lfoAmount = ParameterHolder::inst().parameters[LFO_AMP_PARAM];

	float* writePos[8];
	for (int channel = 0; channel < numChannels; channel++)
	{
		writePos[channel] = buffer.getWritePointer(channel);
		chorus[channel].setModDepth(chorusModAmt);
		chorus[channel].setModFrequency(chorusModSpeed);
		reverb[channel].setRoomSize(reverbLen);
		//delay[channel].setDelay(delayLen);
		//delay[channel].setFeedback(delayFeedback);
	}

	for (int sample = 0; sample < numSamples; sample++)
	{
		float lfoTickHolder = (lfoSine.tick() + 1.0) / 0.5;
		if (lfoRate == 0 || lfoAmount == 0) lfoTickHolder = 1.0;
		for (int channel = 0; channel < numChannels; channel++)
		{
			writePos[channel][sample] = lfoTickHolder * lfoAmount * writePos[channel][sample] + writePos[channel][sample] * (1.0f - lfoAmount);
			writePos[channel][sample] = chorus[channel].tick(writePos[channel][sample]) * chorusMix
				+ writePos[channel][sample] * (1.0f - chorusMix);
			writePos[channel][sample] = reverb[channel].tick(writePos[channel][sample]) * reverbMix
				+ writePos[channel][sample] * (1.0f - reverbMix);
			//writePos[channel][sample] = delay[channel].tick(writePos[channel][sample]) * delayMix
			//	+ writePos[channel][sample] * (1.0f - delayMix);
			//this is causing a massive error - taken out 2/10/2018

		}
	}
}

//==============================================================================
AudioProcessorEditor* JuceDemoPluginAudioProcessor::createEditor()
{
    return new JuceDemoPluginAudioProcessorEditor (*this);
}

//==============================================================================
void JuceDemoPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");

    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);

    // Store the values of all our parameters, using their param ID as the XML attribute
    for (int i = 0; i < getNumParameters(); ++i)
        if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
            xml.setAttribute (p->paramID, p->getValue());

    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void JuceDemoPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our last window size..
            lastUIWidth  = jmax (xmlState->getIntAttribute ("uiWidth", lastUIWidth), 400);
            lastUIHeight = jmax (xmlState->getIntAttribute ("uiHeight", lastUIHeight), 200);

            // Now reload our parameters..
            for (int i = 0; i < getNumParameters(); ++i)
                if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
                    p->setValue ((float) xmlState->getDoubleAttribute (p->paramID, p->getValue()));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuceDemoPluginAudioProcessor();
}

ParameterHolder::ParameterHolder()
{
	parameters[VOLUME_A_PARAM].store(0.05);
	parameters[VOLUME_D_PARAM].store(0.02);
	parameters[VOLUME_S_PARAM].store(0.5);
	parameters[VOLUME_R_PARAM].store(0.1);
	parameters[FILTER_A_PARAM].store(0.05);
	parameters[FILTER_D_PARAM].store(0.02);
	parameters[FILTER_S_PARAM].store(0.5);
	parameters[FILTER_R_PARAM].store(0.1);
	parameters[REVERB_MIX_PARAM].store(0.0);
	parameters[REVERB_LEN_PARAM].store(0.5);
	parameters[DELAY_MIX_PARAM].store(0.0);
	parameters[DELAY_FEEDBACK_PARAM].store(0.5);
	parameters[DELAY_LEN_LABEL].store(0.4);
	parameters[CHORUS_MIX_PARAM].store(0.0);
	parameters[CHORUS_MODAMT_PARAM].store(0.1);
	parameters[CHORUS_MOD_SPEED_PARAM].store(4);
	parameters[LFO_RATE_PARAM].store(0.0);
	parameters[LFO_AMP_PARAM].store(0.0);
	parameters[FILTER_START_PARAM].store(5000.0);
	parameters[FILTER_END_PARAM].store(1.0);
}
