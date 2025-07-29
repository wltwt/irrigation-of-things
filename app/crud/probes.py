from sqlalchemy.orm import Session
from uuid import UUID
from app import models
from app.schemas import probes


def create_probe(db: Session, probe: probes.ProbeCreate):
    db_probe = models.Probe(
        device_id=probe.device_id,
        channel=probe.channel,
        alias=probe.alias
    )
    db.add(db_probe)
    db.commit()
    db.refresh(db_probe)
    return db_probe


def get_probe(db: Session, probe_id: UUID):
    return db.query(models.Probe).filter(models.Probe.id == probe_id).first()


def get_probes_for_device(db: Session, device_id: UUID):
    return db.query(models.Probe).filter(models.Probe.device_id == device_id).all()


def update_probe(db: Session, probe_id: UUID, update: probes.ProbeBase):
    probe = get_probe(db, probe_id)
    if not probe:
        return None
    probe.channel = update.channel
    probe.alias = update.alias
    db.commit()
    db.refresh(probe)
    return probe


def delete_probe(db: Session, probe_id: UUID):
    probe = get_probe(db, probe_id)
    if not probe:
        return None
    db.delete(probe)
    db.commit()
    return probe
