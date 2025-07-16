import { getToken } from './auth.js';

export async function getSecureData() {
  const res = await fetch("/secure/data", {
    headers: { Authorization: `Bearer ${getToken()}` }
  });

  return await res.json();
}
