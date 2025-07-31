import { getToken, getUsername, logout } from './auth.js';
import { getDashboardData } from './api.js';
import { plotDashboard } from './plot.js';

// Hvis ikke logget inn → redirect til login
if (!getToken()) {
  window.location.href = "/static/html/index.html";
} else {
  const username = getUsername();
  const userInfo = document.getElementById("user-info");
  if (username && userInfo) {
    userInfo.textContent = `Brukernavn: ${username}`;
  }
}

// Hent data når bruker klikker
document.getElementById("load-data").addEventListener("click", async () => {
  const start = new Date("2025-07-15T00:00:00Z").toISOString();
  const end = new Date().toISOString();

  const data = await getDashboardData(start, end, "sensor1", ["1", "2"]);
  plotDashboard(data);
});

// Logg ut-knapp
document.getElementById("logout").addEventListener("click", () => {
  logout(); // fjerner token
  window.location.href = "/static/html/index.html";
});
