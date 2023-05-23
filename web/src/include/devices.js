// ============== VARS =================
let devices = {};
let devices_t = {};
let controls = {};
let focused = null;
let touch = 0;
let pressId = null;
let pickers = {};
let dup_names = [];

let wid_row_id = null;
let wid_row_count = 0;
let wid_row_size = 0;
let btn_row_id = null;
let btn_row_count = 0;
let dis_scroll_f = false;

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
function addDevice(id) {
  EL('devices').innerHTML += `<div class="device offline" id="device#${id}" onclick="device_h('${id}')" title="${id} [${devices[id].prefix}]">
  <div class="device_inner">
    <div class="d_icon"><span class="icon" id="icon#${id}">${devices[id].icon}</span></div>
      <div class="d_head">
        <span><span class="d_name" id="name#${id}">${devices[id].name}</span><sup class="conn_dev" id="Serial#${id}">S</sup><sup class="conn_dev" id="BT#${id}">B</sup><sup class="conn_dev" id="WS#${id}">W</sup><sup class="conn_dev" id="MQTT#${id}">M</sup></span>
      </div>
      <div class="d_delete" onclick="delete_h('${id}')">x</div>
    </div>
  </div>`;
}

// ============ COMPONENTS =============
function addButton(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  if (wid_row_id) {
    endButtons();
    let inner = renderButton(ctrl.name, 'icon btn_icon', ctrl.name, '', ctrl.size * 3, ctrl.color, true);
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    if (!btn_row_id) beginButtons();
    EL(btn_row_id).innerHTML += `${renderButton(ctrl.name, 'c_btn', ctrl.name, ctrl.label, ctrl.size, ctrl.color, false)}`;
  }
}
function addButtonIcon(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  if (wid_row_id) {
    endButtons();
    let inner = renderButton(ctrl.name, 'icon btn_icon', ctrl.name, ctrl.label, ctrl.size, ctrl.color, true);
    addWidget(ctrl.tab_w, ctrl.name, '', inner, 0, true);
  } else {
    if (!btn_row_id) beginButtons();
    EL(btn_row_id).innerHTML += `${renderButton(ctrl.name, 'icon btn_icon', ctrl.name, ctrl.label, ctrl.size, ctrl.color, true)}`;
  }
}

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
    addWidget(ctrl.tab_w, '', ctrl.label, inner);
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
function addSpace(ctrl) {
  /*if (wid_row_id) {
    EL(wid_row_id).innerHTML += `
    <div class="widget" style="width:${ctrl.tab_w}%"><div class="widget_inner widget_space"></div></div>
  `;
  } else {*/
  endButtons();
  EL('controls').innerHTML += `
    <div style="height:${ctrl.height}px"></div>
  `;
  //}
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
// =========================
function addLED(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let ch = ctrl.value ? 'checked' : '';
  if (ctrl.text) {

    if (wid_row_id) {
      let inner = `
      <label id="swlabel_${ctrl.name}" class="led_i_cont led_i_cont_tab"><input type="checkbox" class="switch_t" id='#${ctrl.name}' ${ch} disabled><span class="switch_i led_i led_i_tab">${ctrl.text}</span></label>
      `;
      addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
    } else {
      EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Switch'}</label>
        <label id="swlabel_${ctrl.name}" class="led_i_cont"><input type="checkbox" class="switch_t" id='#${ctrl.name}' ${ch} disabled><span class="switch_i led_i">${ctrl.text}</span></label>
      </div>
    `;
    }


  } else {
    if (wid_row_id) {
      let inner = `
    <label class="led_cont"><input type="checkbox" class="switch_t" id='#${ctrl.name}' ${ch} disabled><span class="led"></span></label>
    `;
      addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
    } else {
      EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'LED'}</label>
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
  let col = (ctrl.color != null) ? `color:${intToCol(ctrl.color)}` : '';
  if (wid_row_id) {
    let inner = `
    <span class="icon icon_t" id='#${ctrl.name}' style="${col}">${ctrl.text}</span>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Icon'}</label>
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Label'}</label>
      <label class="c_label text_t" id='#${ctrl.name}' style="${col};font-size:${ctrl.size}px">${ctrl.value}</label>
    </div>
  `;
  }
}

function addInput(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let col = (ctrl.color != null) ? ('box-shadow: 0px 2px 0px 0px ' + intToCol(ctrl.color)) : '';
  if (wid_row_id) {
    let inner = `
      <div class="cfg_inp_row cfg_inp_row_tab">
        <input class="cfg_inp c_inp input_t" style="${col}" type="text" value="${ctrl.value}" id="#${ctrl.name}" name="${ctrl.name}" onkeydown="checkEnter(this)" oninput="checkLen(this,${ctrl.max})">
        <div class="cfg_btn_block">
          <button class="icon cfg_btn" onclick="set_h('${ctrl.name}',EL('#${ctrl.name}').value)"></button>
        </div>
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Input'}</label>
      <div class="cfg_inp_row">
        <input class="cfg_inp c_inp input_t" style="${col}" type="text" value="${ctrl.value}" id="#${ctrl.name}" name="${ctrl.name}" onkeydown="checkEnter(this)" oninput="checkLen(this,${ctrl.max})">
        <div class="cfg_btn_block">
          <button class="icon cfg_btn" onclick="set_h('${ctrl.name}',EL('#${ctrl.name}').value)"></button>
        </div>
      </div>
    </div>
  `;
  }
}
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Password'}</label>
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

function addSliderW(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  EL('controls').innerHTML += `
  <div class="control">
    <div class="sld_name">
      <label title='${ctrl.name}'>${ctrl.label}</label>
    </div>
    <div class="cfg_inp_row">
      <input name="${ctrl.name}" id="#${ctrl.name}" onchange="set_h('${ctrl.name}',this.value)" oninput="moveSlider(this)" type="range" class="c_rangeW slider_t" value="${ctrl.value}" min="${ctrl.min}" max="${ctrl.max}" step="${ctrl.step}"><div class="sldW_out"><output id="out#${ctrl.name}">${ctrl.value}</output></div>
    </div>
  </div>
  `;
}
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <div class="sld_name">
        <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Slider'}</label>
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Switch'}</label>
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
  if (wid_row_id) {
    let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.switch_i_tab{background:${intToCol(ctrl.color)};color:var(--font_inv)} #swlabel_${ctrl.name} .switch_i_tab{box-shadow: 0 0 0 2px ${intToCol(ctrl.color)};color:${intToCol(ctrl.color)}}</style>` : '';
    let inner = `${col}
    <label id="swlabel_${ctrl.name}" class="switch_i_cont switch_i_cont_tab"><input type="checkbox" onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" class="switch_t" id='#${ctrl.name}' ${ch}><span class="switch_i switch_i_tab">${text}</span></label>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner, 120);
  } else {
    let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.switch_i{color:${intToCol(ctrl.color)}}</style>` : '';
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Switch'}</label>
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner, 120);
  } else {
    let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.switch_i{color:${intToCol(ctrl.color)}}</style>` : '';
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Switch'}</label>
      <label id="swlabel_${ctrl.name}" class="switch_i_cont"><input type="checkbox" onclick="set_h('${ctrl.name}',(this.checked ? 1 : 0))" class="switch_t" id='#${ctrl.name}' ${ch}><span class="switch_i switch_txt">${text}</span></label>
    </div>
  `;
  }
}

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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
      <div class="control">
        <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Date'}</label>
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Time'}</label>
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Datime'}</label>
      <input id='#${ctrl.name}' class="cfg_inp c_inp_block datime datime_w datetime_t" style="${col}" type="datetime-local" value="${datetime}" onclick="this.showPicker()" onchange="set_h('${ctrl.name}',getUnix(this))" step="1">
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
    <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Select'}</label>
      <select class="cfg_inp c_inp_block select_t" style="${col}" id='#${ctrl.name}' onchange="set_h('${ctrl.name}',this.value)">
        ${options}
      </select>
    </div>
  `;
  }
}

function addColor(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let color = intToCol(ctrl.value);
  let inner = `
    <div id="color_cont#${ctrl.name}" style="visibility: hidden">
      <div id='#${ctrl.name}'></div>
    </div>
    <button id="color_btn#${ctrl.name}" style="margin-left:-30px;color:${color}" class="icon cfg_btn_tab" onclick="openColor('${ctrl.name}')"></button>
    `;

  if (wid_row_id) {
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Color'}</label>
      ${inner}
    </div>
    `;
  }
  pickers['#' + ctrl.name] = color;

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
function addSpinner(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let formatted = formatToStep(ctrl.value, ctrl.step);
  if (wid_row_id) {
    let inner = `
      <div class="spinner_row">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, -1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
        <input id="#${ctrl.name}" class="cfg_inp spinner input_t" type="number" oninput="resizeSpinner(this)" value="${formatted}" min="${ctrl.min}"
          max="${ctrl.max}" step="${ctrl.step}">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, 1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Spinner'}</label>
      <div class="spinner_row">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, -1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
        <input id="#${ctrl.name}" class="cfg_inp spinner input_t" type="number" oninput="resizeSpinner(this)" value="${formatted}" min="${ctrl.min}"
          max="${ctrl.max}" step="${ctrl.step}">
        <button class="icon cfg_btn btn_no_pad" onclick="spinSpinner(this, 1);set_h('${ctrl.name}',EL('#${ctrl.name}').value);"></button>
      </div>
    </div>
  `;
  }
}

