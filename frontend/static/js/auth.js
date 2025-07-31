export async function login(username, password) {
  const res = await fetch("/auth/login", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: new URLSearchParams({ username, password })
  });

  const data = await res.json();

  if (res.ok) {
    localStorage.setItem("token", data.access_token);
    localStorage.setItem("username", username);
    return true;
  } else {
    alert("Feil brukernavn eller passord");
    return false;
  }
}

export function getToken() {
  return localStorage.getItem("token");
}

export function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("username");
}

export function getUsername() {
  return localStorage.getItem("username");
}