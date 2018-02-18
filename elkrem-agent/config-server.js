var fs = require('fs');
var express = require('express');
var bodyParser = require('body-parser');
var cp = require('child_process');
const EventEmitter = require('events').EventEmitter

var eventEmitter = new EventEmitter();


var app = express();
var router = express.Router();
var port = 8080;

var binaryFilePath = './binary.file.bin';

var flashingProcess = null;

function loadConfig(config) {

}

function init(config) {
    loadConfig(config);

    app.use(bodyParser.json({
        limit: '1mb'
    }));

    router.post('/flash', function (req, res) {
        var errorsArray = [];
        var parametersNumber = 0;
        var binaryFileContentsString = '';
        res.setHeader('Transfer-Encoding', 'chunked');

        if (req.body != undefined) {
            if (req.body.hasOwnProperty('binary')) {
                parametersNumber++;
                binaryFileContentsString = req.body.binary;
            } else {
                errorsArray.push('binary');
            }

            if (parametersNumber > 0) {
                if (errorsArray.length > 0) {
                    res.status(400).json({
                        error: 'Some parameters are invalid',
                        parameters: errorsArray
                    });
                } else {
                    var binayFile = new Buffer(binaryFileContentsString, 'base64');

                    if (fs.existsSync(binaryFilePath)) {
                        fs.unlinkSync(binaryFilePath);
                    }
                    fs.writeFileSync(binaryFilePath, binayFile);

                    eventEmitter.emit('flashingStarted');

                    if (flashingProcess && flashingProcess.connected)
                        flashingProcess.kill();

                    flashingProcess = cp.spawn('esptool-autoreset', ['--port /dev/ttyS3 -b 115200 write_flash 0x0000 ./binary.file.bin']);

                    var readline = require('readline');

                    var iface = readline.createInterface({
                        input: flashingProcess.stdout
                    });

                    iface.on("line", function (line) {
                        console.log(line.toString().trim());
                        res.write(line.toString().trim() + '\n');
                    });

                    // flashingProcess.stdout.on('data', function (data) {
                    //     console.log('stdout: ' + data.toString());
                    //     res.write(data.toString());
                    // });

                    flashingProcess.stderr.on('data', function (data) {
                        console.log(data.toString());
                        res.write(data.toString());
                    });

                    flashingProcess.on('exit', function (code) {
                        console.log('child process exited with code ' + code.toString());
                        if (code == 0) res.end();
                        else res.status(500).end();
                        eventEmitter.emit('flashingFinished');
                    });

                    flashingProcess.on('error', function (error) {
                        console.log('Error occured ' + error.toString());
                        res.status(500).end();
                        eventEmitter.emit('flashingFinished');
                    });
                }

            } else {
                res.status(400).json({
                    error: 'No known parameters were found'
                });
            }
        } else {
            res.status(400).json({
                error: 'The request body is empty'
            });
        }
    });


    app.use('/api', router);

    app.use(function (req, res) {
        res.status(404).json({
            error: 'The endpoint cannot be found'
        });
    });

    app.use(function (error, req, res, next) {
        if (error) {
            console.log(error);
            if (error instanceof SyntaxError) {
                res.status(400).json({
                    error: 'The provided request body is not a valid JSON'
                });
                return;
            } else {
                res.status(500).json({
                    error: 'Something unexpected happened, please repeat the request again later'
                });
                return;
            }
        }
        next();
    });
    configServer = app.listen(port);
    console.log("Config-Server: Server started");
}

function close(callback) {
    if (configServer) configServer.close(typeof callback === 'function' ? callback : undefined);
}

module.exports = eventEmitter;
module.exports.init = init;
module.exports.close = close;