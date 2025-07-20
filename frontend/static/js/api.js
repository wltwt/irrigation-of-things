import { getToken } from './auth.js';

export async function getSecureData() {
  const res = await fetch("/secure/data", {
    headers: { Authorization: `Bearer ${getToken()}` }
  });

  return await res.json();
}

export async function getDashboardData(start, end, sensor = "sensor1", probes = ["1","2"]) {
  const url = new URL("/influx/dashboard", window.location.origin);
  url.searchParams.set("start", start);
  url.searchParams.set("end", end);
  url.searchParams.set("sensor", sensor);
  probes.forEach(p => url.searchParams.append("probes", p));

  const res = await fetch(url, {
    method: "GET",
    headers: { "Content-Type": "application/json", Authorization: `Bearer ${getToken()}` }
  });
  if (!res.ok) throw new Error("Failed to fetch dashboard data");
  return await res.json();
}
