const fallbackLat = -30.0346;
const fallbackLon = -51.2177;

function normalizeNumberInput(id) {
  let value = document.getElementById(id).value;
  // Convert comma -> dot
  value = value.replace(",", ".");
  document.getElementById(id).value = value;
  return parseFloat(value);
}

function validateLatLon(lat, lon) {
  if (isNaN(lat) || lat < -90 || lat > 90) {
    alert("Invalid Latitude! Should be between -90 e +90.");
    return false;
  }
  if (isNaN(lon) || lon < -180 || lon > 180) {
    alert("Invalid Longitude! Should be between -180 e +180.");
    return false;
  }
  return true;
}

function applyDefaultLocation() {
  const select = document.getElementById("predefined_location");
  const option = select.options[select.selectedIndex];
  const status = document.getElementById("geo_status");

  const lat = parseFloat(option.getAttribute("data-lat"));
  const lon = parseFloat(option.getAttribute("data-lon"));

  if (!isNaN(lat) && !isNaN(lon)) {
    document.getElementById("latitude").value = lat;
    document.getElementById("longitude").value = lon;
    status.innerText = "Applied Localization: " + option.textContent;
  } else {
    status.innerText = "";
  }
}

function loadConfig() {
  fetch("/api/config")
    .then(res => res.json())
    .then(cfg => {
      document.getElementById("wifi_ssid").value = cfg.wifi_ssid || "";
      document.getElementById("wifi_pass").value = cfg.wifi_password || "";
      document.getElementById("latitude").value = cfg.latitude ?? fallbackLat;
      document.getElementById("longitude").value = cfg.longitude ?? fallbackLon;
    });
}

function loadStatus() {
    fetch('/api/status')
      .then(r => {
        if (!r.ok) throw new Error('Status fetch failed');
        return r.json();
      })
      .then(obj => {
        document.getElementById('stat_fw').innerText = obj.fw_version || 'n/a';
        document.getElementById('stat_build').innerText = obj.fw_build_date || 'n/a';
        document.getElementById('stat_chip').innerText = obj.chip_model || 'n/a';
        document.getElementById('stat_flash').innerText = obj.flash_size_bytes || 'n/a';
        document.getElementById('stat_chipid').innerText = obj.chip_id || 'n/a';
        document.getElementById('stat_mac').innerText = obj.mac_address || 'n/a';
        document.getElementById('stat_heap').innerText = obj.free_heap || 'n/a';
      })
      .catch(err => {
        console.warn('Failed loadStatus:', err);
        // optionally hide status card or show error
      });
  }
  
function saveConfig() {
  const lat = normalizeNumberInput("latitude");
  const lon = normalizeNumberInput("longitude");

  if (!validateLatLon(lat, lon)) return;

  const cfg = {
    wifi_ssid: document.getElementById("wifi_ssid").value,
    wifi_password: document.getElementById("wifi_pass").value,
    latitude: lat,
    longitude: lon
  };

  fetch("/api/config", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(cfg)
  })
    .then(res => res.json())
    .then(() => alert("Configuration saved successfully.!"))
    .catch(() => alert("Failed to save configuration"));
}

window.onload = function() {
    loadStatus();
    loadConfig();
};
  