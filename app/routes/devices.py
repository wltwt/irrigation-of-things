from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from app import models
from app.crud import devices as crud
from app.schemas import devices
from app.database import get_db
from app.auth import get_current_user
from uuid import UUID

router = APIRouter(prefix="/devices", tags=["Devices"])

@router.get("/", response_model=list[devices.DeviceOut])
def get_devices_for_user(
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    return crud.get_devices_for_user(db, current_user.id)


@router.post("/", response_model=devices.DeviceOut)
def create_device(
    device: devices.DeviceCreate,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    return crud.create_device(db=db, device=device, user_id=current_user.id)



@router.put("/{device_id}/set_cans", response_model=devices.DeviceOut)
def update_device_num_cans(
    device_id: UUID,
    update: devices.DeviceUpdate,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    device = crud.get_device(db, device_id)

    if not device or device.user_id != current_user.id:
        raise HTTPException(status_code=404, detail="Device not found")

    updated = crud.update_device(db, device_id, update)
    # TODO push update to MQTT
    return updated
