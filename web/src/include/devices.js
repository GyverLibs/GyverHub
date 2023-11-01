// ============== VARS =================
let devices = {};
let devices_t = {};
let controls = {};
let focused = null;
let touch = 0;
let pressId = null;
let dup_names = [];
let gauges = {};
let canvases = {};
let pickers = {};
let joys = {};
let prompts = {};
let confirms = {};
let oninp_buffer = {};
let files = [];

let wid_row_id = null;
let wid_row_count = 0;
let wid_row_size = 0;
let btn_row_id = null;
let btn_row_count = 0;
let dis_scroll_f = false;

function waiter(size = 50, col = 'var(--prim)', block = true) {
  return `<div class="waiter ${block ? 'waiter_b' : ''}"><span style="font-size:${size}px;color:${col}" class="icon spinning"></span></div>`;
}


const Modules = {
  INFO: (1 << 0),
  FSBR: (1 << 1),
  FORMAT: (1 << 2),
  DOWNLOAD: (1 << 3),
  UPLOAD: (1 << 4),
  OTA: (1 << 5),
  OTA_URL: (1 << 6),
  REBOOT: (1 << 7),
  SET: (1 << 8),
  READ: (1 << 9),
  DELETE: (1 << 10),
  RENAME: (1 << 11)
};

// ============ SAVE/LOAD ==============
function save_devices() {
  localStorage.setItem('devices', JSON.stringify(devices));
}
function load_devices() {
  if (localStorage.hasOwnProperty('devices')) {
    devices = JSON.parse(localStorage.getItem('devices'));

  }
}

// ============== DEVICE ===============
function readModule(module) {
  return !(devices[focused].modules & module);
}
function addDevice(id) {
  let icon = (!isESP() && devices[id].icon.length) ? `<span class="icon icon_min" id="icon#${id}">${devices[id].icon}</span>` : '';
  EL('devices').innerHTML += `<div class="device offline" id="device#${id}" onclick="device_h('${id}')" title="${id} [${devices[id].prefix}]">
  <div class="device_inner">
    <div class="d_icon ${icon ? '' : 'd_icon_empty'}">${icon}</div>
      <div class="d_head">
        <span><span class="d_name" id="name#${id}">${devices[id].name}</span><sup class="conn_dev" id="Serial#${id}">S</sup><sup class="conn_dev" id="BT#${id}">B</sup><sup class="conn_dev" id="WS#${id}">W</sup><sup class="conn_dev" id="MQTT#${id}">M</sup></span>
      </div>
      <div class="icon d_delete" onclick="delete_h('${id}')"></div>
    </div>
  </div>`;
}

// ============ COMPONENTS =============
// button
function addButton(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  if (wid_row_id) {
    let label = ctrl.wlabel, icon = '';
    if (ctrl.wlabel.charCodeAt(0) >= 0xF005) {
      icon = label[0];
      if (isESP()) icon = "";
      label = label.slice(1).trim();
    }
    endButtons();
    let inner = renderButton(ctrl.name, 'icon btn_icon', ctrl.name, icon, ctrl.size * 3, ctrl.color, true);
    addWidget(ctrl.tab_w, ctrl.name, label, inner);
  } else {
    if (!btn_row_id) beginButtons();
    let label = ctrl.clabel, icon = '';
    if (ctrl.clabel.charCodeAt(0) >= 0xF005) {
      icon = label[0];
      label = label.slice(1).trim();
      label = `<span class="icon icon_min">${icon}</span>&nbsp;` + label;
    }
    EL(btn_row_id).innerHTML += `${renderButton(ctrl.name, 'c_btn', ctrl.name, label, ctrl.size, ctrl.color, false)}`;
  }
}
function addButtonIcon(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  if (isESP()) ctrl.label = "";
  if (wid_row_id) {
    endButtons();
    let inner = renderButton(ctrl.name, 'icon btn_icon', ctrl.name, ctrl.label, ctrl.size, ctrl.color, true);
    addWidget(ctrl.tab_w, ctrl.name, '', inner, 0, true);
  } else {
    if (!btn_row_id) beginButtons();
    EL(btn_row_id).innerHTML += `${renderButton(ctrl.name, 'icon btn_icon', ctrl.name, ctrl.label, ctrl.size, ctrl.color, true)}`;
  }
}
function beginButtons() {
  btn_row_id = 'buttons_row#' + btn_row_count;
  btn_row_count++;
  EL('controls').innerHTML += `
  <div id="${btn_row_id}" class="control control_nob control_scroll"></div>
  `;
}
function endButtons() {
  if (btn_row_id && EL(btn_row_id).getElementsByTagName('*').length == 1) {
    EL(btn_row_id).innerHTML = "<div></div>" + EL(btn_row_id).innerHTML + "<div></div>";  // center button
  }
  btn_row_id = null;
}
function renderButton(title, className, name, label, size, color = null, is_icon = false) {
  let col = (color != null) ? ((is_icon ? ';color:' : ';background:') + intToCol(color)) : '';
  return `<button id="#${name}" title='${title}' style="font-size:${size}px${col}" class="${className}" onclick="set_h('${name}',2)" onmousedown="if(!touch)click_h('${name}',1)" onmouseup="if(!touch&&pressId)click_h('${name}',0)" onmouseleave="if(pressId&&!touch)click_h('${name}',0);" ontouchstart="touch=1;click_h('${name}',1)" ontouchend="click_h('${name}',0)">${label}</button>`;
}

