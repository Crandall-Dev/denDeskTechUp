import requests
import json
from PIL import Image
from time import sleep
from datetime import datetime
import paho.mqtt.client as paho

# MQTT settings
broker="broker.hivemq.com"
port=1883
topic="asc/testingOut123"


BLACK = 0
WHITE = 255
ZERO_BASELINE_Y = 0    # Celsius 0 degrees
JO_PLUS_ZERO_C = 21
MAX_TEMP_Y = 0
MIN_TEMP_Y = 7  
SCREEN_MAX_X = 64
SCREEN_MAX_Y = 48
TEMP_PIXEL_RANGE = (0, 39)
TEMP_PIXEL_MIN_Y = 0
TEMP_PIXEL_MAX_Y =47 
TEMP_MAX_RAIL_C = 40
TEMP_MIN_RAIL_C = -10


# ************************************************************** #
def map_range(x, a, b, c, d):
    """Map an integer value x from range [a, b] to range [c, d]."""
    return int((x - a) / (b - a) * (d - c) + c)


# *************************************************** #
def scale_y_by_temp_c(temp_c) -> int:
    temp_c = min(temp_c, TEMP_MAX_RAIL_C)
    temp_c = max(temp_c,TEMP_MIN_RAIL_C)
    mapped_y_pixels = map_range(temp_c, TEMP_MIN_RAIL_C, TEMP_MAX_RAIL_C, TEMP_PIXEL_MIN_Y, TEMP_PIXEL_MAX_Y)
    mapped_y_pixels = TEMP_PIXEL_MAX_Y - mapped_y_pixels

    return mapped_y_pixels

# ************************************************************** #

def get_temps_from_openweathermap() -> list:
    url = 'https://api.openweathermap.org/data/2.5/forecast?lat=47.6426&lon=-117.423093&appid=66e7f74aa3a366dfe45590d97655d2ef&units=metric'
    response = requests.get(url) 
    temps = []

    if response.status_code == 200:  # 200 indicates success
        data = json.loads(response.content)
        temps_3_hour = data["list"]
        for one_prediction in temps_3_hour:
            temps.append(int(round(one_prediction["main"]["temp"])))
        # print(temps)

    else:
        print('Error:', response.status_code, response.content)

    return temps


# ********************************************************************** #
def generate_screen_image(temperatures: list) -> Image:
 
    img = Image.new('L', (SCREEN_MAX_X, SCREEN_MAX_Y), color='black')
    for curr_x in range(SCREEN_MAX_X):
        curr_y = scale_y_by_temp_c(ZERO_BASELINE_Y)
        curr_jo_y = scale_y_by_temp_c(JO_PLUS_ZERO_C)
        img.putpixel((curr_x, curr_y), WHITE)
        img.putpixel((curr_x, curr_jo_y), WHITE)

    for slot_num in range(len(temperatures)):
        temp_c = temperatures[slot_num]
        curr_x = slot_num
        curr_y = scale_y_by_temp_c(temp_c)
        img.putpixel((curr_x, curr_y), WHITE)

    return img


# ********************************************************************** #
def send_image_to_screen(image: Image) -> None:
    message = "lcd:"

    for curr_y in range(SCREEN_MAX_Y):
        for curr_x in range(SCREEN_MAX_X):
            coordinate = curr_x, curr_y
            pixel = image.getpixel(coordinate)
            if pixel == 0:
                message += "0"
            else:
                message += "1"

    # Doing this twice because... their MQTT server is unreliable
    mqtt_client.publish(topic, message)
    sleep(2)
    mqtt_client.publish(topic, message)


# ********************************************************************** #
def update_temperature_screen():
    temperatures = get_temps_from_openweathermap()
    image = generate_screen_image(temperatures)
    #image.show(title="hi")
    send_image_to_screen(image)


# ********************************************************************** #
def send_blank_screen():
    message = "lcd:" + "0" * (SCREEN_MAX_X * SCREEN_MAX_Y)
    mqtt_client.publish(topic, message)


# ********************************************************************** #
if __name__ == "__main__":
    global mqtt_client
    print("Starting daemon - 5 minute updates")
    minute_interval = 5

    mqtt_client = paho.Client("weather gen agent")
    mqtt_client.connect(broker, port)
    sleep(3)
    try:
        print(datetime.now())
        update_temperature_screen()
        sleep(5)
        # sleep(minute_interval * 60)
    except KeyboardInterrupt:
        print("Received ^C - quitting.")
    finally:
        #sleep(2)
        #send_blank_screen()
        sleep(2)
        mqtt_client.disconnect()

    print("Exiting.")