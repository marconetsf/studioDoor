import time
import paho.mqtt.client as mqtt

# Callback Function on Connection with MQTT Server
def on_connect( client, userdata, flags, rc):
    print ("Connected with Code :" +str(rc))
    # Subscribe Topic from here
    client.subscribe("stopplay/#")

# Callback Function on Receiving the Subscribed Topic/Message
def on_message( client, userdata, msg):
    # print the message received from the subscribed topic
    print ( str(msg.payload) )

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set("akvvmyon", "wioEGgmZ518b")
client.connect("m11.cloudmqtt.com", 15939, 60)



# client.loop_forever()
client.loop_start()
time.sleep(1)
while True:
    client.publish("stopplay","Please Let's install a broker in our server")
    print ("Message Sent")
    time.sleep(15)

client.loop_stop()
client.disconnect()
