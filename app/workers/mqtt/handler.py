from app.database import get_db
from app.crud import devices
from uuid import UUID
import json
from aiomqtt import Client
from app.workers.mqtt.client import settings
from contextlib import asynccontextmanager
from sqlalchemy.orm import Session
from typing import Any

# async def handle_cmd(topic: str, payload: bytes) -> None:
#     print("RAW:", topic, payload)
#     try:
#         _, device_id, action = topic.split("/")
#         data = json.loads(payload)
#     except Exception as e:
#         print("Parse error:", e)
#         return
#     print("-> device:", device_id, "action:", action, "data:", data)


@asynccontextmanager
async def get_mqtt_client():
    async with Client(settings.host, port=settings.port) as c:
        yield c

async def handle_cmd(topic: str, payload: bytes) -> None:
    print("RAW:", topic, payload)
    try:
        _, device_id_str, action = topic.split("/")
        data = json.loads(payload)
        device_id = UUID(data["device"])
    except Exception as e:
        print("Parse error:", e)
        return

    print("-> device:", device_id, "action:", action, "data:", data)

    if data["command"] == "get_settings":
        db: Session = next(get_db())
        device = devices.get_device(db, device_id)
        if device is None:
            print("Device not found:", device_id)
            return

        response = {
            "cans": device.num_cans
        }

        # TODO: make this more generic and restructure the MQTT topic
        # should remove username and device name from topic
        # and use uuid device-ID instead
        
        
        # callback response
        topic_response = f"sensor1/cmd/set_cans"
        async with get_mqtt_client() as mqtt:
            await mqtt.publish(topic_response, json.dumps(response).encode())