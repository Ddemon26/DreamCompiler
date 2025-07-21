"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.run = void 0;
const path = require("path");
const Mocha = require("mocha");
const glob_1 = require("glob");
function run() {
    // Create the mocha test runner
    const mocha = new Mocha({
        ui: 'tdd',
        color: true,
        timeout: 10000 // 10 second timeout
    });
    const testsRoot = path.resolve(__dirname, '..');
    return new Promise((c, e) => {
        // Find all test files
        (0, glob_1.glob)('**/**.test.js', { cwd: testsRoot })
            .then((files) => {
            // Add files to the test suite
            files.forEach(f => mocha.addFile(path.resolve(testsRoot, f)));
            try {
                // Run the mocha test
                mocha.run(failures => {
                    if (failures > 0) {
                        e(new Error(`${failures} tests failed.`));
                    }
                    else {
                        c();
                    }
                });
            }
            catch (err) {
                console.error(err);
                e(err);
            }
        })
            .catch((err) => {
            console.error('Error finding test files:', err);
            e(err);
        });
    });
}
exports.run = run;
//# sourceMappingURL=index.js.map