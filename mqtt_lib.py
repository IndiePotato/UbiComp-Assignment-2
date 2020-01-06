"""
Library file for MQTT-related functions for UbiComp Assignment.
"""

import sys


def on_message(message):
    """
    MQTT Client Object on_message function value.

    Arguments:
        message The message received.
    """
    print("Message received: ", str(message.payload.decode("utf-8")))


def on_connect():
    """
    MQTT Client on_connect function value.
    """
    print("Connected.")


def on_disconnect():
    """
    MQTT Client on_disconnect function value.
    """
    print("Disconnected.")
    sys.exit(0)


def on_subscribe(feed_id):
    """
    MQTT Client on_subscribe function value.

    Arguments:
         feed_id (str): The ID of the Feed to subscribe to.
    """
    print("Subscribed to ", feed_id)



