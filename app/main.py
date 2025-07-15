from fastapi import FastAPI
from app.database import engine
from app import models
from app.routes import user, login

models.Base.metadata.create_all(bind=engine)

app = FastAPI()

app.include_router(user.router)
app.include_router(login.router)