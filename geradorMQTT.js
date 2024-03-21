const mqtt = require('mqtt');

// Obtendo argumentos da linha de comando
const args = process.argv.slice(2);
if (args.length < 4) {
    console.log('Usage: node publisher.js <broker_ip> <topic> <lambda> <duration_seconds>');
    process.exit(1);
}

const brokerIp = args[0];
const topic = args[1];
const lambda = parseFloat(args[2]);
const durationSeconds = parseInt(args[3], 10);

const client = mqtt.connect(`mqtt://${brokerIp}`);

function generatePoisson(lambda) {
    let L = Math.exp(-lambda);
    let p = 1.0;
    let k = 0;

    do {
        k++;
        p *= Math.random();
    } while (p > L);

    return k - 1;
}

client.on('connect', () => {
    console.log('Connected to broker');
    let startTime = Date.now();

    const intervalId = setInterval(() => {
        if (Date.now() - startTime > durationSeconds * 1000) {
            clearInterval(intervalId);
            client.end();
            console.log('Experiment finished!');
            return;
        }

        const messageCount = generatePoisson(lambda);

        for (let i = 0; i < messageCount; i++) {
            const message = new Date().toISOString();
            client.publish(topic, message);
            console.log(`Message sent to ${topic}: ${message}`);
        }
    }, 1000);
});
