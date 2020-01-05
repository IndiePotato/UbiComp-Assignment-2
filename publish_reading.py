import serial

import paho.mqtt.client as mqtt_client
# ToDo: Make this publish to Adafruit.

from mqtt_lib import *

broker_url = "127.0.0.1"
broker_port = 1883

client = mqtt_client.Client()
client.connect(broker_url)

# client.on_message = on_message

serial_connection = serial.Serial("/dev/ttyACM0", 9600, timeout=1)

print("Subscribing to luxReadings feed....")
# ToDo: Add in Try...Catch... statement for error handling.
client.subscribe("luxReadings", qos=0)

while True:
    raw_message = serial_connection.readline()
    message = raw_message.decode().strip()

    if message:
        # ToDo: Add in Try...Catch...statement for error handling.
        client.publish("luxReadings", message, qos=0, retain=False)

