// run-jscpd.js
// Execute jscpd using the CLI via `npx`. This avoids issues with the
// programmatic API ignoring our options.

/**
 * Run the jscpd CLI and save the output for later review.
 */
const { spawnSync } = require("child_process");
const fs = require("fs");

const args = [
  "jscpd",
  "minix/kernel", // analyze only the kernel folder
  "--min-tokens",
  "50",
  "--threshold",
  "100",
  "--reporters",
  "console,json",
  "--output",
  "jscpd-report",
  "--verbose",
  "--ignore",
  "**/node_modules/**,**/*.md,**/releasetools/**,**/build/**",
];

console.log("Running jscpd with CLI:", args.join(" "));
const result = spawnSync("npx", args, { encoding: "utf8" });
fs.mkdirSync("jscpd-report", { recursive: true });
if (result.stdout) {
  fs.writeFileSync("jscpd-report/jscpd.log", result.stdout);
  console.log(result.stdout);
}
if (result.stderr) {
  fs.appendFileSync("jscpd-report/jscpd.log", result.stderr);
  console.error(result.stderr);
}
process.exitCode = result.status;
=======
const { execFileSync } = require("child_process");

console.log("Running jscpd with CLI:", args.join(" "));
execFileSync("npx", args, { stdio: "inherit" });
/**
 * Run jscpd iteratively for each supported extension. Splitting the
 * execution keeps the log output manageable and avoids timeouts.
 *
 * @param {string} ext Extension to analyze (without dot)
 */
function runForExtension(ext) {
  const args = [
    "jscpd",
    "minix/kernel", // analyze only the kernel folder inside the minix directory
    "--pattern",
    `**/*.${ext}`,
    "--min-tokens",
    "50",
    "--threshold",
    "100",
    "--reporters",
    "console,json",
    "--output",
    "jscpd-report",
    "--verbose",
    "--ignore",
    "**/node_modules/**,**/*.md,**/releasetools/**,**/build/**",
  ];

  console.log(`Running jscpd for .${ext} files:`);
  console.log(args.join(" "));
  execFileSync("npx", args, { stdio: "inherit" });
}

["c", "h", "s", "S"].forEach((ext) => runForExtension(ext));
