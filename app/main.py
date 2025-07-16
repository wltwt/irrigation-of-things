from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from app.database import engine
from app import models
from app.routes import user, login, test

models.Base.metadata.create_all(bind=engine)

app = FastAPI()

app.include_router(user.router)
app.include_router(login.router)
app.include_router(test.router)

app.mount("/static", StaticFiles(directory="frontend/static"), name="static")

# Point to the main HTML file
@app.get("/")
def root():
    return FileResponse("frontend/static/index.html")