from aiomqtt import Client
from pydantic_settings import BaseSettings

class MQTTSettings(BaseSettings):
    host: str = "mosquitto"
    port: int = 1883
    user: str | None = None
    password: str | None = None
    topic_sub: str = "command/#"

settings = MQTTSettings()

def get_client() -> Client:
    return Client(
        settings.host,
        port=settings.port,
        username=settings.user,
        password=settings.password,
    )
