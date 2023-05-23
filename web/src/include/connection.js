const http_port = 80;
const ws_port = 81;
const tout_prd = 2800;
const ping_prd = 3000;
const oninput_prd = 100;
const ws_tout = 4000;

// =============== VARS ==============
let discovering = false;
let mq_client;
let mq_discover_flag = false;
let mq_pref_list = [];
let ws_focus_flag = false;
let tout_interval = null;
let ping_interval = null;
let oninput_tout = null;
let refresh_ui = false;
let oninp_buffer = {};

const Conn = {
  SERIAL: 0,
  BT: 1,
  WS: 2,
  MQTT: 3,
  NONE: 4,
  ERROR: 5,
};
const ConnNames = ['Serial', 'BT', 'WS', 'MQTT', 'None', 'Error'];

// ============== SEND ================
function post(cmd, name = '', value = '') {
  if (!focused) return;
  let id = focused;
  cmd = cmd.toString();
  name = name.toString();
  value = value.toString();
  let uri0 = devices[id].prefix + '/' + id + '/' + cfg.hub_id + '/' + cmd;
  let uri = uri0;
  if (name) uri += '/' + name;
  if (value) uri += '=' + value;

  switch (devices_t[id].conn) {
    case Conn.SERIAL:
      break;

    case Conn.BT:
      break;

    case Conn.WS:
      ws_send(id, uri);
      break;

    case Conn.MQTT:
      mq_send(uri0 + (name.length ? ('/' + name) : ''), value);
      break;
  }
  reset_ping();
  reset_tout();
  log('Post to #' + id + ' via ' + ConnNames[devices_t[id].conn] + ', cmd=' + cmd + (name ? (', name=' + name) : '') + (value ? (', value=' + value) : ''))
}
function click_h(name, dir) {
  pressId = (dir == 1) ? name : null;
  post('click', name, dir);
  reset_ping();
}
function set_h(name, value = '') {
  post('set', name, value);
  reset_ping();
}
function input_h(name, value) {
  if (!(name in oninp_buffer)) oninp_buffer[name] = { 'value': null, 'tout': null };

  if (!oninp_buffer[name].tout) {
    set_h(name, value);

    oninp_buffer[name].tout = setTimeout(() => {
      if (oninp_buffer[name].value != null && !tout_interval) set_h(name, oninp_buffer[name].value);
      oninp_buffer[name].tout = null;
      oninp_buffer[name].value = null;
    }, oninput_prd);
  } else {
    oninp_buffer[name].value = value;
  }
}
function reboot_h() {
  post('reboot');
}

// ============== TIMEOUT =============
function change_conn(conn) {
  EL('conn').innerHTML = ConnNames[conn];
}

function stop_tout() {
  refreshSpin(false);
  if (tout_interval) clearTimeout(tout_interval);
  tout_interval = null;
}
function reset_tout() {
  if (tout_interval) return;
  refreshSpin(true);
  tout_interval = setTimeout(function () {
    log('Connection lost');
    refresh_ui = true;
    change_conn(Conn.ERROR);
    showErr(true);
    stop_tout();
  }, tout_prd);
}

function stop_ping() {
  if (ping_interval) clearInterval(ping_interval);
  ping_interval = null;
}
function reset_ping() {
  stop_ping();
  ping_interval = setInterval(() => {
    if (refresh_ui) {
      refresh_ui = false;
      post('focus');
    } else {
      post('ping');
    }
  }, ping_prd);
}

