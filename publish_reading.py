from serial import Serial
from time import sleep

from Adafruit_IO import MQTTClient
# ToDo: Make this publish to Adafruit.

from config import *
from mqtt_lib import *

client = MQTTClient(USERNAME, IO_KEY)
client.on_message = on_message
client.on_subscribe = on_subscribe(FEED_ID)
serial_connection = Serial("/dev/ttyACM0", 9600, timeout=1)

# ToDo: Add in Try...Catch... statement for error handling.
client.connect()
client.subscribe(FEED_ID, qos=0)

while True:
    sleep(3)
    raw_message = serial_connection.readline()
    message = raw_message.decode().strip()

    if message:
        print("Publishing value: ", message)
        # ToDo: Add in Try...Catch...statement for error handling.
        client.publish(FEED_ID, message)

