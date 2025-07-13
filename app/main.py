from fastapi import FastAPI
from fastapi.responses import JSONResponse

app = FastAPI()

@app.get("/")
async def read_root():
    return {"message": "Test FastAPI app"}


@app.get("/status")
async def status():
    return JSONResponse(content={"status": "ok"})