/*function addWeek(ctrl) {
  if (checkDup(ctrl)) return;
  checkWidget(ctrl);
  endButtons();
  let week = "";
  let val = ctrl.value;
  for (let i = 0; i < 7; i++) {
    let ch = (!(val & 1)) ? '' : 'checked';
    val >>= 1;
    week += `<label id="swlabel_${ctrl.name}" class="chbutton">
    <input name="${ctrl.name}" type="checkbox" onclick="set_h('${ctrl.name}',chbuttonEncode('${ctrl.name}'))" ${ch}>
    <span class="chbutton_s">${i + 1}</span></label>`;
  }
  let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.chbutton_s{background:${intToCol(ctrl.color)}}</style>` : '';

  if (wid_row_id) {
    let inner = `${col}
      <div id='#${ctrl.name}' class="chbutton_cont chbutton_cont_tab week_t">
      ${week}
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Week'}</label>
      <div id='#${ctrl.name}' class="chbutton_cont week_t">
      ${week}
      </div>
    </div>
  `;
  }
  //<button class="icon cfg_btn" onclick=""></button>
}*/
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
    <input name="${ctrl.name}" type="checkbox" onclick="set_h('${ctrl.name}',chbuttonEncode('${ctrl.name}'))" ${ch}>
    <span class="chbutton_s chtext_s">${labels[i]}</span></label>`;
  }
  let col = (ctrl.color != null) ? `<style>#swlabel_${ctrl.name} input:checked+.chbutton_s{background:${intToCol(ctrl.color)}}</style>` : '';

  if (wid_row_id) {
    let inner = `${col}
      <div class="chbutton_cont chbutton_cont_tab flags_t" id='#${ctrl.name}'>
        ${flags}
      </div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label, inner);
  } else {
    EL('controls').innerHTML += `${col}
    <div class="control">
      <label title='${ctrl.name}'>${ctrl.label ? ctrl.label : 'Flags'}</label>
      <div class="chbutton_cont flags_t" id='#${ctrl.name}'>
        ${flags}
      </div>
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label ? ctrl.label : 'Log', inner);
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
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label ? ctrl.label : 'Display', inner);
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
  if (wid_row_id) {
    let inner = `
    <div name="text" id="#${ctrl.name}" title='${ctrl.name}' class="c_text text_t">${ctrl.value}</div>
    `;
    addWidget(ctrl.tab_w, ctrl.name, ctrl.label ? ctrl.label : 'HTML', inner);
  } else {
    EL('controls').innerHTML += `
    <div class="control">
      <div name="text" id="#${ctrl.name}" title='${ctrl.name}' class="c_text text_t">${ctrl.value}</div>
    </div>
    `;
  }
}

