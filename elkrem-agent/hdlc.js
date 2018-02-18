const crc = require('crc');
const EventEmitter = require('events').EventEmitter


const FRAME_BOUNDARY_OCTET = 0x7E;
const CONTROL_ESCAPE_OCTET = 0x7D
const INVERT_OCTET = 0x20;
const MINIHDLC_MAX_FRAME_LENGTH = 1024;

var eventEmitter = new EventEmitter();

var pendingFrame = {};
var byteSendingFunction = function (byte) {};

function init(_byteSendingFunction) {
    byteSendingFunction = _byteSendingFunction;
    pendingFrame.framePosition = 0;
    pendingFrame.frameChecksum = undefined;
    pendingFrame.escapeCharacter = false;
    pendingFrame.receivedFrameBuffer = [];
    pendingFrame.isStared = false;
}

function sendchar(data) {
    byteSendingFunction(data & 0xff);
}

function byteReceiver(bytes) {
    for (var i = 0; i < bytes.length; i++) {
        /* FRAME FLAG */
        data = bytes[i];
        /* FRAME FLAG */
        if (data == FRAME_BOUNDARY_OCTET) {
            if (pendingFrame.escapeCharacter == true) {
                pendingFrame.escapeCharacter = false;
            }
            /* If a valid frame is detected */
            else if ((pendingFrame.framePosition >= 2) &&
                (pendingFrame.frameChecksum == ((pendingFrame.receivedFrameBuffer[pendingFrame.framePosition - 1] << 8) | (pendingFrame.receivedFrameBuffer[pendingFrame.framePosition - 2] & 0xff)))) // (msb << 8 ) | (lsb & 0xff)
            {
                /* Call the user defined function and pass frame to it */
                eventEmitter.emit('newFrame', pendingFrame.receivedFrameBuffer.slice(0, pendingFrame.receivedFrameBuffer.length - 2));
            }
            pendingFrame.framePosition = 0;
            pendingFrame.frameChecksum = undefined;
            pendingFrame.escapeCharacter = false;
            pendingFrame.receivedFrameBuffer = [];
            continue;
        }

        if (pendingFrame.escapeCharacter) {
            pendingFrame.escapeCharacter = false;
            data ^= INVERT_OCTET;
        } else if (data == CONTROL_ESCAPE_OCTET) {
            pendingFrame.escapeCharacter = true;
            continue;
        }

        pendingFrame.receivedFrameBuffer[pendingFrame.framePosition] = data;

        if (pendingFrame.framePosition - 2 >= 0) {
            pendingFrame.frameChecksum = crc.crc16ccitt([pendingFrame.receivedFrameBuffer[pendingFrame.framePosition - 2]], pendingFrame.frameChecksum)
        }

        pendingFrame.framePosition++;

        if (pendingFrame.framePosition == MINIHDLC_MAX_FRAME_LENGTH) {
            pendingFrame.framePosition = 0;
            pendingFrame.frameChecksum = undefined;
            pendingFrame.escapeCharacter = false;
            pendingFrame.receivedFrameBuffer = [];
        }
    }
}

function sendFrame(rawFrame) {
    var byte;
    var fcs = undefined;

    sendchar(FRAME_BOUNDARY_OCTET);

    for (i = 0; i < rawFrame.length; i++) {
        byte = rawFrame[i];
        fcs = crc.crc16ccitt([byte], fcs);
        if ((byte == CONTROL_ESCAPE_OCTET) || (byte == FRAME_BOUNDARY_OCTET)) {
            sendchar(CONTROL_ESCAPE_OCTET);
            byte ^= INVERT_OCTET;
        }
        sendchar(byte);
    }

    byte = fcs;
    if ((byte == CONTROL_ESCAPE_OCTET) || (byte == FRAME_BOUNDARY_OCTET)) {
        sendchar(CONTROL_ESCAPE_OCTET);
        byte ^= INVERT_OCTET;
    }
    sendchar(byte);
    byte = fcs >> 8;
    if ((byte == CONTROL_ESCAPE_OCTET) || (byte == FRAME_BOUNDARY_OCTET)) {
        sendchar(CONTROL_ESCAPE_OCTET);
        byte ^= INVERT_OCTET;
    }
    sendchar(byte);
    sendchar(FRAME_BOUNDARY_OCTET);
}

module.exports = eventEmitter;
module.exports.init = init;
module.exports.byteReceiver = byteReceiver;
module.exports.sendFrame = sendFrame;