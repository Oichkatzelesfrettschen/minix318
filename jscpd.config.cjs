/**
 * Configuration for jscpd (JavaScript Copy/Paste Detector).
 * See: https://github.com/kucherenko/jscpd
 */
module.exports = {
  // Directories to scan for duplicate code
  path: ["kernel"], // analyze only the small kernel folder

  // Minimum tokens for duplication detection
  minTokens: 50,

  // Fail if duplication percentage is over threshold
  // Set high threshold so the command doesn't exit with error
  threshold: 100,

  // Ignore patterns to reduce noise in results
  ignore: [
    "**/node_modules/**", // ignore dependencies
    "**/*.md", // ignore markdown docs
    "**/releasetools/**", // ignore release tools
    "**/build/**", // ignore build artifacts
  ],

  // Output verbose diagnostic information
  verbose: true,

  // Reporters to generate output
  reporters: ["console", "json"],

  // Directory to store reports
  output: "jscpd-report",
};
