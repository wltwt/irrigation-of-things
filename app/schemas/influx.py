from pydantic import BaseModel
from datetime import datetime
from typing import List

class TimeSeriesData(BaseModel):
    timestamps: List[datetime]
    values: List[float]
