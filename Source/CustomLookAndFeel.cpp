/*
==============================================================================

CustomLookAndFeel.cpp
Created: 18 Jul 2017 5:18:10pm
Author:  wrattchri1

==============================================================================
*/

/*
==============================================================================

CustomLookAndFeel.h
Created: 18 Jul 2017 5:18:10pm
Author:  wrattchri1

==============================================================================
*/

#pragma once

#include "PluginEditor.h"


OtherLookAndFeel::OtherLookAndFeel()
{
	setColour(Slider::thumbColourId, Colours::hotpink);
}
void OtherLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
	const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
	const float radius = jmin(width / 2, height / 2) - 4.0f;
	const float centreX = x + width * 0.5;
	const float centreY = y + height * 0.5f;
	const float rx = centreX - radius;
	const float ry = centreY - radius;
	const float rw = radius * 2.0f;
	const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

	//fill
	g.setColour(Colours::orange);
	g.fillEllipse(rx, ry, rw, rw);

	//outline
	g.setColour(Colours::red);
	g.drawEllipse(rx, ry, rw, rw, 1.0f);

	Path p;
	const float pointerLength = radius * 0.33f;
	const float pointerThickness = 2.0f;
	p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
	p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

	g.setColour(Colours::yellow);
	g.fillPath(p);
}

//setLookAndFeel(&otherLookAndFeel);
//
////dial1.setColour(Slider::thumbColourId, Colours::green);
////dial1.setLookAndFeel(&otherLookAndFeel);
//private:
//Slider dial1;
//Slider dial2;
//TextButton button1;
//TextButton button2;
//OtherLookAndFeel otherLookAndFeel;