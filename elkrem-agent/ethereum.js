const EventEmitter = require('events').EventEmitter

var subscribedAddresses = {};
var web3 = null;
var currentBlockNumber = null;
var currentBlockHash = null;
var isSyncing = false;

var eventEmitter = new EventEmitter();

function init(_web3) {
    web3 = _web3;
    subscribedAddresses = {};
    currentBlockNumber = null;
    currentBlockHash = null;
    isSyncing = false;

    web3.eth.subscribe('newBlockHeaders', function (error, result) {
            if (error) console.log(error);
        })
        .on("data", function (data) {
            if (Object.keys(subscribedAddresses).length > 0)
                web3.eth.getBlock(data.number, true, function (error, block) {
                    if (!error) {
                        currentBlockNumber = block.number;
                        currentBlockHash = block.hash;
                        let transactions = block.transactions;
                        for (let i = 0; i < transactions.length; i++) {
                            for (var key in subscribedAddresses) {
                                if (transactions[i].to == subscribedAddresses[key].address) {
                                    web3.eth.getBalance(subscribedAddresses[key].address, function (error, balance) {
                                        if (!error) subscribedAddresses[key].balance = parseFloat(web3.utils.fromWei(balance, "ether"));
                                        else subscribedAddresses[key].balance = null;
                                        eventEmitter.emit('newTransaction', {
                                            from: transactions[i].from,
                                            to: transactions[i].to,
                                            value: parseFloat(web3.utils.fromWei(transactions[i].value, "ether")),
                                            hash: transactions[i].hash,
                                            blockNumber: transactions[i].blockNumber,
                                            blockHash: transactions[i].blockHash
                                        });
                                    });
                                    break;
                                }
                            }
                        }
                    } else
                        console.log(error);
                });
        }).on("error", function (error) {
            console.log(error);
        });

    web3.eth.isSyncing().then(function (sync) {
        isSyncing = sync ? true : false;
    });

    web3.eth.subscribe('syncing', function (error, sync) {
            if (error)
                console.log(error);
        })
        .on("data", function (sync) {
            isSyncing = sync ? true : false;

        })
        .on("changed", function (sync) {
            isSyncing = sync ? true : false;
        });
}

function subscribe(address) {
    if (web3.utils.isAddress(address) && !(address in subscribedAddresses)) {
        subscribedAddresses[address] = {};
        subscribedAddresses[address].address = address;
        subscribedAddresses[address].balance = null;
    }
}

function unsubscribe(address) {
    if (web3.utils.isAddress(address)) {
        delete subscribedAddresses[address];
    }
}

function unsubscribeAll() {
    subscribedAddresses = {};
}

function getBalance(address) {
    if (address in subscribedAddresses) {
        return subscribedAddresses[address].balance;
    }
}

function isSyncing() {
    return isSyncing;
}

function getCurrentBlock() {
    return {
        blockNumber: currentBlockNumber,
        blockHash: currentBlockHash
    }
}

module.exports = eventEmitter;
module.exports.init = init;
module.exports.subscribe = subscribe;
module.exports.unsubscribe = unsubscribe;
module.exports.unsubscribeAll = unsubscribeAll;
module.exports.getBalance = getBalance;
// module.exports.isSyncing = isSyncing;
module.exports.getCurrentBlock = getCurrentBlock;