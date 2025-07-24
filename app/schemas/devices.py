from pydantic import BaseModel
from uuid import UUID
from typing import Optional


class DeviceBase(BaseModel):
    name: str


class DeviceCreate(BaseModel):
    name: str


class DeviceOut(DeviceBase):
    id: UUID
    num_cans: int

    model_config = {
        "from_attributes": True
    }


class DeviceUpdate(BaseModel):
    name: Optional[str] = None
    num_cans: Optional[int] = None