// tabs
function addTabs(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let tabs = '';
  let labels = ctrl.text.toString().split(',');
  for (let i in labels) {
    let sel = (i == ctrl.value) ? 'class="tab_act"' : '';
    tabs += `<li onclick="set_h('${ctrl.name}','${i}')" ${sel}>${labels[i]}</li>`;
  }

  if (wid_row_id) {
    let inner = `
    <div class="navtab_tab">
      <ul>
        ${tabs}
      </ul>
    </div>
    `;
    addWidget(ctrl.tab_w, '', ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="navtab">
      <ul>
        ${tabs}
      </ul>
    </div>
  `;
  }
}

// menu
function addMenu(ctrl) {
  let inner = '';
  let labels = ctrl.text.toString().split(',');
  for (let i in labels) {
    let sel = (i == ctrl.value) ? 'menu_act' : '';
    inner += `<div onclick="menuClick(${i})" class="menu_item ${sel}">${labels[i]}</div>`;
  }
  document.querySelector(':root').style.setProperty('--menu_h', ((labels.length + 2) * 35 + 10) + 'px');
  EL('menu_user').innerHTML = inner;
}
function menuClick(num) {
  menu_show(0);
  menuDeact();
  if (screen != 'device') show_screen('device');
  set_h('_menu', num);
}
function menuDeact() {
  let els = document.getElementById('menu_user').children;
  for (let el in els) {
    if (els[el].tagName == 'DIV') els[el].classList.remove('menu_act');
  }
  EL('menu_info').classList.remove('menu_act');
  EL('menu_fsbr').classList.remove('menu_act');
}

// input
function addInput(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let col = (ctrl.color != null) ? ('box-shadow: 0px 2px 0px 0px ' + intToCol(ctrl.color)) : '';
  if (wid_row_id) {
    let inner = `
      <div class="cfg_inp_row cfg_inp_row_tab">
        <input class="cfg_inp c_inp input_t" style="${col}" type="text" value="${ctrl.value}" id="#${ctrl.name}" name="${ctrl.name}" onkeydown="checkEnter(this)" oninput="checkLen(this,${ctrl.max})" pattern="${ctrl.regex}">
        <div class="cfg_btn_block">
          <button class="icon cfg_btn" onclick="sendInput('${ctrl.name}')"></button>
        </div>
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <div class="cfg_inp_row">
        <input class="cfg_inp c_inp input_t" style="${col}" type="text" value="${ctrl.value}" id="#${ctrl.name}" name="${ctrl.name}" onkeydown="checkEnter(this)" oninput="checkLen(this,${ctrl.max})" pattern="${ctrl.regex}">
        <div class="cfg_btn_block">
          <button class="icon cfg_btn" onclick="sendInput('${ctrl.name}')"></button>
        </div>
      </div>
    </div>
  `;
  }
}
function sendInput(name) {
  let inp = EL('#' + name);
  const r = new RegExp(inp.pattern);
  if (r.test(inp.value)) set_h(name, inp.value);
  else showPopupError("Wrong text!");
}
function checkLen(arg, len) {
  if (len && arg.value.length > len) arg.value = arg.value.substring(0, len);
}
function checkEnter(arg) {
  if (event.key == 'Enter') {
    if (arg.pattern) sendInput(arg.name);
    else set_h(arg.name, arg.value);
  }
}

// pass
function addPass(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let col = (ctrl.color != null) ? ('box-shadow: 0px 2px 0px 0px ' + intToCol(ctrl.color)) : '';
  if (wid_row_id) {
    let inner = `
      <div class="cfg_inp_row cfg_inp_row_tab">
        <input class="cfg_inp c_inp input_t" style="${col}" type="password" value="${ctrl.value}" id="#${ctrl.name}" name="${ctrl.name}" onkeydown="checkEnter(this)" oninput="checkLen(this,${ctrl.max})">
        <div class="cfg_btn_block2">
          <button class="icon cfg_btn" onclick="togglePass('#${ctrl.name}')"></button>
          <button class="icon cfg_btn" onclick="set_h('${ctrl.name}',EL('#${ctrl.name}').value)"></button>
        </div>
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <div class="cfg_inp_row">
        <input class="cfg_inp c_inp input_t" style="${col}" type="password" value="${ctrl.value}" id="#${ctrl.name}" name="${ctrl.name}" onkeydown="checkEnter(this)" oninput="checkLen(this,${ctrl.max})">
        <div class="cfg_btn_block2">
          <button class="icon cfg_btn" onclick="togglePass('#${ctrl.name}')"></button>
          <button class="icon cfg_btn" onclick="set_h('${ctrl.name}',EL('#${ctrl.name}').value)"></button>
        </div>
      </div>
    </div>
    `;
  }
}
function togglePass(id) {
  if (EL(id).type == 'text') EL(id).type = 'password';
  else EL(id).type = 'text';
}

// slider
function addSlider(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let col = (ctrl.color != null) ? `background-image: linear-gradient(${intToCol(ctrl.color)}, ${intToCol(ctrl.color)})` : '';
  let formatted = formatToStep(ctrl.value, ctrl.step);
  if (wid_row_id) {
    let inner = `
    <input ontouchstart="dis_scroll_f=2" ontouchend="dis_scroll_f=0;enableScroll()" name="${ctrl.name}" id="#${ctrl.name}" oninput="moveSlider(this)" type="range" class="c_rangeW slider_t" style="${col}" value="${ctrl.value}" min="${ctrl.min}" max="${ctrl.max}" step="${ctrl.step}"><div class="sldW_out"><output id="out#${ctrl.name}">${formatted}</output></div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <div class="sld_name">
        <label title='${ctrl.name}'>${ctrl.clabel}</label>
        <label>:&nbsp;</label>
        <output id="out#${ctrl.name}">${formatted}</output>
      </div>
      <div class="cfg_inp_row">
        <input ontouchstart="dis_scroll_f=2" ontouchend="dis_scroll_f=0;enableScroll()" name="${ctrl.name}" id="#${ctrl.name}" oninput="moveSlider(this)" type="range" class="c_range slider_t" style="${col}" value="${ctrl.value}" min="${ctrl.min}" max="${ctrl.max}" step="${ctrl.step}">      
      </div>
    </div>
  `;
  }
}
function moveSliders() {
  document.querySelectorAll('.c_range, .c_rangeW').forEach(x => { moveSlider(x, false) });
}
function moveSlider(arg, sendf = true) {
  if (dis_scroll_f) {
    dis_scroll_f--;
    if (!dis_scroll_f) disableScroll();
  }
  arg.style.backgroundSize = (arg.value - arg.min) * 100 / (arg.max - arg.min) + '% 100%';
  EL('out' + arg.id).value = formatToStep(arg.value, arg.step);
  if (sendf) input_h(arg.name, arg.value);
}

// switch
function addSwitch(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let ch = ctrl.value ? 'checked' : '';
  let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.slider{background:${intToCol(ctrl.color)}}</style>` : '';
  if (wid_row_id) {
    let inner = `${col}
    <label id="swlabel_${ctrl.name}" class="switch"><input type="checkbox" class="switch_t" id='#${ctrl.name}' onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" ${ch}><span class="slider"></span></label>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <label id="swlabel_${ctrl.name}" class="switch"><input type="checkbox" class="switch_t" id='#${ctrl.name}' onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" ${ch}><span class="slider"></span></label>
    </div>
  `;
  }
}
function addSwitchIcon(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let ch = ctrl.value ? 'checked' : '';
  let text = ctrl.text ? ctrl.text : '';
  if (isESP()) text = "";
  if (wid_row_id) {
    let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.switch_i_tab{background:${intToCol(ctrl.color)};color:var(--font_inv)} #swlabel_${ctrl.name} .switch_i_tab{box-shadow: 0 0 0 2px ${intToCol(ctrl.color)};color:${intToCol(ctrl.color)}}</style>` : '';
    let inner = `${col}
    <label id="swlabel_${ctrl.name}" class="switch_i_cont switch_i_cont_tab"><input type="checkbox" onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" class="switch_t" id='#${ctrl.name}' ${ch}><span class="switch_i switch_i_tab">${text}</span></label>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner, 120);
  } else {
    let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.switch_i{color:${intToCol(ctrl.color)}}</style>` : '';
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <label id="swlabel_${ctrl.name}" class="switch_i_cont"><input type="checkbox" onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" class="switch_t" id='#${ctrl.name}' ${ch}><span class="switch_i">${text}</span></label>
    </div>
  `;
  }
}
function addSwitchText(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let ch = ctrl.value ? 'checked' : '';
  let text = ctrl.text ? ctrl.text : 'ON';
  if (wid_row_id) {
    let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.switch_i_tab{background:${intToCol(ctrl.color)};color:var(--font_inv)} #swlabel_${ctrl.name} .switch_i_tab{box-shadow: 0 0 0 2px ${intToCol(ctrl.color)};color:${intToCol(ctrl.color)}}</style>` : '';
    let inner = `${col}
    <label id="swlabel_${ctrl.name}" class="switch_i_cont switch_i_cont_tab"><input type="checkbox" onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" class="switch_t" id='#${ctrl.name}' ${ch}><span class="switch_i switch_i_tab switch_txt switch_txt_tab">${text}</span></label>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner, 120);
  } else {
    let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.switch_i{color:${intToCol(ctrl.color)}}</style>` : '';
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <label id="swlabel_${ctrl.name}" class="switch_i_cont"><input type="checkbox" onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" class="switch_t" id='#${ctrl.name}' ${ch}><span class="switch_i switch_txt">${text}</span></label>
    </div>
  `;
  }
}

// date time
function addDate(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let date = new Date(ctrl.value * 1000).toISOString().split('T')[0];
  let col = (ctrl.color != null) ? `color:${intToCol(ctrl.color)}` : '';
  if (wid_row_id) {
    let inner = `
    <input id='#${ctrl.name}' class="cfg_inp c_inp_block c_inp_block_tab date_t" style="${col}" type="date" value="${date}" onclick="this.showPicker()" onchange="set_h('${ctrl.name}',getUnix(this))">
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.clabel}</label>
        <input id='#${ctrl.name}' class="cfg_inp c_inp_block datime date_t" style="${col}" type="date" value="${date}" onclick="this.showPicker()" onchange="set_h('${ctrl.name}',getUnix(this))">
      </div>
    `;
  }
}
function addTime(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let time = new Date(ctrl.value * 1000).toISOString().split('T')[1].split('.')[0];
  let col = (ctrl.color != null) ? `color:${intToCol(ctrl.color)}` : '';
  if (wid_row_id) {
    let inner = `
    <input id='#${ctrl.name}' class="cfg_inp c_inp_block c_inp_block_tab time_t" style="${col}" type="time" value="${time}" onclick="this.showPicker()" onchange="set_h('${ctrl.name}',getUnix(this))" step="1">
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <input id='#${ctrl.name}' class="cfg_inp c_inp_block datime time_t" style="${col}" type="time" value="${time}" onclick="this.showPicker()" onchange="set_h('${ctrl.name}',getUnix(this))" step="1">
    </div>
  `;
  }
}
function addDateTime(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let datetime = new Date(ctrl.value * 1000).toISOString().split('.')[0];
  let col = (ctrl.color != null) ? `color:${intToCol(ctrl.color)}` : '';
  if (wid_row_id) {
    let inner = `
    <input id='#${ctrl.name}' class="cfg_inp c_inp_block c_inp_block_tab datetime_t" style="${col}" type="datetime-local" value="${datetime}" onclick="this.showPicker()" onchange="set_h('${ctrl.name}',getUnix(this))" step="1">
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <input id='#${ctrl.name}' class="cfg_inp c_inp_block datime datime_w datetime_t" style="${col}" type="datetime-local" value="${datetime}" onclick="this.showPicker()" onchange="set_h('${ctrl.name}',getUnix(this))" step="1">
    </div>
  `;
  }
}
function getUnix(arg) {
  return Math.floor(arg.valueAsNumber / 1000);
}

// color
function addColor(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let color = intToCol(ctrl.value);
  let inner = `
    <div id="color_cont#${ctrl.name}" style="visibility: hidden">
      <div id='#${ctrl.name}'></div>
    </div>
    <button id="color_btn#${ctrl.name}" style="margin-left:-30px;color:${color}" class="icon cfg_btn_tab" onclick="openPicker('${ctrl.name}')"></button>
    `;

  if (wid_row_id) {
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      ${inner}
    </div>
    `;
  }
  pickers[ctrl.name] = color;
  /*if (wid_row_id) {
    let inner = `
    <input id='#${ctrl.name}' class="c_base_inp c_col c_col_tab input_t" type="color" value="${ctrl.value}" onchange="set_h('${ctrl.name}',this.value)">
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label}</label>
      <input id='#${ctrl.name}' class="c_base_inp c_col input_t" type="color" value="${ctrl.value}" onchange="set_h('${ctrl.name}',this.value)">
    </div>
    `;
  }*/
}
function openPicker(id) {
  EL('color_cont#' + id).getElementsByTagName('button')[0].click()
}
function showPickers() {
  for (let picker in pickers) {
    let id = '#' + picker;
    let obj = Pickr.create({
      el: EL(id),
      theme: 'nano',
      default: pickers[picker],
      defaultRepresentation: 'HEXA',
      components: {
        preview: true,
        hue: true,
        interaction: {
          hex: false,
          input: true,
          save: true
        }
      }
    }).on('save', (color) => {
      let col = color.toHEXA().toString();
      set_h(picker, colToInt(col));
      EL('color_btn' + id).style.color = col;
    });
    pickers[picker] = obj;
  }
}

// spinner
function addSpinner(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let formatted = formatToStep(ctrl.value, ctrl.step);
  if (wid_row_id) {
    let inner = `
      <div class="spinner_row">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, -1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
        <input id="#${ctrl.name}" name="${ctrl.name}" class="cfg_inp spinner input_t" type="number" oninput="resizeSpinner(this)" onkeydown="checkEnter(this)" value="${formatted}" min="${ctrl.min}"
          max="${ctrl.max}" step="${ctrl.step}">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, 1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <div class="spinner_row">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, -1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
        <input id="#${ctrl.name}" name="${ctrl.name}" class="cfg_inp spinner input_t" type="number" oninput="resizeSpinner(this)" onkeydown="checkEnter(this)" value="${formatted}" min="${ctrl.min}"
          max="${ctrl.max}" step="${ctrl.step}">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, 1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
      </div>
    </div>
  `;
  }
}
function spinSpinner(el, dir) {
  let num = (dir == 1) ? el.previousElementSibling : el.nextElementSibling;
  let val = Number(num.value) + Number(num.step) * Number(dir);
  val = Math.max(Number(num.min), val);
  val = Math.min(Number(num.max), val);
  num.value = formatToStep(val, num.step);
  resizeSpinner(num);
}
function resizeSpinner(el) {
  el.style.width = el.value.length + 'ch';
}
function resizeSpinners() {
  let spinners = document.querySelectorAll(".spinner");
  spinners.forEach((sp) => resizeSpinner(sp));
}

// flags
function addFlags(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let flags = "";
  let val = ctrl.value;
  let labels = ctrl.text.toString().split(',');
  for (let i = 0; i < labels.length; i++) {
    let ch = (!(val & 1)) ? '' : 'checked';
    val >>= 1;
    flags += `<label id="swlabel_${ctrl.name}" class="chbutton chtext">
    <input name="${ctrl.name}" type="checkbox" onclick="set_h('${ctrl.name}',encodeFlags('${ctrl.name}'))" ${ch}>
    <span class="chbutton_s chtext_s">${labels[i]}</span></label>`;
  }
  let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.chbutton_s{background:${intToCol(ctrl.color)}}</style>` : '';

  if (wid_row_id) {
    let inner = `${col}
      <div class="chbutton_cont chbutton_cont_tab flags_t" id='#${ctrl.name}'>
        ${flags}
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <div class="chbutton_cont flags_t" id='#${ctrl.name}'>
        ${flags}
      </div>
    </div>
  `;
  }
}
function resizeFlags() {
  let chtext = document.querySelectorAll(".chtext");
  let chtext_s = document.querySelectorAll(".chtext_s");
  chtext.forEach((ch, i) => {
    let len = chtext_s[i].innerHTML.length + 2;
    chtext[i].style.width = (len + 0.5) + 'ch';
    chtext_s[i].style.width = len + 'ch';
  });
}
function encodeFlags(name) {
  let weeks = document.getElementsByName(name);
  let encoded = 0;
  weeks.forEach((w, i) => {
    if (w.checked) encoded |= (1 << weeks.length);
    encoded >>= 1;
  });
  return encoded;
}

// canvas
function addCanvas(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  if (wid_row_id) {
    let inner = `
    <canvas onclick="clickCanvas('${ctrl.name}',event)" class="${ctrl.active ? 'canvas_act' : ''} canvas_t" id="#${ctrl.name}"></canvas>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="cv_block">
      <canvas onclick="clickCanvas('${ctrl.name}',event)" class="${ctrl.active ? 'canvas_act' : ''} canvas_t" id="#${ctrl.name}"></canvas>
    </div>
    `;
  }
  canvases[ctrl.name] = { name: ctrl.name, width: ctrl.width, height: ctrl.height, value: ctrl.value };
}
function showCanvases() {
  Object.values(canvases).forEach(canvas => {
    let cv = EL('#' + canvas.name);
    if (!cv || !cv.parentNode.clientWidth) return;
    let rw = cv.parentNode.clientWidth;
    canvas.scale = rw / canvas.width;
    let rh = Math.floor(canvas.height * canvas.scale);
    cv.style.width = rw + 'px';
    cv.style.height = rh + 'px';
    cv.width = Math.floor(rw * ratio());
    cv.height = Math.floor(rh * ratio());
    canvas.scale *= ratio();
    drawCanvas(canvas);
  });
}
function drawCanvas(canvas) {
  let ev_str = '';
  let cv = EL('#' + canvas.name);

  function cv_map(v, h) {
    v *= canvas.scale;
    return v >= 0 ? v : (h ? cv.height : cv.width) - v;
  }
  function scale() {
    return canvas.scale;
  }

  let cx = cv.getContext("2d");
  const cmd_list = ['fillStyle', 'strokeStyle', 'shadowColor', 'shadowBlur', 'shadowOffsetX', 'shadowOffsetY', 'lineWidth', 'miterLimit', 'font', 'textAlign', 'textBaseline', 'lineCap', 'lineJoin', 'globalCompositeOperation', 'globalAlpha', 'scale', 'rotate', 'rect', 'fillRect', 'strokeRect', 'clearRect', 'moveTo', 'lineTo', 'quadraticCurveTo', 'bezierCurveTo', 'translate', 'arcTo', 'arc', 'fillText', 'strokeText', 'drawImage', 'roundRect', 'fill', 'stroke', 'beginPath', 'closePath', 'clip', 'save', 'restore'];
  const const_list = ['butt', 'round', 'square', 'square', 'bevel', 'miter', 'start', 'end', 'center', 'left', 'right', 'alphabetic', 'top', 'hanging', 'middle', 'ideographic', 'bottom', 'source-over', 'source-atop', 'source-in', 'source-out', 'destination-over', 'destination-atop', 'destination-in', 'destination-out', 'lighter', 'copy', 'xor', 'top', 'bottom', 'middle', 'alphabetic'];

  for (d of canvas.value) {
    let div = d.indexOf(':');
    let cmd = parseInt(d, 10);

    if (!isNaN(cmd) && cmd <= 37) {
      if (div == 1 || div == 2) {
        let val = d.slice(div + 1);
        let vals = val.split(',');
        if (cmd <= 2) ev_str += ('cx.' + cmd_list[cmd] + '=\'' + intToColA(val) + '\';');
        else if (cmd <= 7) ev_str += ('cx.' + cmd_list[cmd] + '=' + (val * scale()) + ';');
        else if (cmd <= 13) ev_str += ('cx.' + cmd_list[cmd] + '=\'' + const_list[val] + '\';');
        else if (cmd <= 14) ev_str += ('cx.' + cmd_list[cmd] + '=' + val + ';');
        else if (cmd <= 16) ev_str += ('cx.' + cmd_list[cmd] + '(' + val + ');');
        else if (cmd <= 26) {
          let str = 'cx.' + cmd_list[cmd] + '(';
          for (let i in vals) {
            if (i > 0) str += ',';
            str += `cv_map(${vals[i]},${(i % 2)})`;
          }
          ev_str += (str + ');');
        } else if (cmd == 27) {
          ev_str += (`cx.${cmd_list[cmd]}(cv_map(${vals[0]},0),cv_map(${vals[1]},1),cv_map(${vals[2]},0),${vals[3]},${vals[4]},${vals[5]});`);
        } else if (cmd <= 29) {
          ev_str += (`cx.${cmd_list[cmd]}(${vals[0]},cv_map(${vals[1]},0),cv_map(${vals[2]},1),${vals[3]});`);
        } else if (cmd == 30) {
          let str = 'cx.' + cmd_list[cmd] + '(';
          for (let i in vals) {
            if (i > 0) {
              str += `,cv_map(${vals[i]},${!(i % 2)})`;
            } else str += vals[i];
          }
          ev_str += (str + ');');
        } else if (cmd == 31) {
          let str = 'cx.' + cmd_list[cmd] + '(';
          for (let i = 0; i < 4; i++) {
            if (i > 0) str += ',';
            str += `cv_map(${vals[i]},${(i % 2)})`;
          }
          if (vals.length == 5) str += `,${vals[4] * scale()}`;
          else {
            str += ',[';
            for (let i = 4; i < vals.length; i++) {
              if (i > 4) str += ',';
              str += `cv_map(${vals[i]},${(i % 2)})`;
            }
            str += ']';
          }
          ev_str += (str + ');');
        }
      } else {
        if (cmd >= 32) ev_str += ('cx.' + cmd_list[cmd] + '();');
      }
    } else {
      ev_str += d + ';';
    }
  }
  eval(ev_str);
  canvas.value = null;
}
function clickCanvas(id, e) {
  if (!(id in canvases)) return;
  let rect = EL('#' + id).getBoundingClientRect();
  let scale = canvases[id].scale;
  let x = Math.round((e.clientX - rect.left) / scale * ratio());
  if (x < 0) x = 0;
  let y = Math.round((e.clientY - rect.top) / scale * ratio());
  if (y < 0) y = 0;
  set_h(id, (x << 16) | y);
}

// gauge
function addGauge(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  if (wid_row_id) {
    let inner = `
    <canvas class="gauge_t" id="#${ctrl.name}"></canvas>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="cv_block cv_block_back">
      <canvas class="gauge_t" id="#${ctrl.name}"></canvas>
    </div>
    `;
  }
  gauges[ctrl.name] = { perc: null, name: ctrl.name, value: Number(ctrl.value), min: Number(ctrl.min), max: Number(ctrl.max), step: Number(ctrl.step), text: ctrl.text, color: ctrl.color };
}
function drawGauge(g) {
  let cv = EL('#' + g.name);
  if (!cv || !cv.parentNode.clientWidth) return;

  let perc = (g.value - g.min) * 100 / (g.max - g.min);
  if (perc < 0) perc = 0;
  if (perc > 100) perc = 100;
  if (g.perc == null) g.perc = perc;
  else {
    if (Math.abs(g.perc - perc) <= 0.2) g.perc = perc;
    else g.perc += (perc - g.perc) * 0.2;
    if (g.perc != perc) setTimeout(() => drawGauge(g), 30);
  }

  let cx = cv.getContext("2d");
  let v = themes[cfg.theme];
  let col = g.color == null ? intToCol(colors[cfg.maincolor]) : intToCol(g.color);
  let rw = cv.parentNode.clientWidth;
  let rh = Math.floor(rw * 0.47);
  cv.style.width = rw + 'px';
  cv.style.height = rh + 'px';
  cv.width = Math.floor(rw * ratio());
  cv.height = Math.floor(rh * ratio());

  cx.clearRect(0, 0, cv.width, cv.height);
  cx.lineWidth = cv.width / 8;
  cx.strokeStyle = theme_cols[v][4];
  cx.beginPath();
  cx.arc(cv.width / 2, cv.height * 0.97, cv.width / 2 - cx.lineWidth, Math.PI * (1 + g.perc / 100), Math.PI * 2);
  cx.stroke();

  cx.strokeStyle = col;
  cx.beginPath();
  cx.arc(cv.width / 2, cv.height * 0.97, cv.width / 2 - cx.lineWidth, Math.PI, Math.PI * (1 + g.perc / 100));
  cx.stroke();

  let font = cfg.font;
  /*NON-ESP*/
  font = 'PTSans Narrow';
  /*/NON-ESP*/

  cx.fillStyle = col;
  cx.font = '10px ' + font;
  cx.textAlign = "center";

  let text = g.text;
  let len = Math.max(
    (formatToStep(g.value, g.step) + text).length,
    (formatToStep(g.min, g.step) + text).length,
    (formatToStep(g.max, g.step) + text).length
  );
  if (len == 1) text += '  ';
  else if (len == 2) text += ' ';

  let w = Math.max(
    cx.measureText(formatToStep(g.value, g.step) + text).width,
    cx.measureText(formatToStep(g.min, g.step) + text).width,
    cx.measureText(formatToStep(g.max, g.step) + text).width
  );

  cx.fillStyle = theme_cols[v][3];
  cx.font = cv.width * 0.45 * 10 / w + 'px ' + font;
  cx.fillText(formatToStep(g.value, g.step) + g.text, cv.width / 2, cv.height * 0.93);

  cx.font = '10px ' + font;
  w = Math.max(
    cx.measureText(Math.round(g.min)).width,
    cx.measureText(Math.round(g.max)).width
  );
  cx.fillStyle = theme_cols[v][2];
  cx.font = cx.lineWidth * 0.55 * 10 / w + 'px ' + font;
  cx.fillText(g.min, cx.lineWidth, cv.height * 0.92);
  cx.fillText(g.max, cv.width - cx.lineWidth, cv.height * 0.92);
}
function showGauges() {
  Object.values(gauges).forEach(gauge => {
    drawGauge(gauge);
  });
}

// joystick
function addJoy(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let inner = `
    <div class="joyCont"><canvas id="#${ctrl.name}"></canvas></div>
  `;

  if (wid_row_id) {
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += inner;
  }
  joys[ctrl.name] = ctrl;
}
function showJoys() {
  for (let joy in joys) {
    joys[joy].joy = new Joystick(joy,
      joys[joy].type == 'dpad',
      intToCol(joys[joy].color == null ? colors[cfg.maincolor] : joys[joy].color),
      joys[joy].auto,
      joys[joy].exp,
      function (data) {
        input_h(joy, ((data.x + 255) << 16) | (data.y + 255));
      });
  }
}

// other
function addSpace(ctrl) {
  if (wid_row_id) {
    checkWidget(ctrl);
    wid_row_size += ctrl.tab_w;
    if (wid_row_size > 100) {
      beginWidgets();
      wid_row_size = ctrl.tab_w;
    }
    EL(wid_row_id).innerHTML += `
    <div class="widget" style="width:${ctrl.tab_w}%"><div class="widget_inner widget_space"></div></div>
  `;
  } else {
    endButtons();
    EL('controls').innerHTML += `
    <div style="height:${ctrl.height}px"></div>
  `;
  }
}
function addTitle(ctrl) {
  endWidgets();
  endButtons();
  EL('controls').innerHTML += `
  <div class="control control_title">
    <span class="c_title">${ctrl.label}</span>
  </div>
  `;
}
function addLED(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let ch = ctrl.value ? 'checked' : '';
  if (ctrl.text && !isESP()) {
    if (wid_row_id) {
      let inner = `
      <label id="swlabel_${ctrl.name}" class="led_i_cont led_i_cont_tab"><input type="checkbox" class="switch_t" id='#${ctrl.name}' ${ch} disabled><span class="switch_i led_i led_i_tab">${ctrl.text}</span></label>
      `;
      addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
    } else {
      EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.clabel}</label>
        <label id="swlabel_${ctrl.name}" class="led_i_cont"><input type="checkbox" class="switch_t" id='#${ctrl.name}' ${ch} disabled><span class="switch_i led_i">${ctrl.text}</span></label>
      </div>
    `;
    }
  } else {
    if (wid_row_id) {
      let inner = `
    <label class="led_cont"><input type="checkbox" class="switch_t" id='#${ctrl.name}' ${ch} disabled><span class="led"></span></label>
    `;
      addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
    } else {
      EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.clabel}</label>
        <label class="led_cont"><input type="checkbox" class="switch_t" id='#${ctrl.name}' ${ch} disabled><span class="led"></span></label>
      </div>
    `;
    }
  }
}
function addIcon(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  if (isESP()) ctrl.text = "";
  let col = (ctrl.color != null) ? `color:${intToCol(ctrl.color)}` : '';
  if (wid_row_id) {
    let inner = `
    <span class="icon icon_t" id='#${ctrl.name}' style="${col}">${ctrl.text}</span>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.clabel}</label>
        <span class="icon icon_t" id='#${ctrl.name}' style="${col}">${ctrl.text}</span>
      </div>
    `;
  }
}
function addLabel(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let col = (ctrl.color) ? (`color:${intToCol(ctrl.color)}`) : '';
  if (wid_row_id) {
    let inner = `
    <label class="c_label text_t c_label_tab" id='#${ctrl.name}' style="${col};font-size:${ctrl.size}px">${ctrl.value}</label>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <label class="c_label text_t" id='#${ctrl.name}' style="${col};font-size:${ctrl.size}px">${ctrl.value}</label>
    </div>
  `;
  }
}
function addSelect(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let elms = ctrl.text.toString().split(',');
  let options = '';
  for (let i in elms) {
    let sel = (i == ctrl.value) ? 'selected' : '';
    options += `<option value="${i}" ${sel}>${elms[i]}</option>`;
  }
  let col = (ctrl.color != null) ? `color:${intToCol(ctrl.color)}` : '';
  if (wid_row_id) {
    let inner = `
    <select class="cfg_inp c_inp_block select_t" style="${col}" id='#${ctrl.name}' onchange="set_h('${ctrl.name}',this.value)">
      ${options}
    </select>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
    <label title='${ctrl.name}'>${ctrl.clabel}</label>
      <select class="cfg_inp c_inp_block select_t" style="${col}" id='#${ctrl.name}' onchange="set_h('${ctrl.name}',this.value)">
        ${options}
      </select>
    </div>
  `;
  }
}
function addLog(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  if (ctrl.text.endsWith('\n')) ctrl.text = ctrl.text.slice(0, -1);
  if (wid_row_id) {
    let inner = `
    <textarea id="#${ctrl.name}" title='${ctrl.name}' class="cfg_inp c_log text_t" readonly>${ctrl.text}</textarea>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <textarea id="#${ctrl.name}" title='${ctrl.name}' class="cfg_inp c_log text_t" readonly>${ctrl.text}</textarea>
    </div>
  `;
  }
}
function addDisplay(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let col = (ctrl.color != null) ? ('background:' + intToCol(ctrl.color)) : '';
  if (wid_row_id) {
    let inner = `
    <textarea id="#${ctrl.name}" title='${ctrl.name}' class="cfg_inp c_area c_disp text_t" style="font-size:${ctrl.size}px;${col}" rows="${ctrl.rows}" readonly>${ctrl.value}</textarea>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <textarea id="#${ctrl.name}" title='${ctrl.name}' class="cfg_inp c_area c_disp text_t" style="font-size:${ctrl.size}px;${col}" rows="${ctrl.rows}" readonly>${ctrl.value}</textarea>
    </div>
  `;
  }
}
function addHTML(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let inner = `<div name="text" id="#${ctrl.name}" title='${ctrl.name}' class="c_text text_t">${ctrl.value}</div>`;
  if (wid_row_id) {
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control control_nob">
      ${inner}
    </div>
    `;
  }
}
function addImage(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let inner = `
    <div class="image_t" name="${ctrl.value}" id="#${ctrl.name}">${waiter()}</div>
    `;
  if (wid_row_id) {
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="cv_block cv_block_back">
    ${inner}
    </div>
    `;
  }
  files.push({ id: '#' + ctrl.name, path: ctrl.value, type: 'img' });
}
function addStream(ctrl, conn, ip) {
  checkWidget(ctrl);
  endButtons();
  let inner = '<label>No connection</label>';
  if (conn == Conn.WS && ip != 'unset') inner = `<img style="width:100%" src="http://${ip}:${ctrl.port}/">`;
  if (wid_row_id) {
    addWidget(ctrl.tab_w, '', ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="cv_block cv_block_back">
    ${inner}
    </div>
    `;
  }
}
function addTable(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let table = parseCSV(ctrl.value);
  let aligns = ctrl.align.split(',');
  let widths = ctrl.width.split(',');
  let inner = '<table class="c_table">';
  for (let row of table) {
    inner += '<tr>';
    for (let col in row) {
      inner += `<td width="${widths[col] ? (widths[col] + '%') : ''}" align="${aligns[col] ? aligns[col] : 'center'}">${row[col]}</td>`;
    }
    inner += '</tr>';
  }
  inner += '</table>';

  if (wid_row_id) {
    addWidget(ctrl.tab_w, ctrl.name, ctrl.wlabel, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control control_nob">
      ${inner}
    </div>
    `;
  }
}

// ================ WIDGET =================
function checkWidget(ctrl) {
  if (ctrl.tab_w && !wid_row_id) beginWidgets(null, true);
}
function beginWidgets(ctrl = null, check = false) {
  if (!check) endButtons();
  wid_row_size = 0;
  if (devices[focused].break_widgets) return;
  let st = (ctrl && ctrl.height) ? `style="height:${ctrl.height}px"` : '';
  wid_row_id = 'widgets_row#' + wid_row_count;
  wid_row_count++;
  EL('controls').innerHTML += `
  <div class="widget_row" id="${wid_row_id}" ${st}></div>
  `;
}
function endWidgets() {
  endButtons();
  wid_row_id = null;
}
function addWidget(width, name, label, inner, height = 0, noback = false) {
  wid_row_size += width;
  if (wid_row_size > 100) {
    beginWidgets();
    wid_row_size = width;
  }

  let h = height ? ('height:' + height + 'px') : '';
  let lbl = (label && label != '_no') ? `<div class="widget_label" title="${name}">${label}<span id="wlabel#${name}"></span></div>` : '';
  EL(wid_row_id).innerHTML += `
  <div class="widget" style="width:${width}%;${h}">
    <div class="widget_inner ${noback ? 'widget_space' : ''}">
      ${lbl}
      <div class="widget_block">
        ${inner}
      </div>
    </div>
  </div>
  `;
}

// ================ UTILS =================
function showNotif(text, name) {
  if (!("Notification" in window) || Notification.permission != 'granted') return;
  let descr = name + ' (' + new Date(Date.now()).toLocaleString() + ')';
  navigator.serviceWorker.getRegistration().then(function (reg) {
    reg.showNotification(text, { body: descr, vibrate: true });
  });
  //new Notification(text, {body: descr});
  //self.registration.showNotification(text, {body: descr});
}
function checkDup(ctrl) {
  if (EL('#' + ctrl.name)) {
    dup_names.push(' ' + ctrl.name);
    return 1;
  }
  return 0;
}
function formatToStep(val, step) {
  step = step.toString();
  if (step.includes('.')) return Number(val).toFixed((step.split('.')[1]).toString().length);
  else return val;
}
function scrollDown() {
  let logs = document.querySelectorAll(".c_log");
  logs.forEach((log) => log.scrollTop = log.scrollHeight);
}
function parseCSV(str) {
  // https://stackoverflow.com/a/14991797
  const arr = [];
  let quote = false;
  for (let row = 0, col = 0, c = 0; c < str.length; c++) {
    let cc = str[c], nc = str[c + 1];
    arr[row] = arr[row] || [];
    arr[row][col] = arr[row][col] || '';
    if (cc == '"' && quote && nc == '"') { arr[row][col] += cc; ++c; continue; }
    if (cc == '"') { quote = !quote; continue; }
    if (cc == ',' && !quote) { ++col; continue; }
    if (cc == '\r' && nc == '\n' && !quote) { ++row; col = 0; ++c; continue; }
    if (cc == '\n' && !quote) { ++row; col = 0; continue; }
    if (cc == '\r' && !quote) { ++row; col = 0; continue; }
    arr[row][col] += cc;
  }
  return arr;
}

// ================ DOWNLOAD =================
function nextFile() {
  if (!files.length) return;
  fetch_to_file = true;
  if (devices_t[focused].conn == Conn.WS && devices_t[focused].http_cfg.download && files[0].path.startsWith(devices_t[focused].http_cfg.path)) {
    downloadFile();
    EL('wlabel' + files[0].id).innerHTML = ' [fetch...]';
  } else {
    fetch_path = files[0].path;
    post('fetch', fetch_path);
  }
}

function downloadFile() {
  fetching = focused;
  var xhr = new XMLHttpRequest();
  xhr.responseType = 'blob';
  xhr.open('GET', 'http://' + devices[focused].ip + ':' + http_port + files[0].path);
  xhr.onprogress = function (e) {
    processFile(Math.round(e.loaded * 100 / e.total));
  };
  xhr.onloadend = function (e) {
    if (e.loaded && e.loaded == e.total) {
      processFile(100);
      var reader = new FileReader();
      reader.readAsDataURL(xhr.response);
      reader.onloadend = function () {
        downloadFileEnd(this.result.split('base64,')[1]);
      }
    } else {
      errorFile();
    }
  }
  xhr.send();
}

function downloadFileEnd(data) {
  switch (files[0].type) {
    case 'img':
      EL(files[0].id).innerHTML = `<img style="width:100%" src="data:${getMime(files[0].path)};base64,${data}">`;
      if (EL('wlabel' + files[0].id)) EL('wlabel' + files[0].id).innerHTML = '';
      break;
  }
  files.shift();
  nextFile();
  fetching = null;
  stopFS();
}
function processFile(perc) {
  if (EL('wlabel' + files[0].id)) EL('wlabel' + files[0].id).innerHTML = ` [${perc}%]`;
}
function errorFile() {
  if (EL('wlabel' + files[0].id)) EL('wlabel' + files[0].id).innerHTML = ' [error]';
  files.shift();
  nextFile();
}