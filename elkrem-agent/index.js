const Web3 = require('web3');
const net = require('net');
const ethereum = require('./ethereum');
const whisper = require('./whisper');
const communication = require('./communication');
const configServer = require('./config-server');
const messages = require('./messages_pb');

web3 = new Web3(new Web3.providers.IpcProvider(process.platform === "win32" ? "\\\\.\\pipe\\geth.ipc" : "/root/.ethereum/testnet/geth.ipc", net));

ethereum.init(web3);
whisper.init(web3);
communication.init(process.platform === "win32" ? 'COM7' : '/dev/ttyS3');
communication.on('message', function (message) {
    if (message.type == messages.MessageType.PING) {
        // communication.sendMessage(messages.MessageType.PONG);
    } else if (message.type == messages.MessageType.INIT_REQUEST) {
        // communication.sendMessage(messages.MessageType.INIT_RESPONSE);
    } else if (message.type == messages.MessageType.ETHEREUM_ADDRESS_SUBSCRIPTION_REQUEST) {
        // console.log('Received ethereum address subscription request message: '+message.content.getToAddress());                
        if (message.content.hasToAddress()) {
            ethereum.subscribe(message.content.getToAddress());
        }
    } else if (message.type == messages.MessageType.ETHEREUM_ADDRESS_UNSUBSCRIPTION_REQUEST) {
        if (message.content.hasToAddress()) {
            ethereum.unsubscribe(message.content.getToAddress());
        }
    } else if (message.type == messages.MessageType.ETHEREUM_ALL_ADDRESSES_UNSUBSCRIPTION_REQUEST) {
        ethereum.unsubscribeAll();
    } else if (message.type == messages.MessageType.ETHEREUM_ADDRESS_BALANCE_REQUEST) {
        if (message.content.hasAddress() && ethereum.getBalance(message.content.getAddress())) {
            message.content.setValue(ethereum.getBalance(message.content.getAddress()));
            communication.sendMessage(messages.MessageType.ETHEREUM_ADDRESS_BALANCE_RESPONSE, message.content);
        }
    } else if (message.type == messages.MessageType.CURRENT_BLOCK_NUMBER_REQUEST) {
        var response = new messages.EthereumBlock();
        response.setBlockNumber(ethereum.getCurrentBlock().blockNumber);
        response.setBlockHash(ethereum.getCurrentBlock().blockHash);
        communication.sendMessage(messages.MessageType.CURRENT_BLOCK_NUMBER_RESPONSE, response);
    } else if (message.type == messages.MessageType.WHISPER_SUBSCRIBE) {
        if (message.content.hasPrivateKey() && message.content.hasTopic()) {
            whisper.subscribe(message.content.getPrivateKey(), undefined, [message.content.getTopic()], message.content.hasPubkey() ? message.content.getPubkey() : undefined, message.content.hasIsAscii() ? message.content.getIsAscii() : false);
        } else if (message.content.hasSymKeyPassword() && message.content.hasTopic()) {
            whisper.subscribe(undefined, message.content.getSymKeyPassword(), [message.content.getTopic()], message.content.hasPubkey() ? message.content.getPubkey() : undefined, message.content.hasIsAscii() ? message.content.getIsAscii() : false);
        }
    } else if (message.type == messages.MessageType.WHISPER_UNSUBSCRIBE_ALL) {
        //whisper.unsubscribeAll();
    } else if (message.type == messages.MessageType.WHISPER_TRANSMITTED_MESSAGE) {
        if (message.content.hasPubKey()) {
            whisper.post(message.content.getPubKey(), undefined, message.content.getSignaturePrivateKey(), message.content.getTopic(), message.content.getIsAscii(), message.content.getPayload());
        } else if (message.content.hasSymKeyPassword()) {
            whisper.post(undefined, message.content.getSymKeyPassword(), message.content.getSignaturePrivateKey(), message.content.getTopic(), message.content.getIsAscii(), message.content.getPayload());
        }
    }
});

ethereum.on('newTransaction', function (transaction) {
    var response = new messages.EthereumAddressSubscription();
    var block = new messages.EthereumBlock();
    response.setToAddress(transaction.to);
    response.setFromAddress(transaction.from);
    response.setValue(transaction.value);
    response.setHash(transaction.hash);
    block.setBlocknumber(transaction.blockNumber);
    block.setBlockhash(transaction.blockHash);
    response.setBlock(block);
    communication.sendMessage(messages.MessageType.ETHEREUM_ADDRESS_SUBSCRIPTION_RESPONSE, response);
});

whisper.on('newWhisperMessage', function (whisperMessage) {
    var response = new messages.ReceivedWhisperMessage();
    if (whisperMessage.from) response.setFrom(whisperMessage.from);
    if (whisperMessage.payload) response.setPayload(whisperMessage.payload);
    if (whisperMessage.topic) response.setTopic(whisperMessage.topic);
    if (whisperMessage.timestamp) response.setTimestamp(whisperMessage.timestamp);
    communication.sendMessage(messages.MessageType.WHISPER_RECEIVED_MESSAGE, response);
});

configServer.init();

configServer.on('flashingStarted', function () {
    communication.close();
});

configServer.on('flashingFinished', function () {
    communication.open();
});

communication.on('open', function () {
    console.log("Communication opened");
});
communication.on('close', function () {
    console.log("Communication closed");
});

// whisper.subscribe('0x61e2225acef5ecfa81652896fb17a29835aa13e6cc0f89c640d2e60e9255ea6e', undefined, ['0xdeadbeef'], undefined, true);
// whisper.post('0x04ee6732b6e0f43b39676b04d1d0a7f0f91794bfc0d56dc857ad200be9d2dcd5eab93053dea1fc8f3238469ca3deb147a641c84bb7b40ac91a8c540aca2231dccf',undefined,'0x61e2225acef5ecfa81652896fb17a29835aa13e6cc0f89c640d2e60e9255ea6e','0xdeadbeef',true,"7amada bas")

function cleanup(error) {
    console.log('Config-Server: Application shutting down, releasing resources.');
    if (error) console.error('Config-Server: error: ' + error.stack);
    configServer.close(function (err) {
        if (err) {
            if (err) console.error('Config-Server: error: ' + err.stack);
        }
        process.exit();
    });
}

process.stdin.resume();
process.on('SIGINT', cleanup);
process.on('SIGTERM', cleanup);
process.on('uncaughtException', cleanup);