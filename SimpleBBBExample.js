var mqtt = require('mqtt');
var b = require('bonescript');

var client = mqtt.connect('mqtt://<User ID>:<Token>@q.m2m.io?clientId=iotbook-bbb');

// setup LED GPIO pins
var greenLed = "P9_15";
var redLed = "P9_11";
b.pinMode(greenLed, b.OUTPUT);
b.pinMode(redLed, b.OUTPUT);
// setup analog pin for light reading
var lightPin = "P9_40";
var lightReading = 0.0;

// subscribe to the command topic
client.subscribe('<domain>/apress/bbb/cmd', function() {
// when a message arrives, check if it is an LED command
  client.on('message', function(topic, message, packet) {
    console.log("Received '" + message + "' on '" + topic + "'");
    if (message === '{"g":1}') {
      b.digitalWrite(greenLed, b.HIGH);
    } else if (message === '{"g":0}') {
      b.digitalWrite(greenLed, b.LOW);
    } else if (message === '{"r":1}') {
      b.digitalWrite(redLed, b.HIGH);
    } else if (message === '{"r":0}') {
      b.digitalWrite(redLed, b.LOW);
    }
  });
});

setInterval(function(){
  console.log('Reading light level');
  b.analogRead(lightPin, function(x){lightReading=x.value;});
  
  console.log(lightReading);
  client.publish('<domain>/apress/bbb', '{"l":' + lightReading.toFixed(2) + '}');
}, 1000);
