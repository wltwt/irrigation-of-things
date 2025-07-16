from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from app.auth import get_current_user
from app.database import get_db

router = APIRouter(prefix="/secure", tags=["Test"])

@router.get("/data")
def get_data(current_user=Depends(get_current_user), db: Session = Depends(get_db)):
    return {
        "message": f"Hello {current_user.username}, here is your data!",
        "x": [1, 2, 3],
        "y": [10, 20, 15]
    }
