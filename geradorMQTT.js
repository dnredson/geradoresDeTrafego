const mqtt = require('mqtt');

// Obtendo argumentos da linha de comando
const args = process.argv.slice(2);
if (args.length < 3) {
    console.log('Usage: node publisher.js <broker_ip> <topic> <messages_per_second>');
    process.exit(1);
}

const brokerIp = args[0];
const topic = args[1];

const client = mqtt.connect(`mqtt://${brokerIp}`);

const messagesPerSecond = args[2]; 

client.on('connect', function () {
    setInterval(() => {
        const message = new Date().toISOString();
        client.publish(topic, message);
        console.log(`Message sent to ${topic}: ${message}`);
    }, 1000 / messagesPerSecond);
});
