from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, declarative_base
from influxdb_client import InfluxDBClient
from .config import DATABASE_URL, INFLUX_URL, INFLUX_TOKEN, INFLUX_ORG, INFLUX_BUCKET
import os

engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

Base = declarative_base()

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

influx_client = InfluxDBClient(
    url=INFLUX_URL,
    token=INFLUX_TOKEN,
    org=INFLUX_ORG
)

