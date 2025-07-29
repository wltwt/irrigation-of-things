import { login, getToken } from './auth.js';

// Hvis allerede logget inn → send bruker til dashboard
const token = getToken();
if (token) {
  window.location.href = '/static/html/dashboard.html';
}

// Håndter login-skjema
document.getElementById("login-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const username = document.getElementById("username").value;
  const password = document.getElementById("password").value;

  const success = await login(username, password);
  if (success) {
    window.location.href = '/static/html/dashboard.html';
  }
});
