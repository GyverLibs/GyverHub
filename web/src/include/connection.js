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
let set_tout = null;
let oninput_tout = null;
let refresh_ui = false;

let s_port = null, s_state = false, s_reader = null, s_buf = '';

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
  if (cmd == 'set' && !readModule(Modules.SET)) return;
  if (cmd == 'set') {
    if (set_tout) clearTimeout(set_tout);
    prev_set = { name: name, value: value };
    set_tout = setTimeout(() => { set_tout = prev_set = null; }, tout_prd);
  }
  let id = focused;
  cmd = cmd.toString();
  name = name.toString();
  value = value.toString();
  let uri0 = devices[id].prefix + '/' + id + '/' + hub.cfg.client_id + '/' + cmd;
  let uri = uri0;
  if (name) uri += '/' + name;
  if (value) uri += '=' + value;

  switch (devices_t[id].conn) {
    case Conn.SERIAL:
      serial_send(uri);
      break;

    case Conn.BT:
      bt_send(uri);
      break;

    case Conn.WS:
      ws_send(id, uri);
      break;
    /*NON-ESP*/
    case Conn.MQTT:
      mq_send(uri0 + (name.length ? ('/' + name) : ''), value);
      break;
    /*/NON-ESP*/
  }
  reset_ping();
  reset_tout();
  log('Post to #' + id + ' via ' + ConnNames[devices_t[id].conn] + ', cmd=' + cmd + (name ? (', name=' + name) : '') + (value ? (', value=' + value) : ''))
}
function release_all() {
  if (pressId) post('set', pressId, 0);
  pressId = null;
}
function click_h(name, dir) {
  pressId = (dir == 1) ? name : null;
  post('set', name, dir);
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
      if (screen == 'info') post('info');
      else if (screen == 'fsbr') post('fsbr');
      else post('focus');
    } else {
      post('ping');
    }
  }, ping_prd);
}
function parsePacket(id, text, conn) {
  function checkPacket() {
    if (devices_t[id]['buffer'][ConnNames[conn]].endsWith('}\n')) {
      if (devices_t[id]['buffer'][ConnNames[conn]].startsWith('\n{')) parseDevice(id, devices_t[id]['buffer'][ConnNames[conn]], conn);
      devices_t[id]['buffer'][ConnNames[conn]] = '';
    }
  }

  if (conn == Conn.BT || conn == Conn.SERIAL) {
    for (let i = 0; i < text.length; i++) {
      devices_t[id]['buffer'][ConnNames[conn]] += text[i];
      checkPacket();
    }
  } else {
    devices_t[id]['buffer'][ConnNames[conn]] += text;
    checkPacket();
  }
}

