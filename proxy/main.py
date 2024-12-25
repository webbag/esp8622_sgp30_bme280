import influxdb_client, os, time
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
org = "webbag"
from flask import Flask, request, jsonify

app = Flask(__name__)

# utwórz w UI token API
token = "*********"
url = "http://127.0.0.1:8086"

write_client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)

write_api = write_client.write_api(write_options=SYNCHRONOUS)

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    print(f"Otrzymano dane: {data}")

    if isinstance(data, dict):  # Upewniamy się, że dane są w formacie słownika

        # Zapisujemy dane do InfluxDB
        bucket = "esp8622model"
        for key, value in data.items():
            point = (
                Point(key)  # Nazwa pomiaru (np. "temperature", "pressure", itp.)
                .tag("Miejsce", "sypialnia")  # Tag z informacją o pokoju
                .field(key, value)  # Pole z wartością (nazwa klucza i wartość)
            )
            write_api.write(bucket=bucket, org=org, record=point)

        return jsonify({"status": "received", "data": data})
    else:
        return jsonify({"status": "error", "message": "Invalid data format, expected JSON object"}), 400


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
