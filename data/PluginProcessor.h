/*
  ==============================================================================

    PluginProcessor.h
    Created: 16 Nov 2025 8:45:04pm
    Author:  Gemini Code Assist

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <map>
#include <string>
#include <vector>

//==============================================================================
/**
*/
class Minor_shiftAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Minor_shiftAudioProcessor();
    ~Minor_shiftAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Add APVTS for parameter management
    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    // MINOR_SHIFT Core Logic Ported to C++
    //==============================================================================
    std::vector<int> getChordNotes(const juce::String& chordName, int octave);
    std::vector<std::vector<int>> generateProgression(const juce::String& key, int rootNote);

    // Helper function to create the layout of parameters
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Minor_shiftAudioProcessor)
};