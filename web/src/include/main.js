let hub = new GyverHub();

function render_main(v) {
  head_cont.innerHTML = `
  <div class="title" id="title_cont">
    <div class="title_inn">
      <div id="title_row" class="title_row" onclick="back_h()">
        <span class="icon i_hover back_btn" id="back"></span>
        <span><span id="title"></span><sup id="conn"></sup><span class='version' id='version'>${v}</span></span>
      </div>

      <div id="conn_icons" style="display:flex">
        <span id='mqtt_ok' style="display:none;margin:0" class="icon cfg_icon"></span>
        <span id='serial_ok' style="display:none;margin:0" class="icon cfg_icon"></span>
        <span id='bt_ok' style="display:none;margin:0" class="icon cfg_icon"></span>
      </div>

      <div class="head_btns">
        <span class="icon i_hover" id='icon_refresh' onclick="refresh_h()"></span>
        <span class="icon i_hover" id='icon_cfg' style="display:none" onclick="config_h()"></span>
        <span class="icon i_hover" id='icon_menu' style="display:none" onclick="menu_h()"></span>
      </div>
    </div>
  </div>
  `;
  /*NON-ESP*/
  test_cont.innerHTML = `
  <div class="test_text">А тут пока ничего нет. Но будет онлайн-тест интерфейса, в котором можно будет поиграться и проверить свой билд без загрузки прошивки</div>
  `;
  
  projects_cont.innerHTML = `
  <div class="projects_inn">
    <div id="projects" class="projects"></div>

    <div class="projects">
      <div class="proj">
        <div class="proj_inn">
          <div class="proj_name">
            <a href="https://github.com/GyverLibs/GyverHub-projects" target="_blank">+ Add Project</a>
          </div>
        </div>
      </div>
    </div>
  </div>
  `;
  /*/NON-ESP*/

  cli_cont.innerHTML = `
  <div class="cli_block">
    <div class="cli_area" id="cli"></div>
    <div class="cli_row">
      <span class="icon cli_icon"></span>
      <input type="text" class="cfg_inp cli_inp" id="cli_input" onkeydown="checkCLI()">
      <button class="icon cfg_btn cli_icon cli_enter" onclick="sendCLI()"></button>
    </div>
  </div>
  `;

  /*NON-ESP*/
  footer_cont.innerHTML = `
  <div class="footer_inner">
    <!--<a href="https://alexgyver.ru/support_alex/" target="_blank"><span class="icon info_icon info_icon_u"></span>Support</a>-->
    <a style="cursor:pointer" onclick="projects_h()"><span class="icon info_icon info_icon_u"></span>Projects</a>
    <a style="cursor:pointer" onclick="test_h()"><span class="icon info_icon info_icon_u"></span>Test</a>
    <a href="https://github.com/GyverLibs/GyverHub/wiki" target="_blank"><span class="icon info_icon info_icon_u"></span>Wiki</a>
  </div>
  `;
  /*/NON-ESP*/

  main_cont.innerHTML = `
  <div id="menu_overlay" onclick="menu_show(0)"></div>
  <div id="menu" class="main_col menu">
    <div class="menu_inn">
      <div id="menu_user"></div>
      <div>
        <div id="menu_info" class="menu_item" onclick="info_h()">Info</div>
        <div id="menu_fsbr" class="menu_item" onclick="fsbr_h()">File & OTA</div>
      </div>
    </div>
  </div>

  <div class="main_inn">
    <div id="devices" class="main_col"></div>
    <div id="controls" class="main_col" style="max-width:unset"></div>
    
    <div id="info" class="main_col">
      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Settings</label>
        </div>

        <div class="cfg_row">
          <label>Console</label>
          <label class="switch"><input type="checkbox" id="info_cli_sw" onchange="showCLI(this.checked);save_devices()">
          <span class="slider"></span></label>
        </div>
        <div class="cfg_row">
          <label>Break Widgets</label>
          <label class="switch"><input type="checkbox" id="info_break_sw" onchange="devices[focused].break_widgets=this.checked;save_devices()">
          <span class="slider"></span></label>
        </div>
        <div class="cfg_row">
          <label>Show names</label>
          <label class="switch"><input type="checkbox" id="info_names_sw" onchange="devices[focused].show_names=this.checked;save_devices()">
          <span class="slider"></span></label>
        </div>
        <div class="cfg_row">
          <button id="reboot_btn" class="c_btn btn_mini" onclick="reboot_h()"><span class="icon info_icon"></span>Reboot</button>
        </div>
      </div>

      <div class="cfg_col" id="info_topics">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Topics</label>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Version</label>
        </div>
        <div id="info_version"></div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Network</label>
        </div>
        <div id="info_net"></div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Memory</label>
        </div>
        <div id="info_memory"></div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>System</label>
        </div>
        <div id="info_system"></div>
      </div>
    </div>

    <div id="fsbr_edit" class="main_col">
      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Editor</label>
        </div>
        <div class="cfg_row">
          <label id="edit_path"></label>
        </div>
        <div class="cfg_row">
          <label>Wrap text</label>
          <label class="switch"><input type="checkbox" id="editor_wrap" onchange="this.checked?editor_area.classList.remove('c_area_wrap'):editor_area.classList.add('c_area_wrap')"><span class="slider"></span></label>
        </div>
        <div class="cfg_row">
          <textarea rows=20 id="editor_area" class="cfg_inp c_log c_area_wrap"></textarea>
        </div>
        <div class="cfg_row">
          <button id="editor_save" onclick="editor_save()" class="c_btn btn_mini">Save & Upload</button>
          <button onclick="editor_cancel()" class="c_btn btn_mini">Cancel</button>
        </div>
      </div>
    </div>

    <div id="fsbr" class="main_col">
      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>FS Browser</label>
        </div>
        <div id="fs_browser">
          <div id="fsbr_inner"></div>
          <div class="cfg_row">
            <div>
              <button id="fs_format" onclick="format_h()" class="c_btn btn_mini">Format</button>
              <button id="fs_update" onclick="updatefs_h()" class="c_btn btn_mini">Refresh</button>
            </div>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Upload to</label>
        </div>
        <div id="fs_upload">
          <div class="upload_row">
            <input class="cfg_inp inp_wbtn" type="text" id="file_upload_path" value="/">
            <input type="file" id="file_upload" style="display:none" onchange="uploadFile(this.files[0], file_upload_path.value)">
            <button id="file_upload_btn" onclick="file_upload.click()" class="c_btn upl_button">Upload</button>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>OTA FILE</label>
        </div>
        <div id="fs_otaf">
          <div class="cfg_row">
            <div>
              <input type="file" id="ota_upload" style="display:none" onchange="uploadOta(this.files[0], 'flash')">
              <button onclick="ota_upload.click()" class="c_btn btn_mini drop_area" ondrop="uploadOta(event.dataTransfer.files[0], 'flash')">Flash</button>
              <input type="file" id="ota_upload_fs" style="display:none" onchange="uploadOta(this.files[0], 'fs')">
              <button onclick="ota_upload_fs.click()" class="c_btn btn_mini drop_area" ondrop="uploadOta(event.dataTransfer.files[0], 'fs')">Filesystem</button>
            </div>
            <label style="font-size:18px" id="ota_label">IDLE</label>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>OTA URL</label>
        </div>
        <div id="fs_otaurl">
          <div class="upload_row">
            <input class="cfg_inp inp_wbtn" type="text" id="ota_url_f">
            <button id="ota_url_btn" onclick="otaUrl(ota_url_f.value,'flash')" class="c_btn upl_button">Flash</button>
          </div>
          <div class="upload_row">
            <input class="cfg_inp inp_wbtn" type="text" id="ota_url_fs">
            <button id="ota_url_btn" onclick="otaUrl(ota_url_fs.value,'fs')" class="c_btn upl_button">FS</button>
          </div>
        </div>
      </div>

    </div>

    <div id="config" class="cfg_in">
      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label class="cfg_label"><span class="icon cfg_icon"></span>Search</label>
          <div>
            <button class="icon cfg_btn_tab" onclick="discover_all()" title="Find new devices"></button>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head cfg_clickable" onclick="use_ws.click()">
          <label class="cfg_label cfg_clickable" id="ws_label"><span class="icon cfg_icon"></span>WS</label>
          <input type="checkbox" id="use_ws" onchange="update_cfg(this)" style="display:none">
        </div>
        <div id="ws_block" style="display:none">
          <div class="cfg_row" id="http_only_http" style="display:none">
            <span style="color:#c60000">Works only on <strong class="span_btn" onclick="window.location.href = window.location.href.replace('https', 'http')">HTTP</strong>!</span>
          </div>

          <div id="http_settings">
            <div class="cfg_row">
              <label class="cfg_label">My IP</label>
              <div class="cfg_inp_row cfg_inp_row_fix">
                <input class="cfg_inp" type="text" id="local_ip" onchange="update_cfg(this)">
                <div class="cfg_btn_block">
                  <button class="icon cfg_btn" onclick="update_ip_h();update_cfg(EL('local_ip'))"></button>
                </div>
              </div>
            </div>

            <div class="cfg_row">
              <label>Netmask</label>
              <select class="cfg_inp c_inp_block с_inp_fix" id="netmask" onchange="update_cfg(this)"></select>
            </div>
            
            <div class="cfg_row">
              <label class="cfg_label">Add by IP</label>
              <div class="cfg_inp_row cfg_inp_row_fix">
                <input class="cfg_inp" type="text" value="192.168.1.1" id="ws_manual_ip">
                <div class="cfg_btn_block">
                  <button class="icon cfg_btn" onclick="manual_ws_h(ws_manual_ip.value)"></button>
                </div>
              </div>
            </div>

            <div class="cfg_row">
              <label>HTTP hook</label>
              <label class="switch"><input type="checkbox" id="use_hook" onchange="update_cfg(this)"><span class="slider"></span></label>
            </div>
            <!--APP-->
            <span class="notice_block">Disable: <u>${browser()}://flags/#block-insecure-private-network-requests</u></span>
            <!--/APP-->
          </div>
        </div>
      </div>

      <!--NON-ESP-->
      <div class="cfg_col" id="mq_col">
        <div class="cfg_row cfg_head cfg_clickable" onclick="use_mqtt.click()">
          <label class="cfg_label cfg_clickable" id="mqtt_label"><span class="icon cfg_icon"></span>MQTT</label>
          <input type="checkbox" id="use_mqtt" onchange="update_cfg(this);mq_stop()" style="display:none">
        </div>

        <div id="mq_block" style="display:none">

          <div class="cfg_row">
            <label class="cfg_label">Host</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="text" id="mq_host" onchange="update_cfg(this);mq_stop()"></div>
          </div>

          <div class="cfg_row">
            <label class="cfg_label">Port (WS TLS)</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="number" id="mq_port" onchange="update_cfg(this);mq_stop()"></div>
          </div>

          <div class="cfg_row">
            <label class="cfg_label">Login</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="text" id="mq_login" onchange="update_cfg(this);mq_stop()"></div>
          </div>

          <div class="cfg_row">
            <label class="cfg_label">Pass</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="password" id="mq_pass" onchange="update_cfg(this);mq_stop()">
            </div>
          </div>
          <div class="cfg_btn_row">
            <button class="c_btn btn_mini" onclick="mq_start()">Connect</button>
            <button class="c_btn btn_mini" onclick="mq_stop()">Disconnect</button>
          </div>

        </div>
      </div>
      
      <div class="cfg_col" id="serial_col" ${("serial" in navigator) ? '' : 'style="display:none"'}>
        <div class="cfg_row cfg_head cfg_clickable" onclick="use_serial.click()">
          <label class="cfg_label cfg_clickable" id="serial_label"><span class="icon cfg_icon"></span>Serial</label>
          <input type="checkbox" id="use_serial" onchange="serial_change();update_cfg(this)" style="display:none">
        </div>

        <div id="serial_block" style="display:none">
          <div class="cfg_row">
            <label class="cfg_label">Baudrate</label>
            <select class="cfg_inp c_inp_block с_inp_fix" id='baudrate' onchange="update_cfg(this)"></select>
          </div>
          <div class="cfg_row">
            <label class="cfg_label">Port</label>
            <div class="cfg_btn_row">
              <button class="c_btn btn_mini" onclick="serial_select()">Select</button>
              <button id="serial_btn" class="c_btn btn_mini" onclick="serial_toggle()">Connect</button>
            </div>
          </div>
        </div>
      </div>

      <div class="cfg_col" id="bt_col" ${("bluetooth" in navigator) ? '' : 'style="display:none"'}>
        <div class="cfg_row cfg_head cfg_clickable" onclick="use_bt.click()">
          <label class="cfg_label cfg_clickable" id="bt_label"><span class="icon cfg_icon"></span>Bluetooth</label>
          <input type="checkbox" id="use_bt" onchange="update_cfg(this)" style="display:none">
        </div>

        <div id="bt_block" style="display:none">
          <div class="cfg_row">
            <label class="cfg_label" id="bt_device">Not Connected</label>
            <button id="bt_btn" class="c_btn btn_mini" onclick="bt_toggle()">Connect</button>
          </div>
        </div>

      </div>
      <!--/NON-ESP-->

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label class="cfg_label"><span class="icon cfg_icon"></span>Settings</label>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Prefix</label>
          <div class="cfg_inp_row cfg_inp_row_fix">
            <input class="cfg_inp" type="text" id="prefix" onchange="update_cfg(this)">
          </div>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Client ID</label>
          <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="text" id="client_id" onchange="update_cfg(this)" oninput="if(this.value.length>8)this.value=this.value.slice(0,-1)">
          </div>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Theme</label>
          <select class="cfg_inp c_inp_block с_inp_fix" id='theme' onchange="update_cfg(this)"></select>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Main Color</label>
          <select class="cfg_inp c_inp_block с_inp_fix" id='maincolor' onchange="update_cfg(this)"></select>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Font</label>
          <select class="cfg_inp c_inp_block с_inp_fix" id='font' onchange="update_cfg(this)"></select>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">UI Width</label>
          <div class="cfg_inp_row cfg_inp_row_fix">
            <input class="cfg_inp" type="number" id="ui_width" onchange="update_cfg(this);update_theme()">
          </div>
        </div>

        <div class="cfg_row">
          <label>Check updates</label>
          <label class="switch"><input type="checkbox" id="check_upd" onchange="update_cfg(this)"><span class="slider"></span></label>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Settings</label>
          <div class="cfg_btn_row">
            <button class="c_btn btn_mini" onclick="cfg_export()">Export</button>
            <button class="c_btn btn_mini" onclick="cfg_import()">Import</button>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head cfg_clickable" onclick="use_pin.click()">
          <label id="pin_label" class="cfg_label cfg_clickable"><span class="icon cfg_icon"></span>PIN</label>
          <input type="checkbox" id="use_pin" onchange="update_cfg(this)" style="display:none">
        </div>

        <div id="pin_block" style="display:none">
          <div class="cfg_row">
            <label class="cfg_label">PIN</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="password" pattern="[0-9]*" inputmode="numeric"
                id="pin" onchange="this.value=this.value.hashCode();update_cfg(this)" oninput="check_type(this)">
            </div>
          </div>
        </div>
      </div>

      <!--NON-ESP-->
      <div class="cfg_col" id="pwa_block">
        <div class="cfg_row cfg_head">
        <label class="cfg_label"><span class="icon cfg_icon"></span>Web App</label>
          <div class="cfg_btn_row">
            <button class="c_btn btn_mini ${isSSL() ? 'info_btn_dis' : ''}" onclick="pwa_install(false)">HTTP</button>
            <button class="c_btn btn_mini ${!isSSL() ? 'info_btn_dis' : ''}" onclick="pwa_install(true)">HTTPS</button>
          </div>
        </div>
        <!--<span class="notice_block">HTTP app: <b>Local</b> and <b>MQTT</b><br>HTTPS app: only <b>MQTT</b></span>-->
        <span class="notice_block" id="pwa_unsafe">Enable <u>${browser()}://flags/#unsafely-treat-insecure-origin-as-secure</u> and add <u>${window.location.href}</u> to list</span>
      </div>
      <!--/NON-ESP-->

      <div class="cfg_col" id="app_block">
        <div class="cfg_row cfg_head">
        <label class="cfg_label"><span class="icon cfg_icon"></span>App</label>
          <div class="cfg_btn_row">
            <button class="c_btn btn_mini" onclick="openURL('https://play.google.com/store/apps/details?id=ru.alexgyver.GyverHub')">Android</button>
            <button class="c_btn btn_mini" onclick="openURL('https://github.com/GyverLibs/GyverHub/raw/main/app/GyverHub.apk')">.apk</button>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_info">
          Contribution:
          <a href="https://github.com/Simonwep/pickr" target="_blank">Pickr</a>
          <a href="https://github.com/mqttjs/MQTT.js" target="_blank">MQTT.js</a>
          <a href="https://github.com/ghornich/sort-paths" target="_blank">sort-paths</a>
          <a href="https://fontawesome.com/v5/search?o=r&m=free&s=solid" target="_blank">Fontawesome</a>
        </div>
      </div>
    </div>

    <div id="password" class="main_col">
      <div class="pass_inp_inner">
        <input class="cfg_inp pass_inp" type="number" pattern="[0-9]*" inputmode="numeric" id="pass_inp" oninput="pass_type('')">
      </div>
      <div class="cfg_row pass_inner">
        <button class="c_btn pass_btn" onclick="pass_type(1)">1</button>
        <button class="c_btn pass_btn" onclick="pass_type(2)">2</button>
        <button class="c_btn pass_btn" onclick="pass_type(3)">3</button>
      </div>
      <div class="cfg_row pass_inner">
        <button class="c_btn pass_btn" onclick="pass_type(4)">4</button>
        <button class="c_btn pass_btn" onclick="pass_type(5)">5</button>
        <button class="c_btn pass_btn" onclick="pass_type(6)">6</button>
      </div>
      <div class="cfg_row pass_inner">
        <button class="c_btn pass_btn" onclick="pass_type(7)">7</button>
        <button class="c_btn pass_btn" onclick="pass_type(8)">8</button>
        <button class="c_btn pass_btn" onclick="pass_type(9)">9</button>
      </div>
      <div class="cfg_row pass_inner">
        <button class="c_btn pass_btn empty_b"></button>
        <button class="c_btn pass_btn" onclick="pass_type(0)">0</button>
        <button class="c_btn pass_btn red_btn" onclick="pass_inp.value=pass_inp.value.slice(0, -1)">&lt;</button>
      </div>
    </div>
  </div>
  <div id="bottom_space"></div>
  `;
}