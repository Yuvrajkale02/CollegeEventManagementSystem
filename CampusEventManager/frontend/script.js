// ===========================================================
// Campus Circular - frontend logic
// Talks to the C++ backend (httplib server) over a small REST API.
// The backend is expected to run on the SAME origin this page is
// served from (http://localhost:8080). If you open this file
// directly (file://) instead of via the server, set API_BASE below.
// ===========================================================
const API_BASE = ""; // same-origin; use e.g. "http://localhost:8080" if needed

let currentEvents = [];
let isAdminLoggedIn = false;

// ---------------- Navigation ----------------
document.querySelectorAll(".nav-btn").forEach(btn => {
  btn.addEventListener("click", () => switchView(btn.dataset.view));
});

function switchView(view) {
  document.querySelectorAll(".nav-btn").forEach(b => b.classList.toggle("active", b.dataset.view === view));
  document.querySelectorAll(".view").forEach(v => v.classList.remove("active"));
  document.getElementById("view-" + view).classList.add("active");
  if (view === "home") loadEvents();
}

// ---------------- Home: load & render events ----------------
async function loadEvents() {
  const category = document.getElementById("categoryFilter").value;
  const url = category ? `${API_BASE}/api/events?category=${encodeURIComponent(category)}` : `${API_BASE}/api/events`;

  try {
    const res = await fetch(url);
    currentEvents = await res.json();
    renderEventBoard(currentEvents);
  } catch (err) {
    document.getElementById("eventBoard").innerHTML =
      `<div class="board-empty">Could not reach the server. Make sure the C++ backend is running on port 8080.</div>`;
  }
}

function renderEventBoard(events) {
  const board = document.getElementById("eventBoard");
  if (!events.length) {
    board.innerHTML = `<div class="board-empty">No notices posted yet. Check back soon.</div>`;
    return;
  }

  board.innerHTML = events.map(e => {
    const full = e.seatsAvailable <= 0;
    return `
      <article class="ticket">
        <div class="ticket-top">
          <span class="ticket-category">${escapeHtml(e.category)}</span>
          <h3>${escapeHtml(e.title)}</h3>
          <p class="ticket-desc">${escapeHtml(e.description)}</p>
          <div class="ticket-meta">
            <div>&#128197; ${escapeHtml(e.eventDate)}</div>
            <div>&#128205; ${escapeHtml(e.venue || "Venue TBA")}</div>
          </div>
        </div>
        <div class="ticket-perforation"></div>
        <div class="ticket-bottom">
          <span class="seats-tag ${full ? "seats-full" : ""}">
            ${full ? "Seats full" : `<strong>${e.seatsAvailable}</strong> / ${e.totalSeats} seats left`}
          </span>
          <button class="ghost-btn" ${full ? "disabled" : ""} onclick="openRegisterModal(${e.eventId}, '${escapeAttr(e.title)}')">
            ${full ? "Full" : "Register"}
          </button>
        </div>
      </article>`;
  }).join("");
}

document.getElementById("categoryFilter").addEventListener("change", loadEvents);
document.getElementById("refreshBtn").addEventListener("click", loadEvents);

// ---------------- Registration modal ----------------
function openRegisterModal(eventId, title) {
  document.getElementById("regEventId").value = eventId;
  document.getElementById("registerEventTitle").textContent = "Register — " + title;
  document.getElementById("registerForm").reset();
  document.getElementById("regEventId").value = eventId;
  document.getElementById("registerMsg").textContent = "";
  document.getElementById("registerModal").classList.add("open");
}
document.getElementById("closeRegisterModal").addEventListener("click", () => {
  document.getElementById("registerModal").classList.remove("open");
});

document.getElementById("registerForm").addEventListener("submit", async (e) => {
  e.preventDefault();
  const msg = document.getElementById("registerMsg");
  msg.textContent = "Submitting...";
  msg.className = "form-msg";

  const payload = {
    eventId: parseInt(document.getElementById("regEventId").value, 10),
    studentName: document.getElementById("regStudentName").value,
    rollNo: document.getElementById("regRollNo").value,
    department: document.getElementById("regDepartment").value,
    email: document.getElementById("regEmail").value
  };

  try {
    const res = await fetch(`${API_BASE}/api/register`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    });
    const data = await res.json();
    if (res.ok) {
      msg.textContent = data.message;
      msg.className = "form-msg success";
      loadEvents();
      setTimeout(() => document.getElementById("registerModal").classList.remove("open"), 1200);
    } else {
      msg.textContent = data.error;
      msg.className = "form-msg error";
    }
  } catch (err) {
    msg.textContent = "Network error — is the backend running?";
    msg.className = "form-msg error";
  }
});

// ---------------- Admin: login ----------------
document.getElementById("adminLoginForm").addEventListener("submit", async (e) => {
  e.preventDefault();
  const msg = document.getElementById("loginMsg");
  msg.textContent = "Signing in...";
  msg.className = "form-msg";

  const payload = {
    username: document.getElementById("adminUsername").value,
    password: document.getElementById("adminPassword").value
  };

  try {
    const res = await fetch(`${API_BASE}/api/admin/login`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    });
    const data = await res.json();
    if (res.ok) {
      isAdminLoggedIn = true;
      document.getElementById("adminLoginSection").classList.add("hidden");
      document.getElementById("adminDashboard").classList.remove("hidden");
      loadAdminEvents();
      loadRegistrations();
    } else {
      msg.textContent = data.error;
      msg.className = "form-msg error";
    }
  } catch (err) {
    msg.textContent = "Network error — is the backend running?";
    msg.className = "form-msg error";
  }
});

