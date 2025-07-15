from sqlalchemy.orm import Session
from datetime import datetime
from app import models, auth


def get_user_by_username(db: Session, username: str):
    return db.query(models.User).filter(models.User.username == username).first()

def get_user_by_email(db: Session, email: str):
    return db.query(models.User).filter(models.User.email == email).first()

def create_user(db: Session, username: str, email: str, password: str):
    user = models.User(
        username=username,
        email=email,
        password_hash=auth.hash_password(password)
    )
    db.add(user)
    db.commit()
    db.refresh(user)
    return user
