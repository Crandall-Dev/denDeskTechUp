import paho.mqtt.client as paho
broker="broker.hivemq.com"
port=1883
topic="asc/testingOut123"

def on_publish(client,userdata,result):     #create function for callback
    print("data published \n")
    pass

client1 = paho.Client("control1")                    #create client object
client1.on_publish = on_publish             #assign function to callback
client1.connect(broker, port)                      #establish connection
ret = client1.publish(topic, "test message") 

width = 64
height = 48
total_pixels = width * height

payload = "1" * total_pixels
#payload = "1" * 20
screen_control_message_1 = "lcd:" + payload
print(screen_control_message_1)

ret = client1.publish(topic, screen_control_message_1) 
