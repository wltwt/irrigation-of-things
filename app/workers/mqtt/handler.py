import json
from typing import Any

async def handle_cmd(topic: str, payload: bytes) -> None:
    print("RAW:", topic, payload)
    try:
        _, device_id, action = topic.split("/")
        data = json.loads(payload)
    except Exception as e:
        print("Parse error:", e)
        return
    print("-> device:", device_id, "action:", action, "data:", data)
