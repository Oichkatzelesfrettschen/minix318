#!/usr/bin/env node
/**
 * run-jscpd.js
 *
 * A robust, highly-verbose wrapper around the jscpd CLI to detect copy/paste
 * clones across a mixed-legacy repo.  It validates inputs, applies sane defaults,
 * handles missing paths, streams logs, and persists detailed run information.
 *
 * Usage:
 *   ./run-jscpd.js [--min-tokens <n>] [--threshold <percent>]
 *                  [--reporters <list>] [--output <dir>]
 *                  [--ignore <globs>] [--verbose] [--debug]
 *                  [--help] [<targetDir>]
 */

'use strict';

const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

//-----------------------------------------------------------------------------
// Default configuration
//-----------------------------------------------------------------------------
let config = {
  targetDir: '.',                          // directory to scan
  minTokens: 50,                           // jscpd --min-tokens
  threshold: 100,                          // jscpd --threshold
  reporters: ['console','json'],           // jscpd --reporters
  outputDir: 'jscpd-report',               // jscpd --output
  ignore: ['**/node_modules/**','**/*.md','**/releasetools/**','**/build/**'],
  verbose: false,                          // jscpd --verbose
  debug: false,                            // extra debug logging
  logFile: 'jscpd-run.log',                // where to persist logs
};

//-----------------------------------------------------------------------------
// Simple logger: timestamps + level, to file and (optionally) console
//-----------------------------------------------------------------------------
function writeLog(level, msg) {
  const ts = new Date().toISOString();
  const line = `${ts} [${level.padEnd(5)}] ${msg}`;
  fs.appendFileSync(config.logFile, line + '\n', 'utf8');
  if (config.debug) console.log(line);
}

//-----------------------------------------------------------------------------
// Print usage and exit
//-----------------------------------------------------------------------------
function printUsage(exitCode=0) {
  console.log(`
Usage: ${path.basename(process.argv[1])} [options] [<targetDir>]

Options:
  --min-tokens <n>     Minimum token length (default: ${config.minTokens})
  --threshold <pct>    Similarity threshold percent (default: ${config.threshold})
  --reporters <r1,r2>  Comma-separated reporters (default: ${config.reporters.join(',')})
  --output <dir>       Report output directory (default: ${config.outputDir})
  --ignore <g1,g2>     Glob patterns to ignore
  --verbose            Pass --verbose to jscpd
  --debug              Enable wrapper debug logging
  --help               Show this help message
  <targetDir>          Directory to scan (defaults to repo root)
`);
  process.exit(exitCode);
}

//-----------------------------------------------------------------------------
// CLI argument parsing
//-----------------------------------------------------------------------------
(function parseArgs() {
  const args = process.argv.slice(2);
  let i = 0;
  while (i < args.length) {
    const a = args[i];
    switch (a) {
      case '--help':
        printUsage();
        break;
      case '--min-tokens':
        config.minTokens = Number(args[++i]); break;
      case '--threshold':
        config.threshold = Number(args[++i]); break;
      case '--reporters':
        config.reporters = args[++i].split(',').map(s => s.trim()); break;
      case '--output':
        config.outputDir = args[++i]; break;
      case '--ignore':
        config.ignore = args[++i].split(',').map(s => s.trim()); break;
      case '--verbose':
        config.verbose = true; break;
      case '--debug':
        config.debug = true; break;
      default:
        if (a.startsWith('-')) {
          console.error(`Unknown option: ${a}`);
          printUsage(1);
        }
        config.targetDir = a;
        break;
    }
    i++;
  }
})();

//-----------------------------------------------------------------------------
// Resolve & validate target directory
//-----------------------------------------------------------------------------
let resolvedTarget = path.resolve(process.cwd(), config.targetDir);
if (!fs.existsSync(resolvedTarget)) {
  writeLog('WARN', `Target '${config.targetDir}' not found at ${resolvedTarget}; falling back to repo root.`);
  resolvedTarget = process.cwd();
} else {
  writeLog('INFO', `Scanning directory: ${resolvedTarget}`);
}

//-----------------------------------------------------------------------------
// Build jscpd CLI arguments
//-----------------------------------------------------------------------------
const jscpdArgs = [
  resolvedTarget,
  '--min-tokens', String(config.minTokens),
  '--threshold', String(config.threshold),
  '--reporters', config.reporters.join(','),
  '--output', config.outputDir,
];
if (config.verbose) jscpdArgs.push('--verbose');
config.ignore.forEach(pat => {
  jscpdArgs.push('--ignore', pat);
});
writeLog('DEBUG', `jscpd arguments: ${JSON.stringify(jscpdArgs)}`);

//-----------------------------------------------------------------------------
// Spawn jscpd via npx
//-----------------------------------------------------------------------------
writeLog('INFO', `Executing: npx jscpd ${jscpdArgs.join(' ')}`);
const proc = spawn('npx', ['jscpd', ...jscpdArgs], { shell: false });

// Stream stdout
proc.stdout.on('data', chunk => {
  chunk.toString().split('\n').forEach(line => {
    if (line.trim()) writeLog('OUT', line.trim());
  });
});

// Stream stderr
proc.stderr.on('data', chunk => {
  chunk.toString().split('\n').forEach(line => {
    if (line.trim()) writeLog('ERR', line.trim());
  });
});

// Handle exit
proc.on('close', code => {
  if (code === 0) {
    writeLog('INFO', `jscpd completed successfully (exit code 0).`);
    process.exit(0);
  } else {
    writeLog('ERROR', `jscpd failed with exit code ${code}.`);
    process.exit(code);
  }
});
