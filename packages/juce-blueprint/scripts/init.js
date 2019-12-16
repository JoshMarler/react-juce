#!/usr/bin/env node

var assert = require('assert');
var chalk = require('chalk');
var cp = require('child_process');
var fs = require('fs-extra');
var path = require('path');

var args = process.argv.slice(2);

assert(
  typeof args[0] === 'string' && args[0].length > 0,
  'Must provide a path to the directory in which to initialize the template.'
);

var targetDir = path.resolve(args[0]);
var packageDir = path.resolve(__dirname, '..');
var templateDir = path.resolve(packageDir, 'template');

console.log('Initializing a Blueprint template in:', chalk.green(targetDir));
console.log('Directory tree will be created if it does not exist.');

fs.mkdirp(targetDir, function(err) {
  if (err) {
    console.error(chalk.red(err));
    process.exit(1);
  }

  console.log('[*] Copying template files');

  fs.copy(templateDir, targetDir, function(err) {
    if (err) {
      console.error(chalk.red(err));
      process.exit(1);
    }

    console.log('[*] Linking juce-blueprint package');

    var pkgPath = path.resolve(targetDir, 'package.json');
    var pkg = require(pkgPath);
    pkg.dependencies['juce-blueprint'] = 'file:' + path.relative(targetDir, packageDir);

    fs.writeJson(pkgPath, pkg, { spaces: 2 }, function(err) {
      if (err) {
        console.error(chalk.red(err));
        process.exit(1);
      }

      console.log('[*] Installing dependencies');

      cp.exec('npm install', { cwd: targetDir }, function(err, stdout, stderr) {
        if (err) {
          console.error(chalk.red(err));
          console.error(stderr);
          process.exit(1);
        }

        console.log();
        console.log(`
${chalk.blue('Success!')} Initialized a Blueprint template in ${chalk.green(targetDir)}

You can now get started by typing:

  ${chalk.blue('cd')} ${args[0]}
  ${chalk.blue('npm start')}

Then adding the blueprint::ReactApplicationRoot component to your project.
        `);
      });
    });
  });
});
