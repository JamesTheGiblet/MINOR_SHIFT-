/*
  ==============================================================================

    PluginEditor.cpp
    Created: 16 Nov 2025 8:45:04pm
    Author:  Gemini Code Assist

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Minor_shiftAudioProcessorEditor::Minor_shiftAudioProcessorEditor (Minor_shiftAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Add a label to show the plugin is working
    titleLabel.setText ("MINOR_SHIFT v2.0", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (24.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

Minor_shiftAudioProcessorEditor::~Minor_shiftAudioProcessorEditor()
{
}

//==============================================================================
void Minor_shiftAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    // You can add drawing code here to make your UI look nice!
}

void Minor_shiftAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    titleLabel.setBounds (getLocalBounds().reduced(20));
}