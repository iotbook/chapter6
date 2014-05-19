#!/usr/bin/python
import paho.mqtt.client as mqtt
from time import sleep
import RPi.GPIO as GPIO
  
user = '<User ID Here>'
key = '<Token Here>'

# this method is called anytime a message is received on a subscribed topic
def on_message(client, obj, msg):
  print("Message on " + msg.topic + " --> " + str(msg.payload))
  if str(msg.payload) == "{\"g\":1}":
    GPIO.output(GREEN_LED, True)
  if str(msg.payload) == "{\"r\":1}":
    GPIO.output(RED_LED, True)
  if str(msg.payload) == "{\"g\":0}":
    GPIO.output(GREEN_LED, False)
  if str(msg.payload) == "{\"r\":0}":
    GPIO.output(RED_LED, False)

# setup GPIO pins
GPIO.setmode(GPIO.BCM)
GREEN_LED = 18
RED_LED = 24
BUTTON_PIN = 25
GPIO.setup(GREEN_LED, GPIO.OUT)
GPIO.setup(RED_LED, GPIO.OUT)
GPIO.output(GREEN_LED, False)
GPIO.output(RED_LED, False)
GPIO.setup(BUTTON_PIN, GPIO.IN)

# create MQTT client
client = mqtt.Client("iotbook-pi")
client.on_message = on_message
client.username_pw_set(user, key)
client.loop()

# connect to 2lemetry platform
client.connect("q.m2m.io", 1883, 60)
client.loop()

# subscribe to command topic
client.subscribe("<domain>/apress/pi/cmd", 0)
client.loop()

try:
  while(True):
    if (GPIO.input(BUTTON_PIN) == True):
      pub_str = "{\"b\":1}"
    else:
      pub_str = "{\"b\":0}"
    print pub_str
    client.publish("<domain>/apress/pi", pub_str)
    client.loop()
    sleep(1)
finally:
  # disconnect client
  client.disconnect()
  # cleanup GPIO
  GPIO.cleanup()