// =============== MQTT ================
/*NON-ESP*/
function mq_start() {
  if (mq_client && mq_client.connected) return;
  if (!cfg.mq_host || !cfg.mq_port) return;

  const url = 'wss://' + cfg.mq_host + ':' + cfg.mq_port + '/mqtt';
  const options = {
    keepalive: 60,
    clientId: 'HUB-' + Math.round(Math.random() * 0xffffffff).toString(16),
    username: cfg.mq_login,
    password: cfg.mq_pass,
    protocolId: 'MQTT',
    protocolVersion: 4,
    clean: true,
    reconnectPeriod: 1000,
    connectTimeout: 20 * 1000
  }

  try {
    mq_client = mqtt.connect(url, options);
  } catch (e) {
    showPopupError('MQTT error');
    log('MQTT error');
    mq_show_err(1);
    return;
  }

  mq_client.on('connect', function () {
    showPopup('MQTT connected');
    log('MQTT connected');
    mq_show_err(0);
    mq_client.subscribe(cfg.prefix + '/hub');

    mq_pref_list = [cfg.prefix];
    mq_client.subscribe(cfg.prefix + '/hub/' + cfg.hub_id + '/#');

    Object.keys(devices).forEach(id => {
      if (!mq_pref_list.includes(devices[id].prefix)) {
        mq_client.subscribe(devices[id].prefix + '/hub/' + cfg.hub_id + '/#');
        mq_pref_list.push(devices[id].prefix);
      }
      mq_client.subscribe(devices[id].prefix + '/hub/' + id + '/get/#');
    });

    if (mq_discover_flag) {
      mq_discover_flag = false;
      mq_discover();
    }
  });

  mq_client.on('error', function () {
    showPopupError('MQTT error');
    log('MQTT error');
    mq_show_err(1);
    mq_client.end();
  });

  mq_client.on('close', function () {
    showPopupError('MQTT closed');
    log('MQTT closed');
    mq_show_err(1);
    mq_client.end();
  });

  mq_client.on('message', function (topic, text) {
    topic = topic.toString();
    text = text.toString();
    for (pref of mq_pref_list) {
      // prefix/hub
      if (topic == (pref + '/hub')) {
        parseDevice('broadcast', text, Conn.MQTT);

        // prefix/hub/hubid/id
      } else if (topic.startsWith(pref + '/hub/' + cfg.hub_id + '/')) {
        let id = topic.split('/').slice(-1);
        if (!(id in devices)) {
          parseDevice(id, text, Conn.MQTT);
          return;
        }

        let st = text.startsWith('\n{');
        let end = text.endsWith('}\n');
        if (st && end) parseDevice(id, text, Conn.MQTT);
        else if (st) {
          devices_t[id].buffer.mq = text;
        } else if (end) {
          log('End MQTT UI chunk #' + id);
          devices_t[id].buffer.mq += text;
          parseDevice(id, devices_t[id].buffer.mq, Conn.MQTT);
        } else {
          devices_t[id].buffer.mq += text;
        }

        // prefix/hub/id/get/name
      } else if (topic.startsWith(pref + '/hub/') && topic.includes('/get/')) {
        let idname = topic.split(pref + '/hub/')[1].split('/get/');
        if (idname[0] != focused || idname.length != 2) return;
        log('Got GET from id=' + idname[0] + ', name=' + idname[1] + ', value=' + text);
        applyUpdate(idname[1], text);
        stop_tout();
      } else {
        log('Got MQTT unknown');
      }
    }
  });
}
function mq_stop() {
  if (mq_state()) mq_client.end();
}
function mq_send(topic, msg = '') {
  if (mq_state()) mq_client.publish(topic, msg);  // no '\0'
}
function mq_state() {
  return (mq_client && mq_client.connected);
}
function mq_discover() {
  if (!mq_state()) mq_discover_flag = true;
  else Object.keys(devices).forEach(id => {
    mq_send(devices[id].prefix + '/' + id, cfg.hub_id);
  });
  log('MQTT discover');
}
function mq_discover_all() {
  if (!mq_state()) return;
  if (!(cfg.prefix in mq_pref_list)) {
    mq_client.subscribe(cfg.prefix + '/hub');
    mq_pref_list.push(cfg.prefix);
  }
  mq_send(cfg.prefix, cfg.hub_id);
  log('MQTT discover all');
}
function mq_show_err(state) {
  EL('head_err').style.display = (state && cfg.use_mqtt) ? 'unset' : 'none';
}
/*/NON-ESP*/

// ============= WEBSOCKET ==============
function ws_start(id) {
  if (!cfg.use_ws) return;
  if (devices_t[id].ws) return;
  if (devices[id].ip == 'unset') return;
  log(`WS ${id} open...`);

  devices_t[id].ws = new WebSocket(`ws://${devices[id].ip}:${ws_port}/`, ['hub']);

  devices_t[id].ws.onopen = function () {
    log(`WS ${id} opened`);
    if (ws_focus_flag) {
      ws_focus_flag = false;
      post('focus');
    }
    if (id != focused) devices_t[id].ws.close();
  };

  devices_t[id].ws.onclose = function () {
    log(`WS ${id} closed`);
    devices_t[id].ws = null;
    ws_focus_flag = false;
    if (id == focused) setTimeout(() => ws_start(id), 500);
  };

  devices_t[id].ws.onerror = function () {
    log(`WS ${id} error`);
  };

  devices_t[id].ws.onmessage = function (event) {
    reset_tout();
    let st = event.data.startsWith('\n{');
    let end = event.data.endsWith('}\n');
    if (st && end) parseDevice(id, event.data, Conn.WS);
    else if (st) {
      devices_t[id].buffer.ws = event.data;
    } else if (end) {
      log('End WS UI chunk #' + id);
      devices_t[id].buffer.ws += event.data;
      parseDevice(id, devices_t[id].buffer.ws, Conn.WS);
    } else {
      devices_t[id].buffer.ws += event.data;
    }
  };
}
function ws_stop(id) {
  if (!devices_t[id].ws || devices_t[id].ws.readyState >= 2) return;
  log(`WS ${id} close...`);
  devices_t[id].ws.close();
}
function ws_state(id) {
  return (devices_t[id].ws && devices_t[id].ws.readyState == 1);
}

