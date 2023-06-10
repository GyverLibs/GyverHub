function render_main(v) {
  head_cont.innerHTML = `
  <div class="title" id="title_cont">
    <div class="title_inn">
      <div id="title_row" class="title_row" onclick="back_h()">
        <span class="icon i_hover back_btn" id="back"></span>
        <span><span id="title"></span><sup id="conn"></sup><span class='version' id='version'>${v}</span></span>
      </div>

      <div class="head_btns">
        <span id='head_err' style="display:none"><strike>MQTT</strike></span>
        <span class="icon i_hover" id='icon_refresh' onclick="refresh_h()"></span>
        <span class="icon i_hover" id='icon_fsbr' style="display:none" onclick="fsbr_h()"></span>
        <span class="icon i_hover" id='icon_info' style="display:none" onclick="info_h()"></span>
        <span class="icon i_hover" id='icon_cfg' style="display:none" onclick="config_h()"></span>
      </div>
    </div>
  </div>
  `;
  /*NON-ESP*/
  test_cont.innerHTML = `<div class="test_text">А тут пока ничего нет. Но будет онлайн-тест интерфейса, в котором можно будет поиграться и проверить свой билд без загрузки прошивки</div>`;
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

  footer_cont.innerHTML = `
  <div class="footer_inner">
    <a href="https://alexgyver.ru/support_alex/" target="_blank"><span class="icon info_icon info_icon_u"></span>Support</a>
    <!--NON-ESP-->
    <a style="cursor:pointer" onclick="test_h()"><span class="icon info_icon info_icon_u"></span>Test</a>
    <!--/NON-ESP-->
    <a href="https://github.com/GyverLibs/GyverHub/wiki" target="_blank"><span class="icon info_icon info_icon_u"></span>Docs</a>
  </div>
  `;

  main_cont.innerHTML = `
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
      </div>

      <div class="cfg_col" id="info_topics">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Topics</label>
        </div>
      </div>

      <div class="cfg_col" id="info_version">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Version</label>
        </div>
      </div>

      <div class="cfg_col" id="info_esp">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>ESP info</label>
        </div>
      </div>
    </div>

    <div id="fsbr" class="main_col">
      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>FS Browser</label>
        </div>
        <div id="fsbr_inner"></div>
        <div class="cfg_row">
          <button onclick="format_h()" class="c_btn btn_mini">Format</button>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Upload</label>
        </div>
        <div class="upload_row">
          <input class="cfg_inp" type="text" id="file_upload_path" value="/">
          <input type="file" id="file_upload" style="display:none" onchange="uploadFile(this)">
          <button id="file_upload_btn" onclick="file_upload.click()" class="c_btn upl_button">Upload</button>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>OTA FILE</label>
        </div>
        <div class="cfg_row">
          <div>
            <input type="file" id="ota_upload" style="display:none" onchange="uploadOta(this, 'flash')">
            <button onclick="ota_upload.click()" class="c_btn btn_mini">Flash</button>
            <input type="file" id="ota_upload_fs" style="display:none" onchange="uploadOta(this, 'fs')">
            <button onclick="ota_upload_fs.click()" class="c_btn btn_mini">Filesystem</button>
          </div>
          <label style="font-size:18px" id="ota_label">IDLE</label>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>OTA URL</label>
        </div>
        <div class="upload_row">
          <input class="cfg_inp" type="text" id="ota_url_f">
          <button id="ota_url_btn" onclick="otaUrl(ota_url_f.value,'flash')" class="c_btn upl_button">Flash</button>
        </div>
        <div class="upload_row">
          <input class="cfg_inp" type="text" id="ota_url_fs">
          <button id="ota_url_btn" onclick="otaUrl(ota_url_fs.value,'fs')" class="c_btn upl_button">FS</button>
        </div>
      </div>

    </div>

    <div id="config" class="cfg_in">
      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Search</label>
          <div>
            <button class="icon cfg_btn_tab" onclick="discover_all()" title="Find new devices"></button>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label id="ws_label"><span class="icon cfg_icon"></span>WS</label>
          <label class="switch"><input type="checkbox" id="use_ws" onchange="update_cfg(this)"><span class="slider"></span></label>
        </div>
        <div id="ws_block" style="display:none">
          <div class="cfg_row" id="http_only_http" style="display:none">
            <span style="color:#c60000">Works only on <strong class="span_btn" onclick="window.location.href = window.location.href.replace('https', 'http')">HTTP</strong>!</span>
          </div>

          <div id="http_settings">
            <div class="cfg_row">
              <label class="cfg_label">My IP</label>
              <div class="cfg_inp_row cfg_inp_row_fix">
                <input class="cfg_inp" type="text" id="client_ip" onchange="update_cfg(this)">
                <div class="cfg_btn_block">
                  <button class="icon cfg_btn" onclick="update_ip();update_cfg(EL('client_ip'))"></button>
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
          
            <span class="notice_block">Disable: <u>${browser()}://flags/#block-insecure-private-network-requests</u></span>
          </div>
        </div>
      </div>

      <!--NON-ESP-->
      <div class="cfg_col" id="mq_col">
        <div class="cfg_row cfg_head">
          <label id="mqtt_label"><span class="icon cfg_icon"></span>MQTT</label>
          <label class="switch"><input type="checkbox" id="use_mqtt" onchange="update_cfg(this);mq_change(this.checked)"><span class="slider"></span></label>
        </div>

        <div id="mq_block" style="display:none">

          <div class="cfg_row">
            <label class="cfg_label">Host</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="text" id="mq_host" onchange="update_cfg(this);mq_change()"></div>
          </div>

          <div class="cfg_row">
            <label class="cfg_label">Port (WS TLS)</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="number" id="mq_port" onchange="update_cfg(this);mq_change()"></div>
          </div>

          <div class="cfg_row">
            <label class="cfg_label">Login</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="text" id="mq_login" onchange="update_cfg(this);mq_change()"></div>
          </div>

          <div class="cfg_row">
            <label class="cfg_label">Pass</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="password" id="mq_pass" onchange="update_cfg(this);mq_change()">
            </div>
          </div>

        </div>
      </div>
      
      <div class="cfg_col" id="serial_col" style="display:none">
        <div class="cfg_row cfg_head">
          <label id="serial_label"><span class="icon cfg_icon"></span>Serial</label>
          <label class="switch"><input type="checkbox" id="use_serial" onchange="update_cfg(this)"><span class="slider"></span></label>
        </div>

        <div id="serial_block" style="display:none">
        </div>

      </div>

      <div class="cfg_col" id="bt_col" style="display:none">
        <div class="cfg_row cfg_head">
          <label id="bt_label"><span class="icon cfg_icon"></span>Bluetooth</label>
          <label class="switch"><input type="checkbox" id="use_bt" onchange="update_cfg(this)"><span class="slider"></span></label>
        </div>

        <div id="bt_block" style="display:none">
        </div>

      </div>
      <!--/NON-ESP-->

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label><span class="icon cfg_icon"></span>Settings</label>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Prefix</label>
          <div class="cfg_inp_row cfg_inp_row_fix">
            <input class="cfg_inp" type="text" id="prefix" onchange="update_cfg(this)">
          </div>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Client ID</label>
          <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="text" id="hub_id" onchange="update_cfg(this)" oninput="if(this.value.length>8)this.value=this.value.slice(0,-1)">
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
            <input class="cfg_inp" type="number" id="ui_width" onchange="update_cfg(this);updateTheme()">
          </div>
        </div>

        <div class="cfg_row">
          <label class="cfg_label">Settings</label>
          <div>
            <button class="c_btn btn_mini" onclick="cfg_export()">Export</button>
            <button class="c_btn btn_mini" onclick="cfg_import()">Import</button>
          </div>
        </div>
      </div>

      <div class="cfg_col">
        <div class="cfg_row cfg_head">
          <label id="pin_label"><span class="icon cfg_icon"></span>PIN</label>
          <label class="switch"><input type="checkbox" id="use_pin" onchange="update_cfg(this)"><span class="slider"></span></label>
        </div>

        <div id="pin_block" style="display:none">
          <div class="cfg_row">
            <label class="cfg_label">PIN</label>
            <div class="cfg_inp_row cfg_inp_row_fix"><input class="cfg_inp" type="password" pattern="[0-9]*" inputmode="numeric"
                id="hub_pin" onchange="this.value=this.value.hashCode();update_cfg(this)" oninput="check_type(this)">
            </div>
          </div>
        </div>
      </div>

      <!--NON-ESP-->
      <div class="cfg_col" id="app_block">
        <div class="cfg_row cfg_head">
        <label><span class="icon cfg_icon"></span>App</label>
          <div>
            <button class="c_btn btn_mini ${isSSL() ? 'info_btn_dis' : ''}" onclick="pwa_install(false)">HTTP</button>
            <button class="c_btn btn_mini ${!isSSL() ? 'info_btn_dis' : ''}" onclick="pwa_install(true)">HTTPS</button>
          </div>
        </div>
        <!--<span class="notice_block">HTTP app: <b>Local</b> and <b>MQTT</b><br>HTTPS app: only <b>MQTT</b></span>-->
        <span class="notice_block" id="pwa_unsafe">Enable <u>${browser()}://flags/#unsafely-treat-insecure-origin-as-secure</u> and add <u>${window.location.href}</u> to list</span>
      </div>
      <!--/NON-ESP-->

      <div class="cfg_col">
        <div class="cfg_info">
          Contribution:
          <a href="https://github.com/Simonwep/pickr" target="_blank">Pickr</a>
          <a href="https://github.com/mqttjs/MQTT.js" target="_blank">MQTT.js</a>
          <a href="https://github.com/ghornich/sort-paths" target="_blank">sort-paths</a>
          <a href="https://fontawesome.com/v5/cheatsheet/free/solid" target="_blank">Fontawesome</a>
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