// =============== VARS ==============
let fs_arr = [];
let fetching = null;
let fetch_name;
let fetch_index;
let fetch_file = '';
let fetch_path = '';
let fetch_tout;
let fetch_to_file = false;
let edit_idx = 0;

let uploading = null;
let upload_tout;
let upload_bytes = [];
let upload_size;
let ota_tout;

// ============ TIMEOUT ============
function stopFS() {
  if (fetching) post('fetch_stop', fetch_path);
  stop_fetch_tout();
  stop_upload_tout();
  stop_ota_tout();
  upload_bytes = [];
  fetching = null;
  uploading = null;
}
function stop_fetch_tout() {
  if (fetch_tout) clearTimeout(fetch_tout);
}
function reset_fetch_tout() {
  stop_fetch_tout();
  fetch_tout = setTimeout(() => {
    stopFS();
    if (!fetch_to_file) EL('process#' + fetch_index).innerHTML = 'Error!';
  }, tout_prd);
}
function stop_upload_tout() {
  if (upload_tout) clearTimeout(upload_tout);
}
function reset_upload_tout() {
  stop_upload_tout();
  upload_tout = setTimeout(() => {
    stopFS();
    EL('file_upload_btn').innerHTML = 'Error!';
    setTimeout(() => EL('file_upload_btn').innerHTML = 'Upload', 2000);
  }, tout_prd);
}
function stop_ota_tout() {
  if (ota_tout) clearTimeout(ota_tout);
}
function reset_ota_tout() {
  stop_ota_tout();
  ota_tout = setTimeout(() => {
    stopFS();
    EL('ota_label').innerHTML = 'Error!';
    setTimeout(() => EL('ota_label').innerHTML = 'IDLE', 3000);
  }, tout_prd);
}

// ============ FS BROWSER ============
function showFsbr(device) {
  fs_arr = [];
  for (let path in device.fs) fs_arr.push(path);
  fs_arr = sortPaths(fs_arr, '/');

  let inner = '';
  for (let i in fs_arr) {
    if (fs_arr[i].endsWith('/')) {
      inner += `<div class="fs_file fs_folder drop_area" onclick="file_upload_path.value='${fs_arr[i]}'/*;file_upload_btn.click()*/" ondrop="file_upload_path.value='${fs_arr[i]}';uploadFile(event.dataTransfer.files[0],'${fs_arr[i]}')">${fs_arr[i]}</div>`;
    } else {
      let none = "style='display:none'";
      inner += `<div class="fs_file" onclick="openFSctrl(${i})">${fs_arr[i]}<div class="fs_weight">${(device.fs[fs_arr[i]] / 1000).toFixed(2)} kB</div></div>
      <div id="fs#${i}" class="fs_controls">
        <button ${readModule(Modules.RENAME) ? '' : none} title="Rename" class="icon cfg_btn_tab" onclick="renameFile(${i})"></button>
        <button ${readModule(Modules.DELETE) ? '' : none} title="Delete" class="icon cfg_btn_tab" onclick="deleteFile(${i})"></button>
        <button ${readModule(Modules.DOWNLOAD) ? '' : none} title="Fetch" class="icon cfg_btn_tab" onclick="fetchFile(${i})"></button>
        <label id="process#${i}"></label>
        <a id="download#${i}" title="Download" class="icon cfg_btn_tab" href="" download="" style="display:none"></a>
        <button id="open#${i}" title="Open" class="icon cfg_btn_tab" onclick="openFile(EL('download#${i}').href)" style="display:none"></button>
        <button ${readModule(Modules.UPLOAD) ? '' : none} id="edit#${i}" title="Edit" class="icon cfg_btn_tab" onclick="editFile(EL('download#${i}').href,'${i}')" style="display:none"></button>
      </div>`;
    }
  }
  if (device.total) inner += `<div class="fs_info">Used ${(device.used / 1000).toFixed(2)}/${(device.total / 1000).toFixed(2)} kB [${Math.round(device.used / device.total * 100)}%]</div>`;
  else inner += `<div class="fs_info">Used ${(device.used / 1000).toFixed(2)} kB</div>`;
  EL('fsbr_inner').innerHTML = inner;
  let ota_t = '.' + devices[focused].ota_t;
  EL('ota_upload').accept = ota_t;
  EL('ota_upload_fs').accept = ota_t;
  EL('ota_url_f').value = "http://url_to_flash" + ota_t;
  EL('ota_url_fs').value = "http://url_to_filesystem" + ota_t;
}
function openFSctrl(i) {
  let current = EL(`fs#${i}`).style.display == 'flex';
  document.querySelectorAll('.fs_controls').forEach(el => el.style.display = 'none');
  if (!current) EL(`fs#${i}`).style.display = 'flex';
}
function renameFile(i) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return;
  }
  let path = fs_arr[i];
  let res = prompt('Rename ' + path + ' to', path);
  if (res && res != path) post('rename', path, res);
}
function deleteFile(i) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return;
  }
  let path = fs_arr[i];
  if (confirm('Delete ' + path + '?')) post('delete', path);
}
function fetchFile(i) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return;
  }
  EL('download#' + i).style.display = 'none';
  EL('open#' + i).style.display = 'none';
  EL('process#' + i).style.display = 'unset';
  EL('process#' + i).innerHTML = '';

  fetch_path = fs_arr[i];
  fetch_index = i;
  fetch_name = fetch_path.split('/').pop();
  fetch_to_file = false;
  if (devices_t[focused].conn == Conn.WS && devices_t[focused].http_cfg.download && fetch_path.startsWith(devices_t[focused].http_cfg.path)) fetchHTTP(fetch_path, fetch_name, fetch_index)
  else post('fetch', fetch_path);
}
function openFile(src) {
  let w = window.open();
  src =
    w.document.write('<iframe src="' + src + '" frameborder="0" style="border:0; top:0px; left:0px; bottom:0px; right:0px; width:100%; height:100%;" allowfullscreen></iframe>');
}
function format_h() {
  if (confirm('Format filesystem?')) post('format');
}
function updatefs_h() {
  post('fsbr');
}