// =============== MQTT ================
/*NON-ESP*/
function mq_start() {
  if (mq_client && mq_client.connected) return;
  if (!hub.cfg.mq_host || !hub.cfg.mq_port) return;

  const url = 'wss://' + hub.cfg.mq_host + ':' + hub.cfg.mq_port + '/mqtt';
  const options = {
    keepalive: 60,
    clientId: 'HUB-' + Math.round(Math.random() * 0xffffffff).toString(16),
    username: hub.cfg.mq_login,
    password: hub.cfg.mq_pass,
    protocolId: 'MQTT',
    protocolVersion: 4,
    clean: true,
    reconnectPeriod: 1000,
    connectTimeout: 20 * 1000
  }

  try {
    mq_client = mqtt.connect(url, options);
  } catch (e) {
    mq_show_icon(0);
    return;
  }

  mq_client.on('connect', function () {
    mq_show_icon(1);
    mq_client.subscribe(hub.cfg.prefix + '/hub');

    mq_pref_list = [hub.cfg.prefix];
    mq_client.subscribe(hub.cfg.prefix + '/hub/' + hub.cfg.client_id + '/#');

    for (let id in devices) {
      if (!mq_pref_list.includes(devices[id].prefix)) {
        mq_client.subscribe(devices[id].prefix + '/hub/' + hub.cfg.client_id + '/#');
        mq_pref_list.push(devices[id].prefix);
      }
      mq_client.subscribe(devices[id].prefix + '/hub/' + id + '/get/#');
    }

    if (mq_discover_flag) {
      mq_discover_flag = false;
      mq_discover();
    }
  });

  mq_client.on('error', function () {
    mq_show_icon(0);
    mq_client.end();
  });

  mq_client.on('close', function () {
    mq_show_icon(0);
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
      } else if (topic.startsWith(pref + '/hub/' + hub.cfg.client_id + '/')) {
        let id = topic.split('/').slice(-1);
        if (!(id in devices) || !(id in devices_t)) {
          parseDevice(id, text, Conn.MQTT);
          return;
        }

        parsePacket(id, text, Conn.MQTT);

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
  else for (let id in devices) {
    mq_send(devices[id].prefix + '/' + id, hub.cfg.client_id);
  }
  log('MQTT discover');
}
function mq_discover_all() {
  if (!mq_state()) return;
  if (!(hub.cfg.prefix in mq_pref_list)) {
    mq_client.subscribe(hub.cfg.prefix + '/hub');
    mq_pref_list.push(hub.cfg.prefix);
    mq_client.subscribe(hub.cfg.prefix + '/hub/' + hub.cfg.client_id + '/#');
  }
  mq_send(hub.cfg.prefix, hub.cfg.client_id);
  log('MQTT discover all');
}
function mq_show_icon(state) {
  EL('mqtt_ok').style.display = state ? 'inline-block' : 'none';
}
/*/NON-ESP*/

// ============= WEBSOCKET ==============
function ws_start(id) {
  if (!hub.cfg.use_ws) return;
  checkHTTP(id);
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
    parsePacket(id, event.data, Conn.WS);
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
  for (let id in devices) {
    if (devices[id].ip == 'unset') continue;
    ws_discover_ip(devices[id].ip, id);
    log('WS discover');
  }
}
function ws_discover_ip(ip, id = 'broadcast') {
  let ws = new WebSocket(`ws://${ip}:${ws_port}/`, ['hub']);
  let tout = setTimeout(() => {
    if (ws) ws.close();
  }, ws_tout);
  ws.onopen = () => ws.send(hub.cfg.prefix + (id != 'broadcast' ? ('/' + id) : '') + '\0');
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
  if (hub.cfg.use_hook) http_hook(ip_arr);
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
function checkHTTP(id) {
  if (devices_t[id].http_cfg.upd) return;

  let xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      devices_t[id].http_cfg.upd = 1;
      let resp;
      try {
        resp = JSON.parse(this.responseText);
      } catch (e) {
        return;
      }
      for (let i in resp) {
        if (resp[i]) devices_t[id].http_cfg[i] = resp[i];
      }
    }
  }
  xhr.timeout = tout_prd;
  xhr.open("GET", 'http://' + devices[id].ip + ':' + http_port + '/hub_http_cfg');
  xhr.send();
}

/*NON-ESP*/
// ================ SERIAL ================
async function serial_select() {
  await serial_stop();
  const ports = await navigator.serial.getPorts();
  for (let port of ports) await port.forget();
  try {
    await navigator.serial.requestPort();
  } catch (e) {
  }
  serial_change();
}
async function serial_discover() {
  serial_send(hub.cfg.prefix);
}
async function serial_start() {
  try {
    s_state = true;
    const ports = await navigator.serial.getPorts();
    if (!ports.length) return;
    s_port = ports[0];
    await s_port.open({ baudRate: hub.cfg.ser_baud });

    log('[Serial] Open');
    serial_show_icon(true);
    if (s_buf) {
      setTimeout(function () {
        serial_send(s_buf);
        s_buf = '';
      }, 2000);
    }

    while (s_port.readable) {
      s_reader = s_port.readable.getReader();
      let buffer = '';
      try {
        while (true && s_state) {
          const { value, done } = await s_reader.read();
          if (done) break;
          const data = new TextDecoder().decode(value);
          if (focused) {
            parsePacket(focused, data, Conn.SERIAL);
          } else {
            buffer += data;
            if (buffer.endsWith("}\n")) {
              parseDevice('broadcast', buffer, Conn.SERIAL);
              buffer = '';
            }
          }
        }
      } catch (error) {
        log("[Serial] " + error);
      } finally {
        await s_reader.releaseLock();
        await s_port.close();
        log('[Serial] Close');
        break;
      }
    }
  } catch (error) {
    log("[Serial] " + error);
  }
  s_reader = null;
  s_state = false;
  serial_show_icon(false);
}
async function serial_send(text) {
  if (!s_state) {
    serial_start();
    s_buf = text;
    return;
  }
  try {
    const encoder = new TextEncoder();
    const writer = s_port.writable.getWriter();
    await writer.write(encoder.encode(text + '\0'));
    writer.releaseLock();
  } catch (e) {
    log("[Serial] " + e);
  }
}
async function serial_stop() {
  if (s_reader) s_reader.cancel();
  s_state = false;
}
function serial_toggle() {
  if (s_state) serial_stop();
  else serial_start();
}
function serial_show_icon(state) {
  EL('serial_ok').style.display = state ? 'inline-block' : 'none';
}
async function serial_change() {
  serial_show_icon(0);
  if (s_state) await serial_stop();
  const ports = await navigator.serial.getPorts();
  EL('serial_btn').style.display = ports.length ? 'inline-block' : 'none';
}

// ================ BT ================
function bt_discover() {
  hub.bt.send(hub.cfg.prefix);
}
function bt_toggle() {
  if (!hub.bt.state()) {
    hub.bt.open();
    EL('bt_device').innerHTML = 'Connecting...';
  } else hub.bt.close();
}
function bt_send(text) {
  hub.bt.send(text);
}
function bt_show_ok(state) {
  EL('bt_ok').style.display = state ? 'inline-block' : 'none';
}
hub.bt.onopen = function () {
  EL('bt_btn').innerHTML = 'Disconnect';
  EL('bt_device').innerHTML = hub.bt.getName();
  bt_show_ok(true);
}
hub.bt.onclose = function () {
  EL('bt_btn').innerHTML = 'Connect';
  EL('bt_device').innerHTML = 'Not Connected';
  bt_show_ok(false);
}
hub.bt.onerror = function (e) {
  EL('bt_device').innerHTML = 'Not Connected';
  bt_show_ok(false);
}

let bt_buffer = '';
hub.bt.onmessage = function (data) {
  if (focused) {
    parsePacket(focused, data, Conn.BT);
  } else {
    bt_buffer += data;
    if (bt_buffer.endsWith("}\n")) {
      parseDevice('broadcast', bt_buffer, Conn.BT);
      bt_buffer = '';
    }
  }
}
/*/NON-ESP*/

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
  for (let id in devices) {
    if (devices[id].ip == 'unset') continue;
    http_send(devices[id].ip, devices[id].prefix + '/' + id, id);
  };
  log('HTTP discover');
}
function http_discover_ip(ip) {
  http_send(ip, hub.cfg.prefix);
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