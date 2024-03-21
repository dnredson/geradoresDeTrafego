const mqtt = require('mqtt');

// Obtendo argumentos da linha de comando
const args = process.argv.slice(2);
if (args.length < 2) {
    console.log('Usage: node subscriber.js <broker_ip> <topic>');
    process.exit(1);
}

const brokerIp = args[0];
const topic = args[1];
const client = mqtt.connect(`mqtt://${brokerIp}`);

client.on('connect', function () {
    client.subscribe(topic, function (err) {
        if (!err) {
            console.log(`Subscribed to ${topic}`);
        } else {
            console.error(`Could not subscribe to ${topic}:`, err);
        }
    });
});

client.on('message', function (topic, message) {
    const messageTime = new Date(message.toString());
    const currentTime = new Date();
    const delta = currentTime - messageTime;
    console.log(`Received message on topic ${topic}: ${message.toString()}`);
    console.log(`Message timestamp: ${messageTime}`);
    console.log(`Received timestamp: ${currentTime}`);
    console.log(`Time delta: ${delta} ms`);
});
