// =============== VARS ==============
let fs_arr = [];
let fetching = null;
let fetch_name;
let fetch_index;
let fetch_file = '';
let fetch_tout;

let uploading = null;
let upload_tout;
let upload_bytes = [];
let upload_size;
let ota_tout;

// ============ TIMEOUT ============
function stopFS() {
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
    EL('process#' + fetch_index).innerHTML = 'Error!';
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
  Object.keys(device.fs).forEach(path => fs_arr.push(path));
  fs_arr = sortPaths(fs_arr, '/');

  let inner = '';
  for (let i in fs_arr) {
    if (fs_arr[i].endsWith('/')) {
      inner += `<div class="fs_file fs_folder" onclick="file_upload_path.value='${fs_arr[i]}'/*;file_upload_btn.click()*/">${fs_arr[i]}</div>`;
    } else {
      inner += `<div class="fs_file" onclick="openFSctrl(${i})">${fs_arr[i]}<div class="fs_weight">${(device.fs[fs_arr[i]] / 1000).toFixed(2)} kB</div></div>
      <div id="fs#${i}" class="fs_controls">
        <button title="Rename" class="icon cfg_btn_tab" onclick="renameFile(${i})"></button>
        <button title="Delete" class="icon cfg_btn_tab" onclick="deleteFile(${i})"></button>
        <button title="Fetch" class="icon cfg_btn_tab" onclick="fetchFile(${i})"></button>
        <label id="process#${i}"></label>
        <a id="download#${i}" title="Download" class="icon cfg_btn_tab" href="" download="" style="display:none"></a>
        <button id="open#${i}" title="Open" class="icon cfg_btn_tab" onclick="openFile(EL('download#${i}').href)" style="display:none"></button>
      </div>`;
    }
  }
  inner += `<div class="fs_info">Used ${(device.used / 1000).toFixed(2)} kB (${Math.round(device.used / device.total * 100)}%) from ${(device.total / 1000).toFixed(2)} kB (${((device.total - device.used) / 1000).toFixed(2)} kB free)</div>`;
  EL('fsbr_inner').innerHTML = inner;
  let accept = device.gzip ? '.gz' : '.bin';
  EL('ota_upload').accept = accept;
  EL('ota_upload_fs').accept = accept;
  EL('ota_url_f').value = "http://url_to_flash" + accept;
  EL('ota_url_fs').value = "http://url_to_filesystem" + accept;
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

  let path = fs_arr[i];
  fetch_index = i;
  fetch_name = path.split('/').pop();
  post('fetch', path);
}
function openFile(src) {
  let w = window.open();
  w.document.write('<iframe src="' + src + '" frameborder="0" style="border:0; top:0px; left:0px; bottom:0px; right:0px; width:100%; height:100%;" allowfullscreen></iframe>');
}
function format_h() {
  if (confirm('Format filesystem?')) post('format');
}

// ============ UPLOAD =============
function uploadFile(arg) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return;
  }
  
  let reader = new FileReader();
  reader.onload = function (e) {
    if (!e.target.result) return;
    let buffer = new Uint8Array(e.target.result);
    if (!confirm('Upload ' + file_upload_path.value + arg.files[0].name + ' (' + buffer.length + ' bytes)?')) return;
    upload_bytes = [];
    for (b of buffer) upload_bytes.push(b);
    upload_size = upload_bytes.length;
    post('upload', file_upload_path.value + arg.files[0].name);
    arg.value = null;
  }

  reader.readAsArrayBuffer(arg.files[0]);
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

// ============== OTA ==============
function uploadOta(arg, type) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return;
  }
  if (!confirm('Upload OTA ' + type + '?')) return;
  let reader = new FileReader();

  reader.onload = function (e) {
    if (!e.target.result) return;
    let buffer = new Uint8Array(e.target.result);
    upload_bytes = [];
    for (b of buffer) upload_bytes.push(b);
    upload_size = upload_bytes.length;
    post('ota', type);
    arg.value = null;
  }

  reader.readAsArrayBuffer(arg.files[0]);
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
}