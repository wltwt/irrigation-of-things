from sqlalchemy.orm import Session
from uuid import UUID
from app import models
from app.schemas import devices


def create_device(db: Session, device: devices.DeviceCreate, user_id: UUID):
    db_device = models.Device(
        name=device.name,
        user_id=user_id,
        num_cans=1
    )
    db.add(db_device)
    db.commit()
    db.refresh(db_device)
    return db_device


def get_device(db: Session, device_id: UUID):
    return db.query(models.Device).filter(models.Device.id == device_id).first()


def update_device(db: Session, device_id: UUID, update: devices.DeviceUpdate):
    device = get_device(db, device_id)
    if not device:
        return None
    if update.name is not None:
        device.name = update.name
    if update.num_cans is not None:
        device.num_cans = update.num_cans
    db.commit()
    db.refresh(device)
    return device


def get_devices_for_user(db: Session, user_id: UUID):
    return db.query(models.Device).filter(models.Device.user_id == user_id).all()
