// =============== VARS ==============
let fs_arr = [];
let fetching = null;
let fetch_name;
let fetch_index;
let fetch_file = '';
let fetch_tout;
let fetch_to_file = false;

let uploading = null;
let upload_tout;
let upload_bytes = [];
let upload_size;
let ota_tout;

// ============ TIMEOUT ============
function stopFS() {
  if (fetching) {post('fetch_stop');log(13123123123)}
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
      inner += `<div class="fs_file fs_folder" onclick="file_upload_path.value='${fs_arr[i]}'/*;file_upload_btn.click()*/">${fs_arr[i]}</div>`;
    } else {
      let none = "style='display:none'";
      inner += `<div class="fs_file" onclick="openFSctrl(${i})">${fs_arr[i]}<div class="fs_weight">${(device.fs[fs_arr[i]] / 1000).toFixed(2)} kB</div></div>
      <div id="fs#${i}" class="fs_controls">
        <button ${readModule(Modules.RENAME) ? '' : none} title="Rename" class="icon cfg_btn_tab" onclick="renameFile(${i})"></button>
        <button ${readModule(Modules.DELETE) ? '' : none} title="Delete" class="icon cfg_btn_tab" onclick="deleteFile(${i})"></button>
        <button ${readModule(Modules.DOWNLOAD) ? '' : none} title="Fetch" class="icon cfg_btn_tab" onclick="fetchFile(${i})"></button>
        <label id="process#${i}"></label>
        <a id="download#${i}" title="Download" class="icon cfg_btn_tab" href="" download="" style="display:none"></a>
        <button id="open#${i}" title="Open" class="icon cfg_btn_tab" onclick="openFile(EL('download#${i}').href)" style="display:none"></button>
      </div>`;
    }
  }
  inner += `<div class="fs_info">Used ${(device.used / 1000).toFixed(2)} / ${(device.total / 1000).toFixed(2)} kB [${Math.round(device.used / device.total * 100)}%]</div>`;
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

  let path = fs_arr[i];
  fetch_index = i;
  fetch_name = path.split('/').pop();
  fetch_to_file = false;
  if (devices_t[focused].conn == Conn.WS && devices_t[focused].http_cfg.download && path.startsWith(devices_t[focused].http_cfg.path)) fetchHTTP(path, fetch_name, fetch_index)
  else post('fetch', path);
}
function openFile(src) {
  let w = window.open();
  w.document.write('<iframe src="' + src + '" frameborder="0" style="border:0; top:0px; left:0px; bottom:0px; right:0px; width:100%; height:100%;" allowfullscreen></iframe>');
}
function format_h() {
  if (confirm('Format filesystem?')) post('format');
}

/*
function fetchHTTP(path, name, index) {
  EL('process#' + index).innerHTML = 'Fetching...';
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    var reader = new FileReader();
    reader.onloadend = function () {
      fetchEnd(name, index, reader.result);
    }
    reader.readAsDataURL(xhr.response);
  };
  xhr.open('GET', 'http://' + devices[focused].ip + ':' + http_port + path);
  xhr.responseType = 'blob';
  xhr.send();
}
*/
async function fetchHTTP(path, name, index) {
  EL('process#' + index).innerHTML = 'Fetching...';
  fetching = focused;
  const response = await fetch('http://' + devices[focused].ip + ':' + http_port + path, { cache: "no-store" });
  const blob = await response.blob();
  return new Promise(() => {
    try {
      const reader = new FileReader();
      reader.readAsDataURL(blob);
      reader.onload = function () {
        fetchEnd(name, index, this.result.split('base64,')[1]);
      };
    } catch (e) {
    }
  });
}

function fetchEnd(name, index, data) {
  if (!fetching) return;
  EL('download#' + index).style.display = 'unset';
  EL('download#' + index).href = ('data:' + getMime(name) + ';base64,' + data);
  EL('download#' + index).download = name;
  EL('open#' + index).style.display = 'unset';
  EL('process#' + index).style.display = 'none';
  fetching = null;
  stopFS();
}

// ============ UPLOAD =============
function uploadFile(arg) {
  if (fetching || uploading) {
    showPopupError('Busy');
    return;
  }

  let reader = new FileReader();
  reader.readAsArrayBuffer(arg.files[0]);

  reader.onload = function (e) {
    if (!e.target.result) return;
    let buffer = new Uint8Array(e.target.result);
    let path = file_upload_path.value;
    if (!path.startsWith('/')) path = '/' + path;
    if (!path.endsWith('/')) path += '/';
    path += arg.files[0].name;
    if (!confirm('Upload ' + path + ' (' + buffer.length + ' bytes)?')) return;

    if (devices_t[focused].conn == Conn.WS && devices_t[focused].http_cfg.upload) {
      EL('file_upload_btn').innerHTML = 'Wait...';
      let xhr = new XMLHttpRequest();
      let formData = new FormData();
      formData.append('upload', arg.files[0], path);
      console.log(formData);
      xhr.onreadystatechange = function () {
        if (this.responseText == 'OK') showPopup('Done');
        if (this.responseText == 'FAIL') showPopupError('Error');
      }
      xhr.onload = () => {
        setLabelTout('file_upload_btn', 'Done', 'Upload');
        post('fsbr');
      }
      xhr.onerror = () => {
        setLabelTout('file_upload_btn', 'Error!', 'Upload');
      }
      xhr.open("POST", 'http://' + devices[focused].ip + ':' + http_port + '/upload');
      xhr.send(formData);

    } else {
      upload_bytes = [];
      for (b of buffer) upload_bytes.push(b);
      upload_size = upload_bytes.length;
      post('upload', path);
    }

    arg.value = null;
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

    if (devices_t[focused].http_cfg.ota) {
      EL('ota_label').innerHTML = 'WAIT...';
      let xhr = new XMLHttpRequest();
      let formData = new FormData();
      formData.append('ota', arg.files[0], type);
      xhr.onreadystatechange = function () {
        if (this.responseText == 'OK') showPopup('Done');
        if (this.responseText == 'FAIL') showPopupError('Error');
      }
      xhr.onload = () => {
        setLabelTout('ota_label', 'DONE', 'IDLE');
      }
      xhr.onerror = () => {
        setLabelTout('ota_label', 'ERROR', 'IDLE');
      }

      xhr.open("POST", 'http://' + devices[focused].ip + ':' + http_port + '/ota');
      xhr.send(formData);

    } else {
      let buffer = new Uint8Array(e.target.result);
      upload_bytes = [];
      for (b of buffer) upload_bytes.push(b);
      upload_size = upload_bytes.length;
      post('ota', type);
    }
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
  showPopup('OTA start');
}