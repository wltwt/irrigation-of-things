from fastapi import APIRouter, Depends, Query
from datetime import datetime
from typing import List, Dict, Tuple
from pydantic import BaseModel, Field
from app.database import influx_client
from app.auth import get_current_user
from app.models import User

from decimal import Decimal

router = APIRouter(prefix="/influx", tags=["influx"])


class Series(BaseModel):
    name: str
    unit: str | None = None
    timestamps: List[datetime]
    values: List[float]

class MultiSeriesResponse(BaseModel):
    series: List[Series]


def build_flux_all(
    *, user: str, sensor: str, probes: list[str],
    start_iso: str, stop_iso: str, window: str = "10m"
) -> str:
    probe_regex = "|".join(map(str, probes))
    return f"""
from(bucket: "metrics")
  |> range(start: {start_iso}, stop: {stop_iso})
  |> filter(fn: (r) =>
        r["device"] == "{sensor}" and
        r["user"]   == "{user}"   and
        r._measurement =~ /(plant_moisture|environment)/
  )
  |> filter(fn: (r) =>
        (r._measurement == "plant_moisture" and r._field == "value" and r.probe =~ /^{probe_regex}$/) or
        (r._measurement == "environment"    and (r._field == "temperature" or r._field == "humidity"))
  )
  |> aggregateWindow(every: {window}, fn: mean, createEmpty: false)
  |> yield(name: "mean")
"""


@router.get("/dashboard", response_model=MultiSeriesResponse)
def get_dashboard_data(
    start: datetime = Query(..., description="ISO-start"),
    end:   datetime = Query(..., description="ISO-slutt"),
    sensor: str     = Query(..., description="sensor1 / sensor2 …"),
    probes: List[str] = Query(["1", "2"], description="?probes=1&probes=2 …"),
    window: str  = Query("10m", description="aggregateWindow-intervall"),
    current_user: User = Depends(get_current_user)
):
    flux = build_flux_all(
        user=current_user.username,
        sensor=sensor,
        probes=probes,
        start_iso=start.isoformat(),
        stop_iso=end.isoformat(),
        window=window
    )

    tables = influx_client.query_api().query(flux)


    grouped: dict[str, dict] = {}

    for table in tables:
        for rec in table.records:
            m, fld = rec.get_measurement(), rec.get_field()
            if m == "plant_moisture":
                probe_id = rec.values.get("probe")
                if probe_id not in probes:
                    continue
                name, unit = f"probe{probe_id}", "%"
            elif fld == "temperature":
                name, unit = "temperature", "°C"
            elif fld == "humidity":
                name, unit = "humidity", "%"
            else:
                continue

            v_raw = rec.get_value()
            if not isinstance(v_raw, (int, float, Decimal)):
                continue
            v = float(v_raw)

            bucket = grouped.setdefault(name, {"unit": unit, "timestamps": [], "values": []})
            
            if bucket["timestamps"] and bucket["timestamps"][-1] == rec.get_time():
                continue
            bucket["timestamps"].append(rec.get_time())
            bucket["values"].append(v)

    series_out = [
        Series(name=n, unit=b["unit"], timestamps=b["timestamps"], values=b["values"])
        for n, b in grouped.items()
    ]
    return MultiSeriesResponse(series=series_out)
