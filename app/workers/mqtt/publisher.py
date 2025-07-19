import json
from aiomqtt import Client
from app.workers.mqtt.client import settings


async def send_mqtt(sensor: str, command: str, seconds: int):
    topic   = f"{sensor}/cmd/{command}"
    payload = json.dumps({"seconds": seconds}).encode()
    #print(payload)
    async with Client(settings.host, port=settings.port) as c:
        await c.publish(topic, payload)
