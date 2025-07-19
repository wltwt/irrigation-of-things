from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from app.database import engine
from app import models
from app.routes import user, login, test, influx, mqtt
from app.workers.mqtt.worker import lifespan

models.Base.metadata.create_all(bind=engine)

app = FastAPI(lifespan=lifespan)

app.include_router(user.router)
app.include_router(login.router)
app.include_router(test.router)
app.include_router(influx.router)
app.include_router(mqtt.router)

app.mount("/static", StaticFiles(directory="frontend/static"), name="static")

# Point to index.html
@app.get("/")
def root():
    return FileResponse("frontend/static/index.html")