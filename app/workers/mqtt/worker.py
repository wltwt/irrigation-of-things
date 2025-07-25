import asyncio
from fastapi import FastAPI
from contextlib import asynccontextmanager, suppress
from aiomqtt import MqttError

from .client import get_client, settings
from .handler import handle_cmd

async def mqtt_loop():
    #print("MQTTworker starter …")
    try:
        async with get_client() as client:
            await client.subscribe(settings.topic_sub)
            #print("SUB filter :", settings.topic_sub)
            await client.subscribe("config/+/+")
            print("SUB filter : config/+/+")  

            async for msg in client.messages:
                await handle_cmd(str(msg.topic), msg.payload)
    except MqttError as e:
        print("MQTT error:", e)
        await asyncio.sleep(5)
        asyncio.create_task(mqtt_loop())



@asynccontextmanager
async def lifespan(app: FastAPI):
    task = asyncio.create_task(mqtt_loop())
    try:
        yield
    finally:
        task.cancel()
        with suppress(asyncio.CancelledError):
            await task

