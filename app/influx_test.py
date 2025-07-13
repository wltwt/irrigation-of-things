from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS
import time
import random
import os

bucket = os.getenv("INFLUXDB_BUCKET")
org = os.getenv("INFLUXDB_ORG")
token = os.getenv("INFLUXDB_TOKEN")
url = "http://influxdb:8086"
client = InfluxDBClient(url=url, token=token, org=org)

write_api = client.write_api(write_options=SYNCHRONOUS)

users = ["alice", "bob"]
sensors = ["sensor1", "sensor2"]

for user in users:
    for sensor in sensors:
        point = (
            Point("sensor_data")
            .tag("user", user)
            .tag("sensor_id", sensor)
            .field("air_temperature", round(random.uniform(20, 25), 2))
            .field("moisture_level", round(random.uniform(30, 70), 2))
        )
        write_api.write(bucket=bucket, org=org, record=point)

print("Data sendt")

query = f'''
from(bucket: "metrics")
  |> range(start: -1h)
  |> filter(fn: (r) => r._measurement == "sensor_data")
  |> filter(fn: (r) => r.user == "alice")
  |> filter(fn: (r) => r._field == "air_temperature")
'''

tables = client.query_api().query(query)

print("Data hentet:")
for table in tables:
    for row in table.records:
        print(f"{row.get_time()} | {row.get_field()} = {row.get_value()}")