function editFile(data, idx) {
  function b64ToText(base64) {
    const binString = atob(base64);
    return new TextDecoder().decode(Uint8Array.from(binString, (m) => m.codePointAt(0)));
  }

  EL('editor_area').value = b64ToText(data.split('base64,')[1]);//window.atob(data.split('base64,')[1]);
  EL('editor_area').scrollTop = 0;
  EL('edit_path').innerHTML = fs_arr[idx];
  EL('fsbr').style.display = 'none';
  EL('fsbr_edit').style.display = 'block';
  edit_idx = idx;
}
function editor_save() {
  editor_cancel();
  let div = fs_arr[edit_idx].lastIndexOf('/');
  let path = fs_arr[edit_idx].slice(0, div);
  let name = fs_arr[edit_idx].slice(div + 1);
  //EL('download#' + edit_idx).href = ('data:' + getMime(name) + ';base64,' + window.btoa(EL('editor_area').value));
  uploadFile(new File([EL('editor_area').value], name, { type: getMime(name), lastModified: new Date() }), path);
}
function editor_cancel() {
  EL('fsbr').style.display = 'block';
  EL('fsbr_edit').style.display = 'none';
}

function fetchHTTP(path, name, index) {
  fetching = focused;
  EL('process#' + index).innerHTML = '0%';
  var xhr = new XMLHttpRequest();
  xhr.responseType = 'blob';
  xhr.open('GET', 'http://' + devices[focused].ip + ':' + http_port + path);

  xhr.onprogress = function (e) {
    EL('process#' + index).innerHTML = Math.round(e.loaded * 100 / e.total) + '%';
  };
  xhr.onloadend = function (e) {
    if (e.loaded && e.loaded == e.total) {
      EL('process#' + index).innerHTML = '100%';
      var reader = new FileReader();
      reader.readAsDataURL(xhr.response);
      reader.onloadend = function () {
        fetchEnd(name, index, this.result.split('base64,')[1]);
      }
    } else {
      EL('process#' + index).innerHTML = 'Error';
      showPopupError('Error fetch ' + path);
    }
  }
  xhr.send();
}

function fetchEnd(name, index, data) {
  if (!fetching) return;
  EL('download#' + index).style.display = 'inline-block';
  EL('download#' + index).href = ('data:' + getMime(name) + ';base64,' + data);
  EL('download#' + index).download = name;
  EL('open#' + index).style.display = 'inline-block';
  EL('edit#' + index).style.display = 'inline-block';
  EL('process#' + index).style.display = 'none';
  fetching = null;
  stopFS();
}

