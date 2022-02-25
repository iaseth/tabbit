#!/usr/bin/env node

'use strict';

const fs = require('fs');



function Tabbit (paths, index) {
	const path = paths[index];
	const naturalIndex = parseInt(index) + 1;
	console.log(`Tabbit (${naturalIndex}/${paths.length}): '${path}'`);
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
