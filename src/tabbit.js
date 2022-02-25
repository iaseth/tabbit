#!/usr/bin/env node

'use strict';

const fs = require('fs');



function Tabbit (paths, index) {
	const path = paths[index];
	const naturalIndex = parseInt(index) + 1;
	console.log(`Tabbit (${naturalIndex}/${paths.length}): '${path}'`);

	try {
		const text = fs.readFileSync(path).toString('utf-8');
		const lines = text.split("\n");
		for (let index in lines) {
			let line = lines[index];
			console.log(`Line ${index}: ${line}`);
		}
		console.log("\tExists!");
	} catch (e) {
		console.log(`\tNot found!`);
	}
}

function main () {
	const args = process.argv.slice(2);
	const paths = [];

	for (let arg of args) {
		if (arg.startsWith("--")) {
			// full flag
		} else if (arg.startsWith("-")) {
			// short flag
		} else {
			paths.push(arg);
		}
	}

	for (let index in paths) {
		Tabbit(paths, index);
	}
}

main();
