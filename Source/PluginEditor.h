/*
  ==============================================================================


  ==============================================================================
*/

#pragma once

#ifdef __APPLE__ 

#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"

#elif defined _WIN32 || defined _WIN64

#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"

#endif



class OtherLookAndFeel : public LookAndFeel_V4
{
public:
	OtherLookAndFeel();
	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
};

//==============================================================================
/** This is the editor component that our filter will display.
*/
class JuceDemoPluginAudioProcessorEditor  : public AudioProcessorEditor,
                                            private Timer,
											private Slider::Listener,
                                            private ComboBox::Listener
{
public:
    JuceDemoPluginAudioProcessorEditor (JuceDemoPluginAudioProcessor&);
    ~JuceDemoPluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:

    MidiKeyboardComponent midiKeyboard;

    //==============================================================================
    JuceDemoPluginAudioProcessor& getProcessor() const
    {
        return static_cast<JuceDemoPluginAudioProcessor&> (processor);
    }
    void sliderValueChanged (Slider* slider) override;
    void comboBoxChanged (ComboBox* comboBox) override;
	void setupSliders();
    
	std::map<int, std::string> labelText =
	{
		{ VOLUME_ADSR_LABEL, "Envelope \n(volume)" },{ FILTER_ADSR_LABEL, "Envelope \n(Filter)" },
		{ REVERB_LEN_LABEL, "Verb Size" },{ REVERB_MIX_LABEL, "Verb Mix" },{ DELAY_LEN_LABEL, "Length" },
		{ DELAY_FEEDBACK_LABEL, "Feedback" },{ DELAY_MIX_LABEL, "Mix" },{ CHORUS_MODAMT_LABEL, "Chorus Amount" },
		{ CHORUS_MOD_SPEED_LABEL, "chorus speed" },{ CHORUS_MIX_LABEL, "chorus Mix" }, { LFO_RATE_LABEL, "LFO rate" },
		{ LFO_AMP_LABEL, "LFO amp" },{ FILTER_START_FREQ, "Filter mod1" },{ FILTER_END_FREQ, "Filter mod2" }
	};

	std::array<Slider, NUM_VERT_DELAY> vertSlidersDelay;
	std::array<Slider, NUM_VERT_AMP> vertSlidersAmp;
	std::array<Slider, NUM_POT_SLIDERS> potSliders;
	std::array<ComboBox, NUM_COMBO_BOX> comboBoxes;
	std::array<Label, NUM_LABELS> labels;

    LookAndFeel_V4 colourSchemeOne;
	Label nameLabel;
	OtherLookAndFeel otherLookAndFeel;
};
