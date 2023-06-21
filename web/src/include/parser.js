let push_timer = 0;

function applyUpdate(name, value) {
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
    release_all();log(confirms[name].label);
    let res = confirm(value ? value : confirms[name].label);
    set_h(name, res ? 1 : 0);
    return;
  }

  let el = EL('#' + name);
  if (!el) return;
  cl = el.classList;
  if (cl.contains('icon_t')) el.style.color = value;
  else if (cl.contains('text_t')) el.innerHTML = value;
  else if (cl.contains('input_t')) el.value = value;
  else if (cl.contains('date_t')) el.value = new Date(ctrl.value * 1000).toISOString().split('T')[0];
  else if (cl.contains('time_t')) el.value = new Date(ctrl.value * 1000).toISOString().split('T')[1].split('.')[0];
  else if (cl.contains('datetime_t')) el.value = new Date(ctrl.value * 1000).toISOString().split('.')[0];
  else if (cl.contains('slider_t')) el.value = value, EL('out#' + name).innerHTML = value, moveSlider(el, false);
  else if (cl.contains('switch_t')) el.checked = (value == '1');
  else if (cl.contains('select_t')) el.value = value;
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
  save_devices();
}
function compareDevice(mem, dev) {
  return mem.id != dev.id ||
    mem.name != dev.name ||
    mem.icon != dev.icon ||
    mem.PIN != dev.PIN ||
    mem.version != dev.version ||
    mem.max_upl != dev.max_upl;
}
function parseDevice(fromID, text, conn, ip = 'unset') {
  let device;
  try {
    device = JSON.parse(text.replaceAll("\'", "\""));
  } catch (e) {
    log('Wrong packet (JSON)');
    return;
  }

  let id = device.id;
  if (!id) return log('Wrong packet (ID)');
  if (fromID != 'broadcast' && fromID != id) return log('Wrong packet (Unknown ID)');
  if (fromID == 'broadcast' && device.type != 'discover' && device.type != 'update' && device.type != 'push' && device.type != 'print') return log('Wrong packet (error)');

  log('Got packet from #' + id + ' ' + device.type + ' via ' + ConnNames[conn]);

  if (id == focused) {
    stop_tout();
    showErr(false);
    change_conn(devices_t[focused].conn);
  }

  switch (device.type) {
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
        devices[id] = { prefix: cfg.prefix, break_widgets: false, show_names: false, ip: ip };
        updateDevice(devices[id], device);

        /*NON-ESP*/
        if (mq_state()) {
          mq_client.subscribe(devices[id].prefix + '/hub/' + id + '/get/#');
          mq_client.subscribe(devices[id].prefix + '/hub/' + cfg.hub_id + '/#');
          if (!mq_pref_list.includes(devices[id].prefix)) mq_pref_list.push(devices[id].prefix);
        }
        /*/NON-ESP*/
        addDevice(id);
      }

      if (!(id in devices_t)) {
        devices_t[id] = { conn: Conn.NONE, ws: null, controls: null, granted: false, buffer: { ws: '', mq: '', serial: '', bt: '' } };
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
      showControls(device.controls, id);
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
    case 'fetch_err':
      if (id != focused) return;

      EL('process#' + fetch_index).innerHTML = 'Aborted';
      showPopupError('Fetch aborted');
      stopFS();
      break;

    case 'fetch_start':
      if (id != focused) return;

      fetching = focused;
      fetch_file = '';
      post('fetch_chunk');
      reset_fetch_tout();
      break;

    case 'fetch_next_chunk':
      if (id != fetching) return;

      fetch_file += device.data;
      if (device.chunk == device.amount - 1) {
        EL('download#' + fetch_index).style.display = 'unset';
        EL('download#' + fetch_index).href = 'data:' + getMime(fetch_name) + ';base64,' + fetch_file;
        EL('download#' + fetch_index).download = fetch_name;
        EL('open#' + fetch_index).style.display = 'unset';
        EL('process#' + fetch_index).style.display = 'none';
        stopFS();
      } else {
        EL('process#' + fetch_index).innerHTML = Math.round(device.chunk / device.amount * 100) + '%';
        post('fetch_chunk');
        reset_fetch_tout();
      }
      break;

    // ============= UPLOAD =============
    case 'upload_err':
      showPopupError('Upload aborted');
      EL('file_upload_btn').innerHTML = 'Error!';
      setTimeout(() => EL('file_upload_btn').innerHTML = 'Upload', 2000);
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
      EL('file_upload_btn').innerHTML = 'Done!';
      setTimeout(() => EL('file_upload_btn').innerHTML = 'Upload', 2000);
      post('fsbr');
      break;

    // ============= OTA =============
    case 'ota_err':
      showPopupError('Ota aborted');
      EL('ota_label').innerHTML = 'Error!';
      setTimeout(() => EL('ota_label').innerHTML = '', 3000);
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
      EL('ota_label').innerHTML = 'Done!';
      setTimeout(() => EL('ota_label').innerHTML = '', 3000);
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
function showControls(controls) {
  EL('controls').style.visibility = 'hidden';
  EL('controls').innerHTML = '';
  if (!controls) return;
  gauges = {};
  canvases = {};
  pickers = {};
  joys = {};
  prompts = {};
  confirms = {};
  dup_names = [];
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
      case 'widget_b': beginWidgets(ctrl); break;
      case 'widget_e': endWidgets(); break;
      case 'canvas': addCanvas(ctrl); break;
      case 'gauge': addGauge(ctrl); break;
      case 'image': addImage(ctrl); break;
      case 'stream': addStream(ctrl); break;
      case 'joy': addJoy(ctrl); break;
      case 'js': eval(ctrl.value); break;
      case 'confirm': confirms[ctrl.name] = {label: ctrl.label}; break;
      case 'prompt': prompts[ctrl.name] = {label: ctrl.label, value: ctrl.value}; break;
      case 'menu': addMenu(ctrl);
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

  setTimeout(() => {
    if (dup_names.length) showPopupError('Duplicated names: ' + dup_names);
    showCanvases();
    showGauges();
    showPickers();
    showJoys();
    EL('controls').style.visibility = 'visible';
  }, 10);

  if (!gauges.length && !canvases.length && !pickers.length && !joys.length) EL('controls').style.visibility = 'visible';
}
function showInfo(device) {
  EL('info_lib_v').innerHTML = device.info[0];
  EL('info_firm_v').innerHTML = device.info[1];
  if (device.info.length == 2) return;
  let count = 2;
  for (let id in info_labels_esp) {
    EL(id).innerHTML = device.info[count];
    count++;
  }
  let ver = devices[focused].version;
  let info_v = EL('info_firm_v');
  if (ver.includes('@')) {
    let link = 'https://github.com/' + ver.split('@')[0];
    info_v.onclick = function () { openURL(link); };
    info_v.classList.add('info_link');
    info_v.title = link;
  } else {
    info_v.onclick = function () { };
    info_v.classList.remove('info_link');
    info_v.title = '';
  }
}