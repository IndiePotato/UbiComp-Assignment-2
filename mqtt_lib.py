"""
Library file for MQTT-related functions for UbiComp Assignment.
"""


def on_message(message):
    """
    MQTT Client Object on_message function value.

    Arguments:
        message The message received.
    """
    print(f"Message received: {0}", str(message.payload.decode("utf-8")))



