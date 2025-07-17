from fastapi import APIRouter, Depends, Query
from datetime import datetime
from app.schemas.influx import TimeSeriesData
from app.database import influx_client
from app.auth import get_current_user
from app.models import User

router = APIRouter(prefix="/influx", tags=["influx"])

@router.get("/soil", response_model=TimeSeriesData)
def get_soil_data(
    start: datetime = Query(..., description="Start time (ISO8601)"),
    end: datetime = Query(..., description="End time (ISO8601)"),
    current_user: User = Depends(get_current_user)
):
    query = f'''
        from(bucket: "metrics")
          |> range(start: {start.isoformat()}, stop: {end.isoformat()})
          |> filter(fn: (r) => r._measurement == "mqtt_consumer")
          |> filter(fn: (r) => r._field == "temp")
          |> filter(fn: (r) => r.sensor_id == "sensor1")
          |> aggregateWindow(every: 10m, fn: mean, createEmpty: false)
    '''

    result = influx_client.query_api().query(query)

    timestamps = []
    values = []

    for table in result:
        for record in table.records:
            timestamps.append(record.get_time())
            values.append(record.get_value())

    return TimeSeriesData(timestamps=timestamps, values=values)
