import paho.mqtt.client as mqtt_client

broker_url = "127.0.0.1"
broker_port = 1883

client = mqtt_client.Client()
client.connect(broker_url)


def on_message(client, userdata, message):
    print(f"Message received: {0}", str(message.payload.decode("utf-8")))


client.on_message = on_message    

print("Test Subscribe")
client.subscribe("luxReadings", qos=0)

print("Test Publish")
client.publish("luxReadings", 450.42, qos=0, retain=False)

