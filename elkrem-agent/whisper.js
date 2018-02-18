const EventEmitter = require('events').EventEmitter
var edn = require("jsedn");
var web3 = null;

var eventEmitter = new EventEmitter();

var identities = {};
var isSubscribed = false;
var subscriptions = [];
var emittedStatusMsgs = [];

function init(_web3) {
    web3 = _web3;
    isSubscribed = false;
}

function subscribe(privateKey, symKeyPassword, topics, pubKey, isAscii) {
    var subscribtionId = getSubscriptionHash(privateKey, symKeyPassword, topics, pubKey, isAscii);
    if (!(subscriptions.indexOf(subscribtionId) > -1)) {
        subscriptions.push(subscribtionId);
        if (privateKey) {
            getIdentity(privateKey, 'asym', function (identity) {
                wsubscribe(identity, topics, pubKey, isAscii);
            });
        } else if (symKeyPassword) {
            getIdentity(privateKey, 'sym', function (identity) {
                wsubscribe(identity, topics, pubKey, isAscii);
            });
        }
    }
}

function getSubscriptionHash(privateKey, symKeyPassword, topics, pubKey, isAscii) {
    var subscriptionData = '';
    if (typeof privateKey !== 'undefined') subscriptionData += privateKey.toString();
    if (typeof symKeyPassword !== 'undefined') subscriptionData += symKeyPassword.toString();
    if (typeof topics !== 'undefined') subscriptionData += topics.toString();
    if (typeof pubKey !== 'undefined') subscriptionData += pubKey.toString();
    if (typeof isAscii !== 'undefined') subscriptionData += isAscii.toString();
    var crypto = require('crypto'),
        shasum = crypto.createHash('sha1');
    shasum.update("subscriptionData");
    return shasum.digest('hex');
}

function getIdentity(key, type, callback) {
    if (key in identities) {
        callback(identities[key]);
    } else {
        web3.shh.addPrivateKey(key).then((id) => {
            identities[key] = {};
            identities[key].id = id;
            identities[key].type = type;
            callback(identities[key]);
        })
    }
}

function wsubscribe(identity, topics, pubKey, isAscii) {
    var subscribeParams = {};
    if (identity.type == 'asym') {
        subscribeParams.privateKeyID = identity.id;
    } else if (identity.type == 'sym') {
        subscribeParams.symKeyID = identity.id;
    }
    if (pubKey) {
        subscribeParams.sig = pubKey;
    }
    if (topics) {
        subscribeParams.topics = topics;
    }
    web3.shh.subscribe("messages", subscribeParams)
        .on('data', function (message) {
            if (message.topic == '0xaabb11ee') {
                var statusMessage = edn.toJS(edn.parse(web3.utils.toAscii(message.payload)));
                if (statusMessage[':type'] == ':message' && !(emittedStatusMsgs.indexOf(statusMessage[':message-id']) > -1)) {
                    emittedStatusMsgs.push(statusMessage[':message-id']);
                    eventEmitter.emit('newWhisperMessage', {
                        timestamp: (statusMessage[':timestamp'] / 1000 | 0),
                        topic: message.topic,
                        payload: statusMessage[':content'],
                        from: message.sig
                    })
                }
            } else {
                var payload = isAscii ? web3.utils.toAscii(message.payload) : message.payload;
                eventEmitter.emit('newWhisperMessage', {
                    timestamp: message.timestamp,
                    topic: message.topic,
                    payload: payload,
                    from: message.sig
                })
            }
        });
    isSubscribed = true;
}

function wpost(pubKey, symKeyIdentity, signature, topic, isAscii, payload) {
    var postParams = {};
    if (pubKey) {
        postParams.pubKey = pubKey;
    } else if (symKeyIdentity) {
        postParams.symKeyID = symKeyIdentity.id;

    }
    if (signature) {
        postParams.sig = signature.id;
    }
    if (topic) {
        postParams.topic = topic;
    }
    if (payload) {
        postParams.payload = isAscii ? web3.utils.fromAscii(payload) : payload;
    }
    postParams.powTime = 1;
    postParams.powTarget = 0.001;
    web3.shh.post(postParams);
}

function unsubscribeAll() {
    if (isSubscribed)
        web3.shh.clearSubscriptions();
    isSubscribed = false;
    subscriptions = [];
}

function post(pubKey, symKeyPassword, signaturePrivateKey, topic, isAscii, payload) {
    if (signaturePrivateKey) {
        getIdentity(signaturePrivateKey, 'asym', function (signaturePrivateKeyIdentity) {
            if (pubKey) {
                wpost(pubKey, undefined, signaturePrivateKeyIdentity, topic, isAscii, payload);
            } else if (symKeyPassword) {
                getIdentity(symKeyPassword, 'sym', function (symKeyPasswordIdentity) {
                    wpost(undefined, symKeyPasswordIdentity, signaturePrivateKeyIdentity.id, topic, isAscii, payload);
                });
            }
        });
    } else {
        if (pubKey) {
            wpost(pubKey, undefined, undefined, topic, isAscii, payload);
        } else if (symKeyPassword) {
            getIdentity(symKeyPassword, 'sym', function (symKeyPasswordIdentity) {
                wpost(undefined, symKeyPasswordIdentity, undefined, topic, isAscii, payload);
            });
        }
    }
}

module.exports = eventEmitter;
module.exports.init = init;
module.exports.subscribe = subscribe;
module.exports.unsubscribeAll = unsubscribeAll;
module.exports.post = post;