document.getElementById("logoutBtn").addEventListener("click", () => {
  isAdminLoggedIn = false;
  document.getElementById("adminDashboard").classList.add("hidden");
  document.getElementById("adminLoginSection").classList.remove("hidden");
  document.getElementById("adminLoginForm").reset();
});

// ---------------- Admin: post / edit event ----------------
const eventForm = document.getElementById("eventForm");
eventForm.addEventListener("submit", async (e) => {
  e.preventDefault();
  const msg = document.getElementById("eventFormMsg");
  msg.textContent = "Saving...";
  msg.className = "form-msg";

  const id = document.getElementById("eventId").value;
  const payload = {
    title: document.getElementById("eventTitle").value,
    description: document.getElementById("eventDescription").value,
    category: document.getElementById("eventCategory").value,
    eventDate: document.getElementById("eventDate").value,
    venue: document.getElementById("eventVenue").value,
    totalSeats: parseInt(document.getElementById("eventSeats").value, 10)
  };

  try {
    const res = await fetch(`${API_BASE}/api/events` + (id ? "/" + id : ""), {
      method: id ? "PUT" : "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    });
    const data = await res.json();
    if (res.ok) {
      msg.textContent = data.message;
      msg.className = "form-msg success";
      resetEventForm();
      loadAdminEvents();
      loadRegistrations();
    } else {
      msg.textContent = data.error;
      msg.className = "form-msg error";
    }
  } catch (err) {
    msg.textContent = "Network error — is the backend running?";
    msg.className = "form-msg error";
  }
});

function resetEventForm() {
  eventForm.reset();
  document.getElementById("eventId").value = "";
  document.getElementById("eventFormTitle").textContent = "Post a new notice";
  document.getElementById("eventFormSubmit").textContent = "Post notice";
  document.getElementById("cancelEditBtn").classList.add("hidden");
}
document.getElementById("cancelEditBtn").addEventListener("click", resetEventForm);

async function loadAdminEvents() {
  const res = await fetch(`${API_BASE}/api/events`);
  const events = await res.json();
  const list = document.getElementById("adminEventList");

  if (!events.length) {
    list.innerHTML = `<p class="hint">No notices posted yet.</p>`;
  } else {
    list.innerHTML = events.map(e => `
      <div class="admin-row">
        <div class="admin-row-info">
          <strong>${escapeHtml(e.title)}</strong>
          <span>${escapeHtml(e.eventDate)} &middot; ${e.seatsFilled}/${e.totalSeats} registered</span>
        </div>
        <div class="admin-row-actions">
          <button class="icon-btn" onclick="editEvent(${e.eventId})">Edit</button>
          <button class="icon-btn danger" onclick="deleteEvent(${e.eventId})">Delete</button>
        </div>
      </div>`).join("");
  }

  // Keep the registration filter dropdown in sync
  const filter = document.getElementById("regEventFilter");
  const selected = filter.value;
  filter.innerHTML = `<option value="">All events</option>` +
    events.map(e => `<option value="${e.eventId}">${escapeHtml(e.title)}</option>`).join("");
  filter.value = selected;
}

function editEvent(id) {
  const e = currentAdminEvents.find(ev => ev.eventId === id);
  if (!e) return;
  document.getElementById("eventId").value = e.eventId;
  document.getElementById("eventTitle").value = e.title;
  document.getElementById("eventDescription").value = e.description;
  document.getElementById("eventCategory").value = e.category;
  document.getElementById("eventDate").value = e.eventDate;
  document.getElementById("eventVenue").value = e.venue;
  document.getElementById("eventSeats").value = e.totalSeats;
  document.getElementById("eventFormTitle").textContent = "Edit notice";
  document.getElementById("eventFormSubmit").textContent = "Save changes";
  document.getElementById("cancelEditBtn").classList.remove("hidden");
}

async function deleteEvent(id) {
  if (!confirm("Delete this notice? This also removes its registrations.")) return;
  const res = await fetch(`${API_BASE}/api/events/${id}`, { method: "DELETE" });
  if (res.ok) {
    loadAdminEvents();
    loadRegistrations();
  } else {
    const data = await res.json();
    alert(data.error);
  }
}

// keep a cache so editEvent() can look up full event details
let currentAdminEvents = [];
const _origLoadAdminEvents = loadAdminEvents;
loadAdminEvents = async function () {
  const res = await fetch(`${API_BASE}/api/events`);
  currentAdminEvents = await res.json();
  await _origLoadAdminEvents();
};

// ---------------- Admin: registrations ----------------
document.getElementById("regEventFilter").addEventListener("change", loadRegistrations);

async function loadRegistrations() {
  const eventId = document.getElementById("regEventFilter").value;
  const url = eventId ? `${API_BASE}/api/registrations?eventId=${eventId}` : `${API_BASE}/api/registrations`;
  const res = await fetch(url);
  const regs = await res.json();
  const tbody = document.querySelector("#regTable tbody");

  if (!regs.length) {
    tbody.innerHTML = `<tr><td colspan="5" class="hint">No registrations yet.</td></tr>`;
    return;
  }
  tbody.innerHTML = regs.map(r => `
    <tr>
      <td>${escapeHtml(r.studentName)}</td>
      <td>${escapeHtml(r.rollNo)}</td>
      <td>${escapeHtml(r.department)}</td>
      <td>${escapeHtml(r.email)}</td>
      <td>${escapeHtml(r.regDate)}</td>
    </tr>`).join("");
}

// ---------------- Helpers ----------------
function escapeHtml(str) {
  if (str === undefined || str === null) return "";
  return String(str).replace(/[&<>"']/g, c => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;"
  }[c]));
}
function escapeAttr(str) {
  return String(str).replace(/'/g, "\\'");
}

// ---------------- Init ----------------
loadEvents();
