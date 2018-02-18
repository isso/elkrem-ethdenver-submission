const SerialPort = require('serialport');
const hdlc = require('./hdlc');
const messages = require('./messages_pb');
const EventEmitter = require('events').EventEmitter

var port = null;
var eventEmitter = new EventEmitter();


function init(_portString) {
    hdlc.init(sendbyte);
    hdlc.on('newFrame', router);

    port = new SerialPort(_portString, {
        baudRate: 115200,
        rtscts: true,
        autoOpen: false
    });

    port.on('error', function (err) {
        eventEmitter.emit('error', err.message);
    })

    port.on('data', function (bytes) {
        // console.log(new String(bytes).toString());
        hdlc.byteReceiver(bytes);
    });

    open();

}

function open() {
    if (!port.isOpen) port.open(function (err) {
        if (err) {
            eventEmitter.emit('error', err.message);
        } else {
            port.set({
                rts: false
            }, (err) => {

            });
            eventEmitter.emit('open');
        }
    });
}

function close() {
    if (port.isOpen) port.close(function (err) {
        if (err) {
            eventEmitter.emit('error', err.message);
        } else {
            eventEmitter.emit('close');
        }
    });
}

function sendbyte(data) {
    port.write([data], function (err) {
        if (err) {
            eventEmitter.emit('error', err.message);
        }
    });
}

function router(frame) {
    if (frame.length > 0) {
        var message = {};
        var type = frame.shift();
        console.log("New message received: " + type);

        if (type == messages.MessageType.ETHEREUM_ADDRESS_SUBSCRIPTION_REQUEST) {
            message = messages.EthereumAddressSubscription.deserializeBinary(frame);
        } else if (type == messages.MessageType.ETHEREUM_ADDRESS_UNSUBSCRIPTION_REQUEST) {
            message = messages.EthereumAddressSubscription.deserializeBinary(frame);
        } else if (type == messages.MessageType.ETHEREUM_ADDRESS_BALANCE_REQUEST) {
            message = messages.EthereumAddressBalance.deserializeBinary(frame);
        } else if (type == messages.MessageType.CURRENT_BLOCK_NUMBER_REQUEST) {
            message = messages.EthereumBlock.deserializeBinary(frame);
        } else if (type == messages.MessageType.WHISPER_SUBSCRIBE) {
            message = messages.WhisperSubscribe.deserializeBinary(frame);
        } else if (type == messages.MessageType.WHISPER_TRANSMITTED_MESSAGE) {
            message = messages.TransmittedWhisperMessage.deserializeBinary(frame);
        } else if (type == messages.MessageType.SERIAL_REQUEST) {
            message = messages.SerialMessage.deserializeBinary(frame);
        }
        eventEmitter.emit('message', {
            type: type,
            content: message
        });
    }
}

function sendMessage(type, message) {
    var bytes = message ? Array.from(message.serializeBinary()) : [];
    bytes.unshift(type);
    hdlc.sendFrame(bytes);
}

module.exports = eventEmitter;
module.exports.init = init;
module.exports.open = open;
module.exports.close = close;
module.exports.sendMessage = sendMessage;