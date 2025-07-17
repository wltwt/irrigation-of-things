import { login } from './auth.js';
import { getSecureData, getSoilData } from './api.js';
import { plotData } from './plot.js';

document.getElementById("login-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const username = document.getElementById("username").value;
  const password = document.getElementById("password").value;

  const success = await login(username, password);
  if (success) {
    document.getElementById("load-data").style.display = "inline";
  }
});

document.getElementById("load-data").addEventListener("click", async () => {
  const start = new Date("2025-07-15T00:00:00Z").toISOString();
  //const end = new Date("2025-07-17T00:00:00Z").toISOString();
  const end = new Date().toISOString();
  
  const data = await getSoilData(start, end);
  plotData(data);
});
