# MINOR_SHIFT VST/DAW Plugin

## Overview

This folder is for the development of a VST/AU/DAW plugin implementing the MINOR_SHIFT algorithm.

## Recommended Framework

- [JUCE](https://juce.com/) (C++)
- [HISE](https://github.com/christophhart/HISE) (open-source)

## Features

- Generate MINOR_SHIFT progressions
- Analyze tracks for MINOR_SHIFT features
- Provide recommendations for hit potential

## Getting Started

1. Install JUCE and set up a new audio plugin project.
2. Implement core logic from the MINOR_SHIFT JavaScript/algorithm.
3. Add UI for progression generation and analysis.

## Roadmap

### MVP (Minimum Viable Product)

- Basic UI for chord progression generation (i–VII–VI in selectable keys)
- Simple MIDI output of generated progression
- Audio playback of progression using built-in synth

### Feature Expansion

- Real-time analysis of imported audio/MIDI for MINOR_SHIFT features
- Hit potential scoring based on progression and tempo
- Preset management and user-defined progressions
- Export to DAW as MIDI or audio

### Testing

- Unit tests for core logic (progression generation, analysis)
- User testing in major DAWs (Ableton, Logic, FL Studio, etc.)
- Cross-platform compatibility (Windows, macOS)

### Release

- Prepare documentation and demo videos
- Package as VST3/AU plugin
- Publish on GitHub and plugin marketplaces

## Contribution Guidelines

We welcome contributions! To get started:

1. Fork the repository and create a new branch for your feature or fix.
2. Follow the code style and structure of the existing project.
3. Write clear commit messages and document your changes.
4. Test your code in at least one major DAW (Ableton, Logic, FL Studio, etc.).
5. Submit a pull request with a description of your changes and testing steps.

For major features or design changes, please open an issue first to discuss your ideas.

## Changelog

### 2025-11-16

- Initial project scaffolding
- Added Roadmap, Contribution Guidelines, and Changelog sections

## Resources

- JUCE Docs: <https://docs.juce.com/>
- Example plugins: <https://github.com/juce-framework/JUCE/tree/master/examples>
