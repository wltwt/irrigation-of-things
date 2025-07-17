import { getToken } from './auth.js';

export async function getSecureData() {
  const res = await fetch("/secure/data", {
    headers: { Authorization: `Bearer ${getToken()}` }
  });

  return await res.json();
}

export async function getSoilData(start, end) {
  const url = new URL("/influx/soil", window.location.origin);
  url.searchParams.set("start", start);
  url.searchParams.set("end", end);

  const res = await fetch(url, {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
      Authorization: `Bearer ${getToken()}`
    }
  });

  if (!res.ok) {
    throw new Error("Failed to fetch soil data");
  }

  return await res.json();
}
