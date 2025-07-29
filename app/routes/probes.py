from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from uuid import UUID

from app import models
from app.database import get_db
from app.auth import get_current_user
from app.crud import probes as crud
from app.schemas import probes

router = APIRouter(prefix="/probes", tags=["Probes"])


@router.post("/", response_model=probes.ProbeOut)
def create_probe(
    probe: probes.ProbeCreate,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    # Sjekk at brukeren eier devicen
    device = db.query(models.Device).filter_by(id=probe.device_id).first()
    if not device or device.user_id != current_user.id:
        raise HTTPException(status_code=403, detail="Du har ikke tilgang til denne enheten")
    
    return crud.create_probe(db, probe)


@router.get("/device/{device_id}", response_model=list[probes.ProbeOut])
def get_probes_for_device(
    device_id: UUID,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    device = db.query(models.Device).filter_by(id=device_id).first()
    if not device or device.user_id != current_user.id:
        raise HTTPException(status_code=403, detail="Du har ikke tilgang til denne enheten")
    
    return crud.get_probes_for_device(db, device_id)


@router.patch("/{probe_id}", response_model=probes.ProbeOut)
def update_probe(
    probe_id: UUID,
    update: probes.ProbeBase,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    probe = crud.get_probe(db, probe_id)
    if not probe or probe.device.user_id != current_user.id:
        raise HTTPException(status_code=403, detail="Du har ikke tilgang til denne proben")
    
    return crud.update_probe(db, probe_id, update)


@router.delete("/{probe_id}", response_model=probes.ProbeOut)
def delete_probe(
    probe_id: UUID,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    probe = crud.get_probe(db, probe_id)
    if not probe or probe.device.user_id != current_user.id:
        raise HTTPException(status_code=403, detail="Du har ikke tilgang til denne proben")
    
    return crud.delete_probe(db, probe_id)
