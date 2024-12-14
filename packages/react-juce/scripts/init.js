#!/usr/bin/env node

import assert from "assert";
import { green, red, blue } from "chalk";
import { exec } from "child_process";
import { mkdirp, copy } from "fs-extra";
import { resolve } from "path";

var args = process.argv.slice(2);

assert(
  typeof args[0] === "string" && args[0].length > 0,
  "Must provide a path to the directory in which to initialize the template."
);

var targetDir = resolve(args[0]);
var packageDir = resolve(__dirname, "..");
var templateDir = resolve(packageDir, "template");

console.log("Initializing a React-JUCE template in:", green(targetDir));
console.log("Directory tree will be created if it does not exist.");

mkdirp(targetDir, function (err) {
  if (err) {
    console.error(red(err));
    process.exit(1);
  }

  console.log("[*] Copying template files");

  copy(templateDir, targetDir, function (err) {
    if (err) {
      console.error(red(err));
      process.exit(1);
    }

    console.log("[*] Installing dependencies");

    exec("npm install", { cwd: targetDir }, function (err, _stdout, stderr) {
      if (err) {
        console.error(red(err));
        console.error(stderr);
        process.exit(1);
      }

      console.log();
      console.log(`
${blue(
  "Success!"
)} Initialized a React-JUCE template in ${green(targetDir)}

You can now get started by typing:

${blue("cd")} ${args[0]}
${blue("npm start")}

Then adding the reactjuce::ReactApplicationRoot component to your project.
      `);
    });
  });
});
