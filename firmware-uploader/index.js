var fs = require('fs');
var request = require('request');
var args = require('yargs').argv;
var filepath = require('filepath');

if (args.binary) {
    var path = filepath.create(args.binary).path;

    var host = args.host ? args.host : '192.168.1.4';

    var file = fs.readFileSync(path);
    // convert binary data to base64 encoded string
    var contents = new Buffer(file).toString('base64');

    request.post({
        url: 'http://' + host + ':8080/api/flash',
        body: {
            binary: contents
        },
        timeout: 60000,
        json: true
    }, function (error, response, body) {
        if (!error && response.statusCode == 200) {
            // console.log(body)
        } else {
            console.log(error)
        }
    }).on('data', function (data) {
        // decompressed data as it is received
        console.log(data.toString().trim())
    })
    // .on('response', function (response) {
    //     // unmodified http.IncomingMessage object
    //     response.on('data', function (data) {
    //         // compressed data as it is received
    //         console.log('received ' + data.length + ' bytes of compressed data')
    //     })
    // });
} else {
    console.log("Please pass a binary file path");
}

// process.argv.forEach(function (val, index, array) {
//     console.log(index + ': ' + val);
// });