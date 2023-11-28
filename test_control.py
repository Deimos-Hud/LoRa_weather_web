import serial
import time
import json

# Set up serial port constants
SERIAL_PORT = 'COM5'
BAUD_RATE = 115200

# Initialize serial connection to the Arduino
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Function to send city to Arduino
def send_city_to_arduino(city):
    # Add some delay to ensure the Arduino is ready to receive data
    time.sleep(2)
    # Format the string and send it to Arduino
    ser.write(f'{city}\n'.encode())
    passive_listen()

# Function for passive listening
def passive_listen(timeout=10):
    """Listen for incoming data with a specified timeout."""
    start_time = time.time()
    while True:
        if time.time() - start_time > timeout:
            return None
        received_message = ser.readline().decode().strip()
        
        if received_message and received_message != "#NODATA":
            # Check if received_message starts and ends with [{ and }]
            if received_message.startswith("[{") and received_message.endswith("}]"):
                # Only update pc_weather_data.json if the format is correct
                with open(r'C:\Users\hudso\Documents\device networks final\pc_weather_data.json', 'w') as json_file:
                    json_file.write(received_message)
                    print(received_message)
        
        time.sleep(0.1)  # Short sleep to prevent busy waiting




# Read city from the file
with open('city_value.txt', 'r') as txt_file:
    city = txt_file.read().strip()

# Main loop
while True:
    with open('city_value.txt', 'r') as txt_file:
        city = txt_file.read().strip()
    city_to_send = city
    #print(city)
    send_city_to_arduino(city_to_send)
    time.sleep(10)