// ================ UTILS =================
function openColor(id) {
  EL('color_cont#' + id).getElementsByTagName('button')[0].click()
}
function checkLen(arg, len) {
  if (len && arg.value.length > len) arg.value = arg.value.substring(0, len);
}
function checkEnter(arg) {
  if (event.key == 'Enter') set_h(arg.name, arg.value);
}
function getUnix(arg) {
  return Math.floor(arg.valueAsNumber / 1000);
}
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
function renderButton(title, className, name, label, size, color = null, is_icon = false) {
  let col = (color != null) ? ((is_icon ? ';color:' : ';background:') + intToCol(color)) : '';
  return `<button id="#${name}" title='${title}' style="font-size:${size}px${col}" class="${className}" onmousedown="if(!touch)click_h('${name}',1)" onmouseup="if(!touch&&pressId)click_h('${name}',0)" onmouseleave="if(pressId&&!touch)click_h('${name}',0);" ontouchstart="touch=1;click_h('${name}',1)" ontouchend="click_h('${name}',0)">${label}</button>`;
}
function checkWidget(ctrl) {
  if (ctrl.tab_w && !wid_row_id) beginWidgets(null, true);
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
  let lbl = label ? `<div class="widget_label" title="${name}">${label}</div>` : '';
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
function moveSlider(arg, sendf = true) {
  if (dis_scroll_f) {
    dis_scroll_f--;
    if (!dis_scroll_f) disableScroll();
  }
  arg.style.backgroundSize = (arg.value - arg.min) * 100 / (arg.max - arg.min) + '% 100%';
  EL('out' + arg.id).value = formatToStep(arg.value, arg.step);
  if (sendf) input_h(arg.name, arg.value);
}
function formatToStep(val, step) {
  step = step.toString();
  if (step.indexOf('.') > 0) return Number(val).toFixed((step.split('.')[1]).toString().length);
  else return val;
}
function moveSliders() {
  document.querySelectorAll('.c_range, .c_rangeW').forEach(x => { moveSlider(x, false) });
}
function togglePass(id) {
  if (EL(id).type == 'text') EL(id).type = 'password';
  else EL(id).type = 'text';
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
function resizeChbuttons() {
  let chtext = document.querySelectorAll(".chtext");
  let chtext_s = document.querySelectorAll(".chtext_s");
  chtext.forEach((ch, i) => {
    let len = chtext_s[i].innerHTML.length + 2;
    chtext[i].style.width = (len + 0.5) + 'ch';
    chtext_s[i].style.width = len + 'ch';
  });
}
function chbuttonEncode(name) {
  let weeks = document.getElementsByName(name);
  let encoded = 0;
  weeks.forEach((w, i) => {
    if (w.checked) encoded |= (1 << weeks.length);
    encoded >>= 1;
  });
  return encoded;
}
function scrollDown() {
  let logs = document.querySelectorAll(".c_log");
  logs.forEach((log) => log.scrollTop = log.scrollHeight);
}