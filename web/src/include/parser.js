let push_timer = 0;
let prev_set = null;

function applyUpdate(name, value) {
  if (screen != 'device') return;
  if (prev_set && prev_set.name == name && prev_set.value == value) {
    prev_set = null;
    return;
  }
  if (name in prompts) {
    release_all();
    let res = prompt(value ? value : prompts[name].label, prompts[name].value);
    if (res !== null) {
      prompts[name].value = res;
      set_h(name, res);
    }
    return;
  }
  if (name in confirms) {
    release_all();
    let res = confirm(value ? value : confirms[name].label);
    set_h(name, res ? 1 : 0);
    return;
  }
  if (name in pickers) {
    pickers[name].setColor(intToCol(value));
    return;
  }

  let el = EL('#' + name);
  if (!el) return;
  cl = el.classList;
  if (cl.contains('icon_t')) el.style.color = value;
  else if (cl.contains('text_t')) el.innerHTML = value;
  else if (cl.contains('input_t')) el.value = value;
  else if (cl.contains('date_t')) el.value = new Date(value * 1000).toISOString().split('T')[0];
  else if (cl.contains('time_t')) el.value = new Date(value * 1000).toISOString().split('T')[1].split('.')[0];
  else if (cl.contains('datetime_t')) el.value = new Date(value * 1000).toISOString().split('.')[0];
  else if (cl.contains('slider_t')) el.value = value, EL('out#' + name).innerHTML = value, moveSlider(el, false);
  else if (cl.contains('switch_t')) el.checked = (value == '1');
  else if (cl.contains('select_t')) el.value = value;
  else if (cl.contains('image_t')) {
    files.push({ id: '#' + name, path: (value ? value : EL('#' + name).getAttribute("name")), type: 'img' });
    EL('wlabel#' + name).innerHTML = ' [0%]';
    if (files.length == 1) nextFile();
  }
  else if (cl.contains('canvas_t')) {
    if (name in canvases) {
      if (!canvases[name].value) {
        canvases[name].value = value;
        drawCanvas(canvases[name]);
      }
    }
  }
  else if (cl.contains('gauge_t')) {
    if (name in gauges) {
      gauges[name].value = Number(value);
      drawGauge(gauges[name]);
    }
  }
  else if (cl.contains('flags_t')) {
    let flags = document.getElementById('#' + name).getElementsByTagName('input');
    let val = value;
    for (let i = 0; i < flags.length; i++) {
      flags[i].checked = val & 1;
      val >>= 1;
    }
  }
}
function updateDevice(mem, dev) {
  mem.id = dev.id;
  mem.name = dev.name;
  mem.icon = dev.icon;
  mem.PIN = dev.PIN;
  mem.version = dev.version;
  mem.max_upl = dev.max_upl;
  mem.modules = dev.modules;
  mem.ota_t = dev.ota_t;
  save_devices();
}
function compareDevice(mem, dev) {
  return mem.id != dev.id ||
    mem.name != dev.name ||
    mem.icon != dev.icon ||
    mem.PIN != dev.PIN ||
    mem.version != dev.version ||
    mem.max_upl != dev.max_upl ||
    mem.modules != dev.modules ||
    mem.ota_t != dev.ota_t;
}
function parseDevice(fromID, text, conn, ip = 'unset') {
  let device;
  text = text.trim().replaceAll(/([^\\])\\([^\"\\nrt])/ig, "$1\\\\$2").replaceAll(/\t/ig, "\\t").replaceAll(/\n/ig, "\\n").replaceAll(/\r/ig, "\\r");

  try {
    device = JSON.parse(text);
  } catch (e) {
    log('Wrong packet (JSON):' + text);
    return;
  }

  let id = device.id;
  if (!id) return log('Wrong packet (ID)');
  if (fromID != 'broadcast' && fromID != id) return log('Wrong packet (Unknown ID)');
  if (fromID == 'broadcast' && device.type != 'discover' && device.type != 'update' && device.type != 'push' && device.type != 'print' && device.type != 'data') return log('Wrong packet (error)');

  log('Got packet from #' + id + ' ' + device.type + ' via ' + ConnNames[conn]);

  if (id == focused) {
    stop_tout();
    showErr(false);
    change_conn(devices_t[focused].conn);
  }

  switch (device.type) {
    case 'data':
      // RAW DATA
      break;

    case 'alert':
      release_all();
      alert(device.text);
      break;

    case 'notice':
      showPopup(device.text, intToCol(device.color));
      break;

    case 'OK':
      break;

    case 'ERR':
      showPopupError(device.text);
      break;

    case 'discover':
      if (focused) return;

      // COMPATIBILITY
      if (device.modules == undefined) device.modules = 0;
      if (device.ota_t == undefined) device.ota_t = 'bin';
      //

      if (id in devices) {
        let upd = false;
        if (compareDevice(devices[id], device)) upd = true;

        if (devices[id].ip == 'unset' && ip != 'unset') {
          devices[id].ip = ip;
          upd = true;
        }
        if (upd) {
          log('Update device #' + id);
          updateDevice(devices[id], device);
          /*NON-ESP*/
          if (device.icon.length) EL(`icon#${id}`).innerHTML = device.icon;
          /*/NON-ESP*/
          EL(`name#${id}`).innerHTML = device.name ? device.name : 'Unknown';
        }
      } else {
        log('Add new device #' + id);
        devices[id] = { prefix: hub.cfg.prefix, break_widgets: false, show_names: false, ip: ip };
        updateDevice(devices[id], device);

        /*NON-ESP*/
        if (mq_state()) {
          mq_client.subscribe(devices[id].prefix + '/hub/' + id + '/get/#');
          mq_client.subscribe(devices[id].prefix + '/hub/' + hub.cfg.client_id + '/#');
          if (!mq_pref_list.includes(devices[id].prefix)) mq_pref_list.push(devices[id].prefix);
        }
        /*/NON-ESP*/
        addDevice(id);
      }

      if (!(id in devices_t)) {
        devices_t[id] = {
          conn: Conn.NONE, ws: null, controls: null, granted: false,
          buffer: { WS: '', MQTT: '', Serial: '', BT: '' },
          port: null,
          http_cfg: { upd: 0, upload: 0, download: 0, ota: 0, path: '/fs/' }
        };
      }

      EL(`device#${id}`).className = "device";
      EL(`${ConnNames[conn]}#${id}`).style.display = 'unset';
      if (conn < devices_t[id].conn) devices_t[id].conn = conn;
      break;

    case 'print':
      if (id != focused) return;
      printCLI(device.text, device.color);
      break;

    case 'update':
      if (id != focused) return;
      if (!(id in devices)) return;
      for (let name in device.updates) applyUpdate(name, device.updates[name]);
      break;

    case 'ui':
      if (id != focused) return;
      devices_t[id].controls = device.controls;
      showControls(device.controls, false, conn, devices[focused].ip);
      break;

    case 'info':
      if (id != focused) return;
      showInfo(device);
      break;

    case 'push':
      if (!(id in devices)) return;
      let date = (new Date).getTime();
      if (date - push_timer < 3000) return;
      push_timer = date;
      showNotif(device.text, devices[id].name);
      break;

    // ============== FS ==============
    case 'fsbr':
      if (id != focused) return;
      showFsbr(device);
      break;

    case 'fs_error':
      if (id != focused) return;
      EL('fsbr_inner').innerHTML = '<div class="fs_err">FS ERROR</div>';
      break;

    // ============= FETCH =============
    case 'fetch_start':
      if (id != focused) return;

      fetching = focused;
      fetch_file = '';
      post('fetch_chunk', fetch_path);
      reset_fetch_tout();
      break;

    case 'fetch_next_chunk':
      if (id != fetching) return;

      fetch_file += device.data;
      if (device.chunk == device.amount - 1) {
        if (fetch_to_file) downloadFileEnd(fetch_file);
        else fetchEnd(fetch_name, fetch_index, fetch_file);
      } else {
        let perc = Math.round(device.chunk / device.amount * 100);
        if (fetch_to_file) processFile(perc);
        else EL('process#' + fetch_index).innerHTML = perc + '%';
        post('fetch_chunk', fetch_path);
        reset_fetch_tout();
      }
      break;

    case 'fetch_err':
      if (id != focused) return;

      if (fetch_to_file) errorFile();
      else EL('process#' + fetch_index).innerHTML = 'Aborted';
      showPopupError('Fetch aborted');
      stopFS();
      break;

    // ============= UPLOAD =============
    case 'upload_err':
      showPopupError('Upload aborted');
      setLabelTout('file_upload_btn', 'Error!', 'Upload');
      stopFS();
      break;

    case 'upload_start':
      if (id != focused) return;
      uploading = focused;
      uploadNextChunk();
      reset_upload_tout();
      break;

    case 'upload_next_chunk':
      if (id != uploading) return;
      uploadNextChunk();
      reset_upload_tout();
      break;

    case 'upload_end':
      showPopup('Upload Done!');
      stopFS();
      setLabelTout('file_upload_btn', 'Done!', 'Upload');
      post('fsbr');
      break;

    // ============= OTA =============
    case 'ota_err':
      showPopupError('Ota aborted');
      setLabelTout('ota_label', 'ERROR', 'IDLE');
      stopFS();
      break;

    case 'ota_start':
      if (id != focused) return;
      uploading = focused;
      otaNextChunk();
      reset_ota_tout();
      break;

    case 'ota_next_chunk':
      if (id != uploading) return;
      otaNextChunk();
      reset_ota_tout();
      break;

    case 'ota_end':
      showPopup('OTA Done! Reboot');
      stopFS();
      setLabelTout('ota_label', 'DONE', 'IDLE');
      break;

    // ============ OTA URL ============
    case 'ota_url_ok':
      showPopup('OTA Done!');
      break;

    case 'ota_url_err':
      showPopupError('OTA Error!');
      break;
  }
}
function showControls(controls, from_buffer = false, conn = Conn.NONE, ip = 'unset') {
  EL('controls').style.visibility = 'hidden';
  EL('controls').innerHTML = '';
  if (!controls) return;
  oninp_buffer = {};
  gauges = {};
  canvases = {};
  pickers = {};
  joys = {};
  prompts = {};
  confirms = {};
  dup_names = [];
  files = [];
  wid_row_count = 0;
  btn_row_count = 0;
  wid_row_id = null;
  btn_row_id = null;

  for (ctrl of controls) {
    if (devices[focused].show_names && ctrl.name) ctrl.label = ctrl.name;
    ctrl.wlabel = ctrl.label ? ctrl.label : ctrl.type;
    ctrl.clabel = (ctrl.label && ctrl.label != '_no') ? ctrl.label : ctrl.type;
    ctrl.clabel = ctrl.clabel.charAt(0).toUpperCase() + ctrl.clabel.slice(1);

    switch (ctrl.type) {
      case 'button': addButton(ctrl); break;
      case 'button_i': addButtonIcon(ctrl); break;
      case 'spacer': addSpace(ctrl); break;
      case 'tabs': addTabs(ctrl); break;
      case 'title': addTitle(ctrl); break;
      case 'led': addLED(ctrl); break;
      case 'label': addLabel(ctrl); break;
      case 'icon': addIcon(ctrl); break;
      case 'input': addInput(ctrl); break;
      case 'pass': addPass(ctrl); break;
      case 'slider': addSlider(ctrl); break;
      case 'sliderW': addSliderW(ctrl); break;
      case 'switch': addSwitch(ctrl); break;
      case 'switch_i': addSwitchIcon(ctrl); break;
      case 'switch_t': addSwitchText(ctrl); break;
      case 'date': addDate(ctrl); break;
      case 'time': addTime(ctrl); break;
      case 'datetime': addDateTime(ctrl); break;
      case 'select': addSelect(ctrl); break;
      case 'week': addWeek(ctrl); break;
      case 'color': addColor(ctrl); break;
      case 'spinner': addSpinner(ctrl); break;
      case 'display': addDisplay(ctrl); break;
      case 'html': addHTML(ctrl); break;
      case 'flags': addFlags(ctrl); break;
      case 'log': addLog(ctrl); break;
      case 'row_b': case 'widget_b': beginWidgets(ctrl); break;
      case 'row_e': case 'widget_e': endWidgets(); break;
      case 'canvas': addCanvas(ctrl); break;
      case 'gauge': addGauge(ctrl); break;
      case 'image': addImage(ctrl); break;
      case 'stream': addStream(ctrl, conn, ip); break;
      case 'dpad': case 'joy': addJoy(ctrl); break;
      case 'js': eval(ctrl.value); break;
      case 'confirm': confirms[ctrl.name] = { label: ctrl.label }; break;
      case 'prompt': prompts[ctrl.name] = { label: ctrl.label, value: ctrl.value }; break;
      case 'menu': addMenu(ctrl); break;
      case 'table': addTable(ctrl); break;
    }
  }
  if (devices[focused].show_names) {
    let labels = document.querySelectorAll(".widget_label");
    for (let lbl of labels) lbl.classList.add('widget_label_name');
  }

  resizeFlags();
  moveSliders();
  scrollDown();
  resizeSpinners();
  renderElms(from_buffer);
}
async function renderElms(from_buffer) {
  while (1) {
    await waitAnimationFrame();
    let end = 1;
    for (let i in gauges) if (EL('#' + i) == null) end = 0;
    for (let i in canvases) if (EL('#' + i) == null) end = 0;
    for (let i in joys) if (EL('#' + i) == null) end = 0;
    for (let i in pickers) if (EL('#' + i) == null) end = 0;

    if (end) {
      if (dup_names.length) showPopupError('Duplicated names: ' + dup_names);
      showCanvases();
      showGauges();
      showPickers();
      showJoys();
      EL('controls').style.visibility = 'visible';
      if (!from_buffer) nextFile();
      break;
    }
  }
}

function showInfo(device) {
  function addInfo(el, label, value, title = '') {
    EL(el).innerHTML += `
    <div class="cfg_row info">
      <label>${label}</label>
      <label title="${title}" class="lbl_info">${value}</label>
    </div>`;
  }
  EL('info_version').innerHTML = '';
  EL('info_net').innerHTML = '';
  EL('info_memory').innerHTML = '';
  EL('info_system').innerHTML = '';

  for (let i in device.info.version) addInfo('info_version', i, device.info.version[i]);
  for (let i in device.info.net) addInfo('info_net', i, device.info.net[i]);
  for (let i in device.info.memory) {
    if (typeof (device.info.memory[i]) == 'object') {
      let used = device.info.memory[i][0];
      let total = device.info.memory[i][1];
      let mem = (used / 1000).toFixed(1) + ' kB';
      if (total) mem += ' [' + (used / total * 100).toFixed(0) + '%]';
      addInfo('info_memory', i, mem, `Total ${(total / 1000).toFixed(1)} kB`);
    } else addInfo('info_memory', i, device.info.memory[i]);
  }
  for (let i in device.info.system) {
    if (i == 'Uptime') {
      let sec = device.info.system[i];
      let upt = Math.floor(sec / 86400) + ':' + new Date(sec * 1000).toISOString().slice(11, 19);
      let d = new Date();
      let utc = d.getTime() - (d.getTimezoneOffset() * 60000);
      addInfo('info_system', i, upt);
      addInfo('info_system', 'Started', new Date(utc - sec * 1000).toISOString().split('.')[0].replace('T', ' '));
      continue;
    }
    addInfo('info_system', i, device.info.system[i]);
  }
}
function setLabelTout(el, text1, text2) {
  EL(el).innerHTML = text1;
  setTimeout(() => EL(el).innerHTML = text2, 3000);
}