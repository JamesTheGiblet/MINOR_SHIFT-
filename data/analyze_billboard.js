// Analyze and visualize Billboard 2010-2018 data
// Usage: node analyze_billboard.js [filename]

const fs = require('fs');
const path = require('path');

function loadData(fileName) {
  const dataPath = path.join(__dirname, fileName);
  if (!fs.existsSync(dataPath)) {
    console.error(`Error: File not found at ${dataPath}`);
    process.exit(1);
  }
  const raw = fs.readFileSync(dataPath, 'utf-8');
  const data = JSON.parse(raw);

  // Adapt to different JSON structures
  if (data.songs && !data.years) {
    // Handle flat structure like billboard_2010_2024.json
    const songsByYear = data.songs.reduce((acc, song) => {
      (acc[song.year] = acc[song.year] || []).push(song);
      return acc;
    }, {});
    return Object.keys(songsByYear).sort().map(year => ({ year: parseInt(year), songs: songsByYear[year] }));
  } else if (data.years) {
    // Handle nested structure like billboard_2010_2018_template.json
    return data.years;
  }
  return [];
}

const noteMap = {
    'C': 0, 'C#': 1, 'Db': 1, 'D': 2, 'D#': 3, 'Eb': 3, 'E': 4, 'F': 5, 'F#': 6, 'Gb': 6, 'G': 7, 'G#': 8, 'Ab': 8, 'A': 9, 'A#': 10, 'Bb': 10, 'B': 11
};

const romanNumerals = ['I', 'bII', 'II', 'bIII', 'III', 'IV', 'bV', 'V', 'bVI', 'VI', 'bVII', 'VII'];

function getRomanNumeral(chord, key) {
    const keyRootNote = key.replace('m', '');
    const isMinorKey = key.endsWith('m');

    const keyRootValue = noteMap[keyRootNote];
    const chordRootValue = noteMap[chord.replace('m', '')];

    if (keyRootValue === undefined || chordRootValue === undefined) return '?';

    let interval = (chordRootValue - keyRootValue + 12) % 12;

    let numeral = romanNumerals[interval];
    const isMinorChord = chord.endsWith('m');

    if (isMinorKey) {
        // Adjust for minor key context
        const minorScaleIntervals = { 0: 'i', 3: 'III', 5: 'iv', 7: 'v', 8: 'VI', 10: 'VII' };
        numeral = minorScaleIntervals[interval] || numeral;
    } else {
        // Adjust for major key context
        const majorScaleIntervals = { 0: 'I', 2: 'ii', 4: 'iii', 5: 'IV', 7: 'V', 9: 'vi' };
        numeral = majorScaleIntervals[interval] || numeral;
    }

    // Ensure chord quality (major/minor) is reflected in the numeral case
    if (isMinorChord) {
        // It's a minor chord, so the numeral should be lowercase.
        return numeral.toLowerCase();
    } else if (numeral.length > 0 && numeral.toLowerCase() === numeral) {
        // It's a major chord, but the diatonic numeral is minor (e.g. 'ii' or 'iii'). Uppercase it.
        return numeral.toUpperCase();
    }
    return numeral;
}

function convertProgressionToRoman(progression, key) {
    if (!Array.isArray(progression) || !key) return '';
    return progression.map(chord => getRomanNumeral(chord, key)).join('-');
}

function countProgressions(data, targetProgressions) {
  const yearStats = [];
  let totalSongs = 0;
  let totalTarget = 0;
  for (const yearObj of data) {
    let yearTotal = 0;
    let yearTarget = 0;
    for (const song of yearObj.songs) {
      yearTotal++;
      let prog = '';
      if (song.chord_progression) {
        // Handle old format (string)
        prog = song.chord_progression.replace(/\s+/g, '').toLowerCase();
      } else if (song.progression && song.key) {
        // Handle new format (array), convert to Roman numerals
        prog = convertProgressionToRoman(song.progression, song.key).toLowerCase();
      }
      if (targetProgressions.some(tp => prog.startsWith(tp.replace(/\s+/g, '').toLowerCase()))) {
        yearTarget++;
      }
    }
    yearStats.push({
      year: yearObj.year,
      total: yearTotal,
      target: yearTarget,
      percent: yearTotal ? (100 * yearTarget / yearTotal).toFixed(1) : '0.0'
    });
    totalSongs += yearTotal;
    totalTarget += yearTarget;
  }
  return { yearStats, totalSongs, totalTarget, percent: totalSongs ? (100 * totalTarget / totalSongs).toFixed(1) : '0.0' };
}

function printSummary(stats, label) {
  console.log(`\n=== ${label} ===`);
  for (const y of stats.yearStats) {
    console.log(`${y.year}: ${y.target}/${y.total} (${y.percent}%)`);
  }
  console.log(`Total: ${stats.totalTarget}/${stats.totalSongs} (${stats.percent}%)\n`);
}

function main() {
  // Default to the original file, but allow overriding via command line argument
  const fileName = process.argv[2] || 'billboard_2010_2018_template.json';
  console.log(`Analyzing data from: ${fileName}\n`);
  const data = loadData(fileName);

  // Define target progressions (edit as needed)
  const minorShiftProgressions = ['i-vii-vi', 'i-vii-vi-v', 'i-vi-iii-vii'];
  const popPunkProgressions = ['i-v-vi-iv', 'i-v-vi-iii-iv', 'i-v-vi-iv'];

  const minorShiftStats = countProgressions(data, minorShiftProgressions);
  const popPunkStats = countProgressions(data, popPunkProgressions);

  printSummary(minorShiftStats, 'MINOR_SHIFT Progressions');
  printSummary(popPunkStats, 'Pop Punk Progressions');

  // Simple ASCII bar chart for MINOR_SHIFT
  console.log('Yearly MINOR_SHIFT Prevalence:');
  for (const y of minorShiftStats.yearStats) {
    const bar = '#'.repeat(Math.round(y.percent));
    console.log(`${y.year}: ${bar} (${y.percent}%)`);
  }
}

main();
