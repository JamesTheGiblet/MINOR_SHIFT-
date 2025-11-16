# 🎵 Hit Analyzer Web Tool

**The bridge between MINOR_SHIFT theory and real-world audio.**

## 🚀 Overview

The Hit Analyzer is a web-based tool that uses the [Meyda.js](https://meyda.js.org/) audio feature extraction library to analyze an audio file in real-time. It extracts key musical features and scores them against the proven patterns identified in the MINOR_SHIFT research, providing a practical "Hit Potential" score.

This moves beyond theoretical analysis (e.g., checking a known chord progression) to practical analysis of a final audio mix.

## ✨ Features

- **Real-Time Analysis**: Extracts features directly from an audio file in the browser.
- **Tempo (BPM) Detection**: Compares the song's tempo to the optimal 95-118 BPM range.
- **Dynamic Range Analysis**: Uses RMS (Root Mean Square) to measure loudness variation between song sections, rewarding dynamic builds.
- **Spectral Analysis**: Uses features like `spectralCentroid` to analyze the "brightness" or "darkness" of the track, which correlates to emotional tone.
- **Emotional Arc Scoring**: By analyzing features over time, it can map the song's energy curve to the ideal "verse-chorus-verse" dynamic lift.

## 🛠️ How It Works

The `HitAnalyzer` class takes an audio buffer, creates a Meyda analyzer, and extracts a set of features. These raw features are then passed to a scoring module that translates them into a `minorShiftScore`.

### Example Usage

```javascript
import { HitAnalyzer } from './hit-analyzer.js';

const audioFile = document.getElementById('audio-upload').files;
const audioContext = new AudioContext();
const fileReader = new FileReader();

fileReader.onload = async (event) => {
  const audioBuffer = await audioContext.decodeAudioData(event.target.result);
  
  const analyzer = new HitAnalyzer(audioContext, audioBuffer);
  const analysis = await analyzer.analyze();

  console.log(analysis);
  // {
  //   bpm: 115,
  //   dynamics: { verseRms: 0.4, chorusRms: 0.8, dynamicScore: 90 },
  //   spectral: { averageCentroid: 1500, brightnessScore: 85 },
  //   minorShiftScore: 88
  // }
};

fileReader.readAsArrayBuffer(audioFile);
```

### Scoring Integration

The extracted features are scored based on the MINOR_SHIFT evidence base:

| Feature | Ideal Range | Scoring Logic |
|---|---|---|
| **BPM** | 95-118 | 100 points if inside range, proportionally less outside. |
| **Dynamics** | Chorus RMS > Verse RMS | Points awarded for a clear "lift" in energy. |
| **Brightness** | Varies by genre | Compares spectral centroid to genre benchmarks. |

## 🔮 Future Enhancements

- **Chord Detection**: Integrate a more advanced model to detect the actual harmonic progression.
- **Vocal Analysis**: Isolate the vocal track to analyze lyrical delivery and emotional intensity.
- **Structural Analysis**: Automatically identify intro, verse, and chorus sections based on dynamic and spectral changes.

This tool provides a powerful feedback loop for producers, allowing them to test their mixes against data-driven models of success.