// ============ UPLOAD =============
function uploadFile(file, path) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return clearFiles();
  }

  let reader = new FileReader();
  reader.readAsArrayBuffer(file);
  reader.onload = function (e) {
    if (!e.target.result) return clearFiles();

    let buffer = new Uint8Array(e.target.result);
    if (!path.startsWith('/')) path = '/' + path;
    if (!path.endsWith('/')) path += '/';
    path += file.name;
    if (!confirm('Upload ' + path + ' (' + buffer.length + ' bytes)?')) return clearFiles();

    if (devices_t[focused].conn == Conn.WS && devices_t[focused].http_cfg.upload) {
      EL('file_upload_btn').innerHTML = waiter(22, 'var(--font_inv)', false);
      let formData = new FormData();
      formData.append('upload', file, path);
      let xhr = new XMLHttpRequest();
      xhr.open("POST", 'http://' + devices[focused].ip + ':' + http_port + '/upload');
      xhr.onreadystatechange = function () {
        if (this.responseText == 'OK') showPopup('Done');
        if (this.responseText == 'FAIL') showPopupError('Error');
      }
      xhr.onload = function () {
        setLabelTout('file_upload_btn', 'Done', 'Upload');
        post('fsbr');
      }
      xhr.onerror = function () {
        setLabelTout('file_upload_btn', 'Error!', 'Upload');
      }
      xhr.send(formData);

    } else {
      upload_bytes = [];
      for (b of buffer) upload_bytes.push(b);
      upload_size = upload_bytes.length;
      post('upload', path);
    }
    clearFiles();
  }

}
function uploadNextChunk() {
  let i = 0;
  let data = '';
  while (true) {
    if (!upload_bytes.length) break;
    i++;
    data += String.fromCharCode(upload_bytes.shift());
    if (i >= devices[focused].max_upl * 3 / 4) break;
  }
  EL('file_upload_btn').innerHTML = Math.round((upload_size - upload_bytes.length) / upload_size * 100) + '%';
  post('upload_chunk', (upload_bytes.length) ? 'next' : 'last', window.btoa(data));
}
function clearFiles() {
  EL('file_upload').value = '';
  EL('ota_upload').value = '';
  EL('ota_upload_fs').value = '';
}

// ============== OTA ==============
function uploadOta(file, type) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return clearFiles();
  }
  if (!file.name.endsWith(devices[focused].ota_t)) {
    alert('Wrong file! Use .' + devices[focused].ota_t);
    return clearFiles();
  }
  if (!confirm('Upload OTA ' + type + '?')) return clearFiles();

  let reader = new FileReader();
  reader.readAsArrayBuffer(file);
  reader.onload = function (e) {
    if (!e.target.result) return clearFiles();

    if (devices_t[focused].conn == Conn.WS && devices_t[focused].http_cfg.ota) {
      EL('ota_label').innerHTML = waiter(25, 'var(--font)', false);
      let formData = new FormData();
      formData.append(type, file, file.name);
      let xhr = new XMLHttpRequest();
      xhr.open("POST", 'http://' + devices[focused].ip + ':' + http_port + '/ota?type=' + type);
      xhr.onreadystatechange = function () {
        if (this.responseText == 'OK') showPopup('Done');
        if (this.responseText == 'FAIL') showPopupError('Error');
      }
      xhr.onload = function () {
        setLabelTout('ota_label', 'DONE', 'IDLE');
      }
      xhr.onerror = function () {
        setLabelTout('ota_label', 'ERROR', 'IDLE');
      }
      xhr.send(formData);

    } else {
      let buffer = new Uint8Array(e.target.result);
      upload_bytes = [];
      for (b of buffer) upload_bytes.push(b);
      upload_size = upload_bytes.length;
      post('ota', type);
    }
    clearFiles();
  }
}
function otaNextChunk() {
  let i = 0;
  let data = '';
  while (true) {
    if (!upload_bytes.length) break;
    i++;
    data += String.fromCharCode(upload_bytes.shift());
    if (i >= devices[focused].max_upl * 3 / 4) break;
  }
  EL('ota_label').innerHTML = Math.round((upload_size - upload_bytes.length) / upload_size * 100) + '%';
  post('ota_chunk', (upload_bytes.length) ? 'next' : 'last', window.btoa(data));
}
function otaUrl(url, type) {
  post('ota_url', type, url);
  showPopup('OTA start');
}