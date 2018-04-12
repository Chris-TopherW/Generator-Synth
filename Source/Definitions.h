/*
  ==============================================================================

    Definitions.h
    Created: 3 Jul 2017 4:01:12pm
    Author:  wrattchri1

	Current bugs: Sometimes voices don't take new parameters- note on events might not be triggering properly or I need to delete notes
	between plays...

  ==============================================================================
*/

#pragma once

#ifdef __APPLE__
#include "../JuceLibraryCode/JuceHeader.h"

#elif defined _WIN32 || defined _WIN64
#include "..\JuceLibraryCode\JuceHeader.h"

#endif

#include <map>
#include <atomic>
#include <array>

enum { OSC_SINE, OSC_SQUARE, OSC_SAW, OSC_NOISE};

enum {
	VOLUME_A_SLIDER, VOLUME_D_SLIDER, VOLUME_R_SLIDER,
	FILTER_A_SLIDER, FILTER_D_SLIDER, FILTER_R_SLIDER,
	NUM_VERT_DELAY
};

enum { VOLUME_S_SLIDER, FILTER_S_SLIDER, NUM_VERT_AMP };

enum {
	REVERB_LEN_SLIDER, REVERB_MIX_SLIDER, DELAY_LEN_SLIDER, DELAY_FEEDBACK_SLIDER, 
	DELAY_MIX_SLIDER, CHORUS_MODAMT_SLIDER, CHORUS_MOD_SPEED_SLIDER, CHORUS_MIX_SLIDER,
	LFO_RATE_SLIDER, LFO_AMP_SLIDER, FILTER_START_SLIDER, FILTER_END_SLIDER,
	NUM_POT_SLIDERS
};

enum {
	OSCIL_TYPE, LFO_TYPE, NUM_COMBO_BOX
};

enum {
	VOLUME_ADSR_LABEL, FILTER_ADSR_LABEL, REVERB_LEN_LABEL, REVERB_MIX_LABEL, 
	DELAY_MIX_LABEL, DELAY_FEEDBACK_LABEL, DELAY_LEN_LABEL, CHORUS_MODAMT_LABEL, 
	CHORUS_MOD_SPEED_LABEL, CHORUS_MIX_LABEL, LFO_RATE_LABEL, LFO_AMP_LABEL,
	FILTER_START_FREQ, FILTER_END_FREQ, NUM_LABELS
};

//all parameters
enum {
	VOLUME_A_PARAM, VOLUME_D_PARAM, VOLUME_R_PARAM,
	FILTER_A_PARAM, FILTER_D_PARAM, FILTER_R_PARAM, VOLUME_S_PARAM, FILTER_S_PARAM,
	REVERB_LEN_PARAM, REVERB_MIX_PARAM, DELAY_LEN_PARAM, DELAY_FEEDBACK_PARAM, 
	DELAY_MIX_PARAM, CHORUS_MODAMT_PARAM, CHORUS_MOD_SPEED_PARAM, CHORUS_MIX_PARAM,
	LFO_RATE_PARAM, LFO_AMP_PARAM, FILTER_START_PARAM, FILTER_END_PARAM, NUM_ALL_PARAMS
};

constexpr int PLUGIN_CHANNELS = 2;
constexpr int defaultNumVoices = 6;
constexpr int PARAM_UPDATE_SPEED = 100;

constexpr float volumeDefaultA = 204.0;
constexpr float volumeDefaultD = 331.0;
constexpr float volumeDefaultS = 0.6;
constexpr float volumeDefaultR = 500.0;

constexpr float filterDefaultA = 100.0;
constexpr float filterDefaultD = 500.0;
constexpr float filterDefaultS = 0.3;
constexpr float filterDefaultR = 300.0;

constexpr float filterStartDefault = 2000.0;
constexpr float filterEndDefault = 15000.0;

constexpr int defaultOsc = OSC_SAW;
