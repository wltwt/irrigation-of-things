import { getToken, getUsername, logout } from './auth.js';

if (!getToken()) {
  window.location.href = "/static/html/index.html";
} else {
  const username = getUsername();
  const userInfo = document.getElementById("user-info");
  if (username && userInfo) {
    userInfo.textContent = `Brukernavn: ${username}`;
  }
}

document.getElementById("logout").addEventListener("click", () => {
  logout();
  window.location.href = "/static/html/index.html";
});


async function fetchDevices() {
  const res = await fetch('/devices', {
    headers: { Authorization: `Bearer ${getToken()}` }
  });

  if (!res.ok) {
    console.error("Klarte ikke hente enheter");
    return [];
  }

  return await res.json();
}

document.addEventListener("DOMContentLoaded", async () => {
  const container = document.getElementById("settings-container");
  const devices = await fetchDevices();

  if (devices.length === 0) {
    container.textContent = "Ingen enheter funnet.";
    return;
  }

  devices.forEach(device => {
    const section = document.createElement("div");
    section.className = "device-section";

    const name = document.createElement("p");
    name.innerHTML = `<strong>Navn:</strong> ${device.name}`;

    const uuid = document.createElement("p");
    uuid.innerHTML = `<strong>ID:</strong> ${device.id}`;

    const cans = document.createElement("p");
    cans.innerHTML = `<strong>Antall vanntanker:</strong> ${device.num_cans}`;

    section.appendChild(name);
    section.appendChild(uuid);
    section.appendChild(cans);

    section.style.border = "1px solid #ccc";
    section.style.padding = "10px";
    section.style.marginBottom = "10px";

    container.appendChild(section);
  });
});
