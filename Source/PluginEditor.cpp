/*
  ==============================================================================



  ==============================================================================
*/

#ifdef __APPLE__ 

#include "PluginProcessor.h"
#include "PluginEditor.h"

#elif defined _WIN32 || defined _WIN64

#include "PluginProcessor.h"
#include "PluginEditor.h"

#endif

//==============================================================================
JuceDemoPluginAudioProcessorEditor::JuceDemoPluginAudioProcessorEditor (JuceDemoPluginAudioProcessor& owner)
    : AudioProcessorEditor (owner),
      midiKeyboard (owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
	//setLookAndFeel(&otherLookAndFeel);
    // add the midi keyboard component..
    addAndMakeVisible (midiKeyboard);

	const int readBoxHeight = 15;
	const int readBoxWidth = 50;
	Colour labelColour = Colour(1.0f, 0.0f, 0.0f);
	Font labelFont = Font("Times New Roman", 20.0f, Font::italic);

	setupSliders();

	//combo boxes
	comboBoxes[OSCIL_TYPE].addItem("Sine Osc", 1);
	comboBoxes[OSCIL_TYPE].addItem("Square Osc", 2);
	comboBoxes[OSCIL_TYPE].addItem("Saw Osc", 3);
	comboBoxes[OSCIL_TYPE].addItem("Noise", 4);

	comboBoxes[OSCIL_TYPE].setSelectedId(3, false);

	comboBoxes[LFO_TYPE].addItem("Amplitude", 1);
	//comboBoxes[LFO_TYPE].addItem("Frequency", 2);
	comboBoxes[LFO_TYPE].setSelectedId(1, false);

	for (int i = 0; i < NUM_COMBO_BOX; i++)
	{
		addAndMakeVisible(&comboBoxes[i]);
		comboBoxes[i].addListener(this);
	}

	//labels
	for (int i = 0; i < NUM_LABELS; i++)
	{
		labels[i].setText(labelText.at(i), dontSendNotification);
		labels[i].setJustificationType(Justification::centred);
		labels[i].setFont(labelFont);
		labels[i].setColour(juce::Label::textColourId, labelColour);
		addAndMakeVisible(&labels[i]);
	}

	//Name and oscil type
	nameLabel.setText("GENERATOR \nCreated by Chris Wratt", dontSendNotification);
	nameLabel.setJustificationType(Justification::centred);
	nameLabel.setColour(juce::Label::textColourId, labelColour);
	nameLabel.setFont(labelFont);
	addAndMakeVisible(&nameLabel);

    // set resize limits for this plug-in
    setResizeLimits (700, 400, 1000, 600);

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize (owner.lastUIWidth,
             owner.lastUIHeight);
}

