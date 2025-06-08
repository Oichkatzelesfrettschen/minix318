// run-jscpd.js
// Execute jscpd using the CLI via `npx`. This avoids issues with the
// programmatic API ignoring our options.

const { execFileSync } = require('child_process');

const args = [
  'jscpd',
  'kernel', // analyze only the kernel folder
  '--min-tokens', '50',
  '--threshold', '100',
  '--reporters', 'console,json',
  '--output', 'jscpd-report',
  '--verbose',
  '--ignore', '**/node_modules/**,**/*.md,**/releasetools/**,**/build/**'
];

console.log('Running jscpd with CLI:', args.join(' '));
execFileSync('npx', args, { stdio: 'inherit' });

