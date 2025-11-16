# 🤝 Contributing to MINOR_SHIFT

First off, thank you for considering contributing to MINOR_SHIFT! Your help is invaluable in building the world's most comprehensive, data-driven music analysis tool. Every piece of data, bug report, or feature suggestion helps us get closer to decoding the DNA of hit music.

This document provides guidelines for contributing to the project. Please feel free to propose changes to this document in a pull request.

## Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## How Can I Contribute?

* [Reporting Bugs](#reporting-bugs)
* [Suggesting Enhancements](#suggesting-enhancements)
* [Submitting New Song Data](#submitting-new-song-data)
* [Pull Requests](#pull-requests)

---

## 📊 Submitting New Song Data

This is one of the most important ways to contribute. Our analysis relies on high-quality data. We are currently focused on populating the `data/billboard_2010_2024.json` file.

### Data Structure

Each song entry must follow this exact JSON structure. Please ensure all fields are present.

```json
{
  "year": 2023,
  "artist": "Artist Name",
  "title": "Song Title",
  "chartPosition": 1,
  "key": "Bm",
  "progression": ["Bm", "A", "G"],
  "bpm": 118,
  "themes": ["Theme 1", "Theme 2"],
  "genreFusion": ["Genre 1", "Genre 2"],
  "streamingData": {
    "spotifyStreams": 1600000000,
    "youtubeViews": 700000000,
    "saveRate": 0.12,
    "skipRate": 0.21
  }
}
```

### Field Guidelines

| Field | Type | Description | Data Source Examples |
|---|---|---|---|
| `year` | `Number` | The year the song reached #1 on the Billboard Hot 100. | Wikipedia, Billboard.com |
| `artist` | `String` | The primary artist credited. | Wikipedia, Spotify |
| `title` | `String` | The official title of the song. | Wikipedia, Spotify |
| `chartPosition` | `Number` | For this dataset, this should always be `1`. | N/A |
| `key` | `String` | The musical key of the song (e.g., "C#m", "Gm"). | Tunebat, SongKeyBPM |
| `progression` | `Array<String>` | The primary chord progression of the chorus. | Ultimate Guitar, Chordify |
| `bpm` | `Number` | Beats Per Minute. | Tunebat, Spotify |
| `themes` | `Array<String>` | Lyrical themes. Refer to `LYRIC ANALYSIS.md` for examples. | Your own analysis, Genius |
| `genreFusion` | `Array<String>` | The blended genres. Refer to `LYRIC ANALYSIS.md`. | Wikipedia, AllMusic |
| `streamingData` | `Object` | Contains streaming metrics. | |
| `spotifyStreams` | `Number` | Total streams on Spotify (visible on desktop client). | Spotify Desktop |
| `youtubeViews` | `Number` | Total views on the official music video. | YouTube |
| `saveRate` | `Number` | **(Optional)** Estimated save rate. Use `null` if unknown. | N/A |
| `skipRate` | `Number` | **(Optional)** Estimated skip rate. Use `null` if unknown. | N/A |

### How to Submit

1. **Fork the repository.**
2. **Create a new branch:** `git checkout -b feature/add-song-YYYY-title`.
3. **Add your new song object** to the `songs` array in `data/billboard_2010_2024.json`. Please add it in reverse chronological order (newest songs first).
4. **Validate your JSON** to ensure it is well-formed. You can use an online JSON validator.
5. **Commit your changes:** `git commit -m "feat(data): Add [Song Title] by [Artist]"`.
6. **Push to your branch:** `git push origin feature/add-song-YYYY-title`.
7. **Open a Pull Request.** In the description, please mention the sources you used for the data (e.g., "Key/BPM from Tunebat, stream counts from Spotify on YYYY-MM-DD").

---

## 🐛 Reporting Bugs

1. **Check existing issues** to see if the bug has already been reported.
2. If not, **create a new issue**.
3. Provide a **clear title and description**, including steps to reproduce the bug and the expected behavior.

## ✨ Suggesting Enhancements

1. **Check existing issues** to see if your enhancement has already been suggested.
2. If not, **create a new issue**.
3. Provide a **clear title and description** of the enhancement, explaining why it would be beneficial to the project.

## 🔃 Pull Requests

1. Fork the repo and create your branch from `main`.
2. If you've added code that should be tested, add tests.
3. Ensure the test suite passes (`npm test`).
4. Make sure your code lints.
5. Issue that pull request!

Your PR will be reviewed, and we may suggest some changes or improvements. We appreciate your patience and collaboration.

Thank you for making MINOR_SHIFT better!