void JuceDemoPluginAudioProcessorEditor::setupSliders()
{
	for (int i = 0; i < NUM_VERT_DELAY; i++)
	{
		vertSlidersDelay[i].setSliderStyle(Slider::LinearVertical);
		vertSlidersDelay[i].setRange(0.0, 1000.0, 1.0);
		vertSlidersDelay[i].setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		vertSlidersDelay[i].setPopupDisplayEnabled(true, this);
		vertSlidersDelay[i].setTextValueSuffix(" MS");
		vertSlidersDelay[i].addListener(this);
		addAndMakeVisible(&vertSlidersDelay[i]);
	}

	vertSlidersDelay[VOLUME_A_SLIDER].setValue(volumeDefaultA);
	vertSlidersDelay[VOLUME_D_SLIDER].setValue(volumeDefaultD);
	vertSlidersDelay[VOLUME_R_SLIDER].setValue(volumeDefaultR);

	vertSlidersDelay[FILTER_A_SLIDER].setValue(filterDefaultA);
	vertSlidersDelay[FILTER_D_SLIDER].setValue(filterDefaultD);
	vertSlidersDelay[FILTER_R_SLIDER].setValue(filterDefaultR);

	for (int i = 0; i < NUM_VERT_AMP; i++)
	{
		vertSlidersAmp[i].setSliderStyle(Slider::LinearVertical);
		vertSlidersAmp[i].setRange(0.0, 1.0, 0.001);
		vertSlidersAmp[i].setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		vertSlidersAmp[i].setPopupDisplayEnabled(true, this);
		vertSlidersAmp[i].setTextValueSuffix(" Amp");
		vertSlidersAmp[i].addListener(this);
		addAndMakeVisible(&vertSlidersAmp[i]);
	}

	vertSlidersAmp[VOLUME_S_SLIDER].setValue(volumeDefaultS);
	vertSlidersAmp[FILTER_S_SLIDER].setValue(filterDefaultS);

	for (int i = 0; i < NUM_POT_SLIDERS; i++)
	{
		potSliders[i].setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		potSliders[i].setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		potSliders[i].setPopupDisplayEnabled(true, this);
		potSliders[i].setTextValueSuffix(" Mix");
		potSliders[i].addListener(this);
		addAndMakeVisible(&potSliders[i]);
	}

	potSliders[REVERB_MIX_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[REVERB_MIX_SLIDER].setValue(0.2);
	potSliders[REVERB_LEN_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[REVERB_LEN_SLIDER].setValue(0.5);
	potSliders[DELAY_MIX_SLIDER].setRange(0.0, 1.0, 0.001);			potSliders[DELAY_MIX_SLIDER].setValue(0.0);
	potSliders[DELAY_LEN_SLIDER].setRange(0.0, 3.0, 0.001);			potSliders[DELAY_LEN_SLIDER].setValue(0.4);
	potSliders[DELAY_FEEDBACK_SLIDER].setRange(0.0, 1.0, 0.001);	potSliders[DELAY_FEEDBACK_SLIDER].setValue(0.5);
	potSliders[CHORUS_MIX_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[CHORUS_MIX_SLIDER].setValue(0.1);
	potSliders[CHORUS_MODAMT_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[CHORUS_MODAMT_SLIDER].setValue(0.01);
	potSliders[CHORUS_MOD_SPEED_SLIDER].setRange(0.0, 30.0, 0.001); potSliders[CHORUS_MOD_SPEED_SLIDER].setValue(0.1);
	potSliders[LFO_RATE_SLIDER].setRange(0.0, 100.0, 0.001);		potSliders[LFO_RATE_SLIDER].setValue(20.0);
	potSliders[LFO_RATE_SLIDER].setTextValueSuffix(" Hz");			potSliders[LFO_RATE_SLIDER].setSkewFactor(0.5);
	potSliders[LFO_AMP_SLIDER].setRange(0.0, 1.0, 0.001);			potSliders[LFO_AMP_SLIDER].setValue(0.1);
	potSliders[LFO_AMP_SLIDER].setTextValueSuffix("");
	potSliders[FILTER_START_SLIDER].setRange(20.0, 20000.0, 1.0);	potSliders[FILTER_START_SLIDER].setValue(filterStartDefault);
	potSliders[FILTER_START_SLIDER].setTextValueSuffix(" Hz");
	potSliders[FILTER_END_SLIDER].setRange(20.0, 20000.0, 1.0);		potSliders[FILTER_END_SLIDER].setValue(filterEndDefault);
	potSliders[FILTER_END_SLIDER].setTextValueSuffix(" Hz");

}

JuceDemoPluginAudioProcessorEditor::~JuceDemoPluginAudioProcessorEditor()
{
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::paint (Graphics& g)
{
	const Colour colourOne = Colours::lightblue;
	const Colour colourTwo = Colours::white;
	Rectangle<int> area = getLocalBounds();

	Rectangle<int> top = area.removeFromTop(area.getHeight() / 2);
	Rectangle<int> topLeft = top.removeFromLeft(top.getWidth() / 3);
	Rectangle<int> topMid = top.removeFromLeft(top.getWidth() / 2);
	Rectangle<int> topRight = top;

	Rectangle<int> bottomLeft = area.removeFromLeft(area.getWidth() / 3);
	Rectangle<int> bottomMid = area.removeFromLeft(area.getWidth() / 2);
	Rectangle<int> bottomRight = area;

	g.setColour(colourOne);
	g.fillRect(topLeft);
	g.setColour(colourTwo);
	g.fillRect(topMid);
	g.setColour(colourOne);
	g.fillRect(topRight);

	g.setColour(colourTwo);
	g.fillRect(bottomLeft);
	g.setColour(colourOne);
	g.fillRect(bottomMid);
	g.setColour(colourTwo);
	g.fillRect(bottomRight);
}

void JuceDemoPluginAudioProcessorEditor::resized()
{
	const int labelHeightRatio = 5;
	const float comboBoxRatio = 3.0;
	const int comboBoxMargin = 4;

	//general dividing into rects
	Rectangle<int> fullArea(getLocalBounds());
	Rectangle<int> halfAreaTop = fullArea.removeFromTop(fullArea.getHeight() / 2);
	{
		Rectangle<int> topLeftThirdRect = halfAreaTop.removeFromLeft(halfAreaTop.getWidth() / 3);
		nameLabel.setBounds(topLeftThirdRect.removeFromTop(topLeftThirdRect.getHeight() / 2));
		comboBoxes[OSCIL_TYPE].setBounds(topLeftThirdRect.removeFromTop(topLeftThirdRect.getHeight() / comboBoxRatio).reduced(comboBoxMargin));
	}

	//ADSR
	Rectangle<int> topMiddleRect = halfAreaTop.removeFromLeft(halfAreaTop.getWidth() / 2);
	{
		labels[VOLUME_ADSR_LABEL].setBounds(topMiddleRect.removeFromTop(topMiddleRect.getHeight() / labelHeightRatio));
		vertSlidersDelay[VOLUME_A_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth() / 4));
		vertSlidersDelay[VOLUME_D_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth() / 3));
		vertSlidersAmp[VOLUME_S_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth() / 2));
		vertSlidersDelay[VOLUME_R_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth()));
	}

	//LFO
	Rectangle<int> topRightRect = halfAreaTop;
	{
		Rectangle<int> top = topRightRect.removeFromTop(topRightRect.getHeight() / 2);
		Rectangle<int> topBottom = top.removeFromBottom(top.getHeight() / 2);
		Rectangle<int> placeHolderRect1 = top.removeFromTop(top.getHeight() / (comboBoxRatio / 2));
		comboBoxes[LFO_TYPE].setBounds(topBottom.removeFromTop(topBottom.getHeight() / (comboBoxRatio / 2)).reduced(comboBoxMargin));
		Rectangle<int> bottomLeft = topRightRect.removeFromLeft(topRightRect.getWidth() / 2);
		labels[LFO_RATE_LABEL].setBounds(bottomLeft.removeFromTop(bottomLeft.getHeight() / labelHeightRatio));
		potSliders[LFO_RATE_SLIDER].setBounds(bottomLeft);
		labels[LFO_AMP_LABEL].setBounds(topRightRect.removeFromTop(topRightRect.getHeight() / labelHeightRatio));
		potSliders[LFO_AMP_SLIDER].setBounds(topRightRect.removeFromLeft(topRightRect.getWidth()));
	}

	//Filter
	Rectangle<int> bottomLeftRect = fullArea.removeFromLeft(fullArea.getWidth() / 3);
	{
		Rectangle<int> top = bottomLeftRect.removeFromTop(bottomLeftRect.getHeight() / 2);
		Rectangle<int> placeHolderRect2 = top.removeFromTop(top.getHeight() / comboBoxRatio);
		Rectangle<int> bottomLeft = bottomLeftRect.removeFromLeft(bottomLeftRect.getWidth() / 2);
		labels[FILTER_START_FREQ].setBounds(bottomLeft.removeFromTop(bottomLeft.getHeight() / labelHeightRatio));
		potSliders[FILTER_START_SLIDER].setBounds(bottomLeft);
		labels[FILTER_END_FREQ].setBounds(bottomLeftRect.removeFromTop(bottomLeftRect.getHeight() / labelHeightRatio));
		potSliders[FILTER_END_SLIDER].setBounds(bottomLeftRect);
	}

	//Filter ADSR
	Rectangle<int> bottompMiddleThirdRect = fullArea.removeFromLeft(fullArea.getWidth() / 2);
	{
		labels[FILTER_ADSR_LABEL].setBounds(bottompMiddleThirdRect.removeFromTop(bottompMiddleThirdRect.getHeight() / labelHeightRatio));
		vertSlidersDelay[FILTER_A_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth() / 4));
		vertSlidersDelay[FILTER_D_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth() / 3));
		vertSlidersAmp[FILTER_S_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth() / 2));
		vertSlidersDelay[FILTER_R_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth()));
	}

	//Effects
	Rectangle<int> bottomRightRect = fullArea;
	{
		Rectangle<int> topThird = bottomRightRect.removeFromTop(bottomRightRect.getHeight() / 3);
		Rectangle<int> topThirdLeft = topThird.removeFromLeft(topThird.getWidth() / 3);
		Rectangle<int> topThirdMid = topThird.removeFromLeft(topThird.getWidth() / 2);
		Rectangle<int> topThirdRight = topThird;
		Rectangle<int> middleThird = bottomRightRect.removeFromTop(bottomRightRect.getHeight() / 2);
		Rectangle<int> midThirdLeft = middleThird.removeFromLeft(middleThird.getWidth() / 3);
		Rectangle<int> midThirdMid = middleThird.removeFromLeft(middleThird.getWidth() / 2);
		Rectangle<int> midThirdRight = middleThird;
		Rectangle<int> bottomThird = bottomRightRect;
		Rectangle<int> bottomThirdLeft = bottomThird.removeFromLeft(bottomThird.getWidth() / 3);
		Rectangle<int> bottomThirdMid = bottomThird.removeFromLeft(bottomThird.getWidth() / 2);
		Rectangle<int> bottomThirdRight = bottomThird;

		labels[REVERB_LEN_LABEL].setBounds(topThirdLeft.removeFromTop(topThirdLeft.getHeight() / labelHeightRatio));
		labels[REVERB_MIX_LABEL].setBounds(topThirdRight.removeFromTop(topThirdRight.getHeight() / labelHeightRatio));
		potSliders[REVERB_LEN_SLIDER].setBounds(topThirdLeft);
		potSliders[REVERB_MIX_SLIDER].setBounds(topThirdRight);

		//labels[DELAY_LEN_LABEL].setBounds(midThirdLeft.removeFromTop(midThirdLeft.getHeight() / labelHeightRatio));
		//labels[DELAY_FEEDBACK_LABEL].setBounds(midThirdMid.removeFromTop(midThirdMid.getHeight() / labelHeightRatio));
		//labels[DELAY_MIX_LABEL].setBounds(midThirdRight.removeFromTop(midThirdRight.getHeight() / labelHeightRatio));
		//potSliders[DELAY_LEN_SLIDER].setBounds(midThirdLeft);
		//potSliders[DELAY_FEEDBACK_SLIDER].setBounds(midThirdMid);
		//potSliders[DELAY_MIX_SLIDER].setBounds(midThirdRight);

		labels[CHORUS_MODAMT_LABEL].setBounds(bottomThirdLeft.removeFromTop(bottomThirdLeft.getHeight() / labelHeightRatio));
		labels[CHORUS_MOD_SPEED_LABEL].setBounds(bottomThirdMid.removeFromTop(bottomThirdMid.getHeight() / labelHeightRatio));
		labels[CHORUS_MIX_LABEL].setBounds(bottomThirdRight.removeFromTop(bottomThirdRight.getHeight() / labelHeightRatio));
		potSliders[CHORUS_MODAMT_SLIDER].setBounds(bottomThirdLeft);
		potSliders[CHORUS_MOD_SPEED_SLIDER].setBounds(bottomThirdMid);
		potSliders[CHORUS_MIX_SLIDER].setBounds(bottomThirdRight);
	}
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::timerCallback()
{
}

void JuceDemoPluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
		if (slider == &vertSlidersDelay[VOLUME_A_SLIDER]) ParameterHolder::inst().parameters[VOLUME_A_PARAM].store(slider->getValue() / 1000.0); 
		else if (slider == &vertSlidersDelay[VOLUME_D_SLIDER]) ParameterHolder::inst().parameters[VOLUME_D_PARAM].store(slider->getValue() / 1000.0);
		else if (slider == &vertSlidersAmp[VOLUME_S_SLIDER]) ParameterHolder::inst().parameters[VOLUME_S_PARAM].store(slider->getValue());
		else if (slider == &vertSlidersDelay[VOLUME_R_SLIDER]) ParameterHolder::inst().parameters[VOLUME_R_PARAM].store(slider->getValue() / 1000.0);
		else if (slider == &vertSlidersDelay[FILTER_A_SLIDER]) ParameterHolder::inst().parameters[FILTER_A_PARAM].store(slider->getValue() / 1000.0);
		else if (slider == &vertSlidersDelay[FILTER_D_SLIDER]) ParameterHolder::inst().parameters[FILTER_D_PARAM].store(slider->getValue() / 1000.0);
		else if (slider == &vertSlidersAmp[FILTER_S_SLIDER]) ParameterHolder::inst().parameters[FILTER_S_PARAM].store(slider->getValue());
		else if (slider == &vertSlidersDelay[FILTER_R_SLIDER]) ParameterHolder::inst().parameters[FILTER_R_PARAM].store(slider->getValue() / 1000.0);
		else if (slider == &potSliders[REVERB_MIX_SLIDER]) ParameterHolder::inst().parameters[REVERB_MIX_PARAM].store(slider->getValue());
		else if (slider == &potSliders[REVERB_LEN_SLIDER]) ParameterHolder::inst().parameters[REVERB_LEN_PARAM].store(slider->getValue());
		else if (slider == &potSliders[DELAY_MIX_SLIDER]) ParameterHolder::inst().parameters[DELAY_MIX_PARAM].store(slider->getValue());
		else if (slider == &potSliders[DELAY_LEN_SLIDER]) ParameterHolder::inst().parameters[DELAY_LEN_PARAM].store(slider->getValue());
		else if (slider == &potSliders[DELAY_FEEDBACK_LABEL]) ParameterHolder::inst().parameters[DELAY_FEEDBACK_PARAM].store(slider->getValue());
		else if (slider == &potSliders[CHORUS_MIX_SLIDER]) ParameterHolder::inst().parameters[CHORUS_MIX_PARAM].store(slider->getValue());
		else if (slider == &potSliders[CHORUS_MODAMT_SLIDER]) ParameterHolder::inst().parameters[CHORUS_MODAMT_PARAM].store(slider->getValue());
		else if (slider == &potSliders[CHORUS_MOD_SPEED_SLIDER]) ParameterHolder::inst().parameters[CHORUS_MOD_SPEED_PARAM].store(slider->getValue());
		else if (slider == &potSliders[LFO_RATE_SLIDER]) ParameterHolder::inst().parameters[LFO_RATE_PARAM].store(slider->getValue());
		else if (slider == &potSliders[LFO_AMP_SLIDER]) ParameterHolder::inst().parameters[LFO_AMP_PARAM].store(slider->getValue());
		else if (slider == &potSliders[FILTER_START_SLIDER]) ParameterHolder::inst().parameters[FILTER_START_PARAM].store(slider->getValue());
		else if (slider == &potSliders[FILTER_END_SLIDER]) ParameterHolder::inst().parameters[FILTER_END_PARAM].store(slider->getValue());
}

void JuceDemoPluginAudioProcessorEditor::comboBoxChanged (ComboBox* comboBox)
{
	if (comboBox == &comboBoxes[OSCIL_TYPE])
	{

		switch (comboBox->getSelectedItemIndex())
		{
		case(OSC_SINE):
			ParameterHolder::inst().currentOsc = OSC_SINE;
			break;
		case(OSC_SQUARE):
			ParameterHolder::inst().currentOsc = OSC_SQUARE;
			break;
		case(OSC_SAW):
			ParameterHolder::inst().currentOsc = OSC_SAW;
			break;
		case(OSC_NOISE):
			ParameterHolder::inst().currentOsc = OSC_NOISE;
			break;
		}
	}
}
