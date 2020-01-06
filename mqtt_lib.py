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
    print(f"Message received: {0}", str(message.payload.decode("utf-8")))


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


