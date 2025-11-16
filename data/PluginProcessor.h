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
    // MINOR_SHIFT Core Logic Ported to C++
    //==============================================================================
    std::vector<std::string> generateProgression(const std::string& key)
    {
        // The progression map from the original JS algorithm
        static const std::map<std::string, std::vector<std::string>> progressionMap = {
            {"Bm",  {"Bm", "A", "G"}},
            {"C#m", {"C#m", "B", "A"}},
            {"F#m", {"F#m", "E", "D"}},
            {"Gm",  {"Gm", "F", "Eb"}},
            {"Dm",  {"Dm", "C", "Bb"}}
        };

        auto it = progressionMap.find(key);
        if (it != progressionMap.end())
        {
            return it->second;
        }

        // Return the default "Bm" progression if the key is not found
        return progressionMap.at("Bm");
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Minor_shiftAudioProcessor)
};