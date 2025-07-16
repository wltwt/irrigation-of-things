import { login } from './auth.js';
import { getSecureData } from './api.js';
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
  const data = await getSecureData();
  plotData(data);
});
