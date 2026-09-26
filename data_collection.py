import serial
import csv
import time
from datetime import datetime

PORT = "COM3"
BAUD = 115200
FILE_NAME = "plant_data.csv"

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

with open(FILE_NAME, "a", newline="") as file:
    writer = csv.writer(file)

    if file.tell() == 0:
        writer.writerow(["Date", "Time", "Temperature", "Humidity", "Soil", "Light"])

    try:
        while True:
            line = ser.readline().decode("utf-8").strip()

            if line:
                try:
                    parts = line.split(" | ")

                    temp = parts[0].replace("temp: ", "").replace(" C", "")
                    humidity = parts[1].replace("humidity: ", "").replace(" %", "")
                    soil = parts[2].replace("soil: ", "")
                    light = parts[3].replace("light: ", "")

                    now = datetime.now()
                    date = now.strftime("%Y.%m.%d")
                    current_time = now.strftime("%H:%M:%S.%f")[:-3]

                    writer.writerow([date, current_time, temp, humidity, soil, light])
                    file.flush()

                    print(f"{date} {current_time}   Temp: {temp} C   Humidity: {humidity} %   Soil: {soil}   Light: {light}")

                except (ValueError, IndexError):
                    pass

    except KeyboardInterrupt:
        print()
        print(f"Data saved to {FILE_NAME}")

    finally:
        ser.close()
