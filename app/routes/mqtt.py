from fastapi import APIRouter, Depends, HTTPException
from pydantic import BaseModel
#from app.workers.mqtt.publisher import MqttPublisher
from app.workers.mqtt.publisher import send_mqtt as pub


router = APIRouter()
#pub = MqttPublisher()

class Cmd(BaseModel):
    sensor: str
    command: str
    seconds: int = 10

@router.post("/command")
async def send_command(cmd: Cmd):
    try:
        await pub(cmd.sensor, cmd.command, cmd.seconds)
        return {"status": "sent"}
    except Exception as e:
        raise HTTPException(500, f"MQTT error: {e}")
