/*
  ==============================================================================

    PluginEditor.h
    Created: 16 Nov 2025 8:45:04pm
    Author:  Gemini Code Assist

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Minor_shiftAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Minor_shiftAudioProcessorEditor (Minor_shiftAudioProcessor&);
    ~Minor_shiftAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Minor_shiftAudioProcessor& audioProcessor;

    juce::Label titleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Minor_shiftAudioProcessorEditor)
};