function ws_send(id, text) {
  if (ws_state(id)) devices_t[id].ws.send(text.toString() + '\0');   // no '\0'
}
function ws_discover() {
  Object.keys(devices).forEach(id => {
    if (devices[id].ip == 'unset') return;
    ws_discover_ip(devices[id].ip, id);
    log('WS discover');
  });
}
function ws_discover_ip(ip, id = 'broadcast') {
  let ws = new WebSocket(`ws://${ip}:${ws_port}/`, ['hub']);
  let tout = setTimeout(() => {
    if (ws) ws.close();
  }, ws_tout);
  ws.onopen = () => ws.send(cfg.prefix + (id != 'broadcast' ? ('/' + id) : '') + '\0');
  ws.onerror = () => ws.close();
  ws.onclose = () => ws = null;
  ws.onmessage = function (event) {
    clearTimeout(tout);
    parseDevice(id, event.data, Conn.WS, ip);
    ws.close();
  };
}
function ws_discover_ips(ips) {
  discovering = true;
  refreshSpin(true);
  setTimeout(() => {
    refreshSpin(false);
    discovering = false;
  }, ws_tout / 200 * ips.length);

  for (let i in ips) {
    setTimeout(() => ws_discover_ip(ips[i]), ws_tout / 200 * i);
  }
  log('WS discover all');
}
function http_hook(ips) {
  discovering = true;
  refreshSpin(true);
  setTimeout(() => {
    refreshSpin(false);
    discovering = false;
  }, ws_tout);

  function hook(ip) {
    try {
      let xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          if (this.responseText == 'OK') ws_discover_ip(ip);
        }
      }
      xhr.timeout = tout_prd;
      xhr.open("GET", 'http://' + ip + ':' + http_port + '/hub_discover_all');
      xhr.send();
    } catch (e) {
    }
  }

  for (let i in ips) {
    setTimeout(() => hook(ips[i]), 10 * i);
  }
  log('WS hook discover all');
}
function ws_discover_all() {
  let ip_arr = getIPs();
  if (ip_arr == null) return;
  if (cfg.use_hook) http_hook(ip_arr);
  else ws_discover_ips(ip_arr);
}
function manual_ws_h(ip) {
  if (!checkIP(ip)) {
    showPopupError('Wrong IP!');
    return;
  }
  log('WS manual ' + ip);
  ws_discover_ip(ip);
  back_h();
}

// ================= HTTP =================
/*
function http_send(ip, uri, id = 'broadcast') {
  try {
    let xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) parseDevice(id, this.responseText, Conn.HTTP, ip);
    }
    xhr.timeout = tout_prd;
    xhr.open("GET", 'http://' + ip + ':' + http_port + '/' + uri);
    xhr.send();
  } catch (e) {
  }
}
function http_discover() {
  Object.keys(devices).forEach(id => {
    if (devices[id].ip == 'unset') return;
    http_send(devices[id].ip, devices[id].prefix + '/' + id, id);
  });
  log('HTTP discover');
}
function http_discover_ip(ip) {
  http_send(ip, cfg.prefix);
}
function http_discover_all() {
  let ips = getIPs();
  if (ips == null) return;
  discovering = true;
  refreshSpin(true);
  setTimeout(() => {
    refreshSpin(false);
    discovering = false;
  }, 10 * ips.length + tout_prd);

  for (let i in ips) {
    setTimeout(() => http_discover_ip(ips[i]), 10 * i);
  }
  log('HTTP discover all');
}
function manual_http_h(ip) {
  if (!checkIP(ip)) {
    showPopupError('Wrong IP!');
    return;
  }
  log('HTTP manual ' + ip);
  http_discover_ip(ip);
  back_h();
}
*/