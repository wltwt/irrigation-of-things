from pydantic import BaseModel
from uuid import UUID
from typing import Optional


class ProbeBase(BaseModel):
    channel: str
    alias: Optional[str] = None


class ProbeCreate(ProbeBase):
    device_id: UUID


class ProbeOut(ProbeBase):
    id: UUID
    device_id: UUID

    model_config = {
        "from_attributes": True
    }
