const app_title = 'GyverHub';
const non_esp = '__ESP__';
const non_app = '__APP__';
const app_version = '__VER__';

const log_enable = true;
const log_network = false;

const colors = {
  //RED: 0xcb2839,
  ORANGE: 0xd55f30,
  YELLOW: 0xd69d27,
  GREEN: 0x37A93C,
  MINT: 0x25b18f,
  AQUA: 0x2ba1cd,
  BLUE: 0x297bcd,
  VIOLET: 0x825ae7,
  PINK: 0xc8589a,
};
const fonts = [
  'monospace',
  'system-ui',
  'cursive',
  'Arial',
  'Verdana',
  'Tahoma',
  'Trebuchet MS',
  'Georgia',
  'Garamond',
];
const themes = {
  DARK: 0,
  LIGHT: 1
};
const baudrates = [4800, 9600, 19200, 38400, 57600, 74880, 115200, 230400, 250000, 500000, 1000000, 2000000];
const theme_cols = [
  // back/tab/font/font2/dark/thumb/black/scheme/font4/shad/font3
  ['#1b1c20', '#26272c', '#eee', '#ccc', '#141516', '#444', '#0e0e0e', 'dark', '#222', '#000'],
  ['#eee', '#fff', '#111', '#333', '#ddd', '#999', '#bdbdbd', 'light', '#fff', '#000000a3']
];

function isSSL() {
  return window.location.protocol == 'https:';
}
function isLocal() {
  return window.location.href.startsWith('file') || checkIP(window_ip()) || window_ip() == 'localhost';
}
function isApp() {
  return !non_app;
}
function isPWA() {
  return (window.matchMedia('(display-mode: standalone)').matches) || (window.navigator.standalone) || document.referrer.includes('android-app://');
}
function isESP() {
  return !non_esp;
}
function isTouch() {
  return navigator.maxTouchPoints || 'ontouchstart' in document.documentElement;
}

function getMime(name) {
  const mime_table = {
    'avi': 'video/x-msvideo',
    'bin': 'application/octet-stream',
    'bmp': 'image/bmp',
    'css': 'text/css',
    'csv': 'text/csv',
    'gz': 'application/gzip',
    'gif': 'image/gif',
    'html': 'text/html',
    'jpeg': 'image/jpeg',
    'jpg': 'image/jpeg',
    'js': 'text/javascript',
    'json': 'application/json',
    'png': 'image/png',
    'svg': 'image/svg+xml',
    'txt': 'text/plain',
    'wav': 'audio/wav',
    'xml': 'application/xml',
  };
  let ext = name.split('.').pop();
  if (ext in mime_table) return mime_table[ext];
  else return 'text/plain';
}
function EL(id) {
  return document.getElementById(id);
}
function log(text) {
  let texts = text.toString();
  if (!log_network && (texts.includes('discover') || texts.startsWith('Post') || texts.startsWith('Got'))) return;
  console.log(text);
}
function openURL(url) {
  window.open(url, '_blank').focus();
}
function intToCol(val) {
  return "#" + Number(val).toString(16).padStart(6, '0');
}
function intToColA(val) {
  return "#" + Number(val).toString(16).padStart(8, '0');
}
function colToInt(str) {
  return parseInt(str.substr(1), 16);
}
function random(min, max) {
  return Math.floor(Math.random() * (max - min + 1) + min)
}
function randomChar() {
  let code;
  switch (random(0, 2)) {
    case 0: code = random(48, 57); break;
    case 1: code = random(65, 90); break;
    case 2: code = random(97, 122); break;
  }
  return String.fromCharCode(code);
}
function notSupported() {
  alert('Browser not supported');
}
function browser() {
  if (navigator.userAgent.includes("Opera") || navigator.userAgent.includes('OPR')) return 'opera';
  else if (navigator.userAgent.includes("Edg")) return 'edge';
  else if (navigator.userAgent.includes("Chrome")) return 'chrome';
  else if (navigator.userAgent.includes("Safari")) return 'safari';
  else if (navigator.userAgent.includes("Firefox")) return 'firefox';
  else if ((navigator.userAgent.includes("MSIE")) || (!!document.documentMode == true)) return 'IE';
  else return 'unknown';
}
function disableScroll() {
  TopScroll = window.pageYOffset || document.documentElement.scrollTop;
  LeftScroll = window.pageXOffset || document.documentElement.scrollLeft,
    window.onscroll = function () {
      window.scrollTo(LeftScroll, TopScroll);
    };
}
function enableScroll() {
  window.onscroll = function () { };
}
function refreshSpin(val) {
  if (val) EL('icon_refresh').classList.add('spinning');
  else EL('icon_refresh').classList.remove('spinning');
}
function ratio() {
  return window.devicePixelRatio;
}
function resize_h() {
  showGauges();
}
function waitAnimationFrame() {
  return new Promise(res => {
    requestAnimationFrame(() => res());
  });
}
function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

// ========== POPUP ==============
let popupT1 = null, popupT2 = null;
function showPopup(text, color = '#37a93c') {
  if (popupT1) clearTimeout(popupT1);
  if (popupT2) clearTimeout(popupT2);
  EL('notice').innerHTML = text;
  EL('notice').style.background = color;
  EL('notice').style.display = 'block';
  EL('notice').style.animation = "fade-in 0.5s forwards";
  popupT1 = setTimeout(() => { popupT1 = null; EL('notice').style.display = 'none' }, 3500);
  popupT2 = setTimeout(() => { popupT2 = null; EL('notice').style.animation = "fade-out 0.5s forwards" }, 3000);
}
function showPopupError(text) {
  showPopup(text, '#a93737');
}
function showErr(v) {
  EL('head_cont').style.background = v ? 'var(--err)' : 'var(--prim)';
}

// ============ IP ================
/*NON-ESP*/
function getLocalIP() {
  return new Promise(function (resolve, reject) {
    var RTCPeerConnection = window.webkitRTCPeerConnection || window.mozRTCPeerConnection;
    if (!RTCPeerConnection) reject('Not supported');

    var rtc = new RTCPeerConnection({ iceServers: [] });
    var addrs = {};
    addrs["0.0.0.0"] = false;

    function grepSDP(sdp) {
      var hosts = [];
      var finalIP = '';
      sdp.split('\r\n').forEach(function (line) { // c.f. http://tools.ietf.org/html/rfc4566#page-39
        if (~line.indexOf("a=candidate")) {     // http://tools.ietf.org/html/rfc4566#section-5.13
          var parts = line.split(' '),        // http://tools.ietf.org/html/rfc5245#section-15.1
            addr = parts[4],
            type = parts[7];
          if (type === 'host') {
            finalIP = addr;
          }
        } else if (~line.indexOf("c=")) {       // http://tools.ietf.org/html/rfc4566#section-5.7
          var parts = line.split(' '),
            addr = parts[2];
          finalIP = addr;
        }
      });
      return finalIP;
    }

    if (1 || window.mozRTCPeerConnection) {      // FF [and now Chrome!] needs a channel/stream to proceed
      rtc.createDataChannel('', { reliable: false });
    };

    rtc.onicecandidate = function (evt) {
      // convert the candidate to SDP so we can run it through our general parser
      // see https://twitter.com/lancestout/status/525796175425720320 for details
      if (evt.candidate) {
        var addr = grepSDP("a=" + evt.candidate.candidate);
        resolve(addr);
      }
    };
    rtc.createOffer(function (offerDesc) {
      rtc.setLocalDescription(offerDesc);
    }, function (e) { return; });
  });
}
/*/NON-ESP*/
function update_ip_h() {
  /*NON-ESP*/
  if (!Boolean(window.webkitRTCPeerConnection || window.mozRTCPeerConnection)) notSupported();
  else getLocalIP().then((ip) => {
    if (ip.indexOf("local") > 0) alert(`Disable WEB RTC anonymizer: ${browser()}://flags/#enable-webrtc-hide-local-ips-with-mdns`);
    else EL('local_ip').value = ip;
  });
  /*/NON-ESP*/
  if (isESP()) EL('local_ip').value = window_ip();
}
function checkIP(ip) {
  return Boolean(ip.match(/^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])(\.(?!$)|$)){4}$/));
}
function window_ip() {
  let ip = window.location.href.split('/')[2].split(':')[0];
  return checkIP(ip) ? ip : 'error';
}

function getIPs() {
  let ip = EL('local_ip').value;
  if (!checkIP(ip)) {
    showPopupError('Wrong IP!');
    return null;
  }
  let ip_a = ip.split('.');
  let sum_ip = (ip_a[0] << 24) | (ip_a[1] << 16) | (ip_a[2] << 8) | ip_a[3];
  let cidr = Number(hub.cfg.netmask);
  let mask = ~(0xffffffff >>> cidr);
  let network = 0, broadcast = 0, start_ip = 0, end_ip = 0;
  if (cidr === 32) {
    network = sum_ip;
    broadcast = network;
    start_ip = network;
    end_ip = network;
  } else {
    network = sum_ip & mask;
    broadcast = network + (~mask);
    if (cidr === 31) {
      start_ip = network;
      end_ip = broadcast;
    } else {
      start_ip = network + 1;
      end_ip = broadcast - 1;
    }
  }
  let ips = ['192.168.4.1'];
  for (let ip = start_ip; ip <= end_ip; ip++) {
    ips.push(`${(ip >>> 24) & 0xff}.${(ip >>> 16) & 0xff}.${(ip >>> 8) & 0xff}.${ip & 0xff}`);
  }
  return ips;
}

/*
// ============ SHA1 ==============
function SHA1(msg) {
  function rotate_left(n, s) { var t4 = (n << s) | (n >>> (32 - s)); return t4; }; function lsb_hex(val) {
    var str = ''; var i; var vh; var vl; for (i = 0; i <= 6; i += 2) { vh = (val >>> (i * 4 + 4)) & 0x0f; vl = (val >>> (i * 4)) & 0x0f; str += vh.toString(16) + vl.toString(16); }
    return str;
  }; function cvt_hex(val) {
    var str = ''; var i; var v; for (i = 7; i >= 0; i--) { v = (val >>> (i * 4)) & 0x0f; str += v.toString(16); }
    return str;
  }; function Utf8Encode(string) {
    string = string.replace(/\r\n/g, '\n'); var utftext = ''; for (var n = 0; n < string.length; n++) {
      var c = string.charCodeAt(n); if (c < 128) { utftext += String.fromCharCode(c); }
      else if ((c > 127) && (c < 2048)) { utftext += String.fromCharCode((c >> 6) | 192); utftext += String.fromCharCode((c & 63) | 128); }
      else { utftext += String.fromCharCode((c >> 12) | 224); utftext += String.fromCharCode(((c >> 6) & 63) | 128); utftext += String.fromCharCode((c & 63) | 128); }
    }
    return utftext;
  }; var blockstart; var i, j; var W = new Array(80); var H0 = 0x67452301; var H1 = 0xEFCDAB89; var H2 = 0x98BADCFE; var H3 = 0x10325476; var H4 = 0xC3D2E1F0; var A, B, C, D, E; var temp; msg = Utf8Encode(msg); var msg_len = msg.length; var word_array = new Array(); for (i = 0; i < msg_len - 3; i += 4) { j = msg.charCodeAt(i) << 24 | msg.charCodeAt(i + 1) << 16 | msg.charCodeAt(i + 2) << 8 | msg.charCodeAt(i + 3); word_array.push(j); }
  switch (msg_len % 4) { case 0: i = 0x080000000; break; case 1: i = msg.charCodeAt(msg_len - 1) << 24 | 0x0800000; break; case 2: i = msg.charCodeAt(msg_len - 2) << 24 | msg.charCodeAt(msg_len - 1) << 16 | 0x08000; break; case 3: i = msg.charCodeAt(msg_len - 3) << 24 | msg.charCodeAt(msg_len - 2) << 16 | msg.charCodeAt(msg_len - 1) << 8 | 0x80; break; }
  word_array.push(i); while ((word_array.length % 16) != 14) word_array.push(0); word_array.push(msg_len >>> 29); word_array.push((msg_len << 3) & 0x0ffffffff); for (blockstart = 0; blockstart < word_array.length; blockstart += 16) {
    for (i = 0; i < 16; i++)W[i] = word_array[blockstart + i]; for (i = 16; i <= 79; i++)W[i] = rotate_left(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1); A = H0; B = H1; C = H2; D = H3; E = H4; for (i = 0; i <= 19; i++) { temp = (rotate_left(A, 5) + ((B & C) | (~B & D)) + E + W[i] + 0x5A827999) & 0x0ffffffff; E = D; D = C; C = rotate_left(B, 30); B = A; A = temp; }
    for (i = 20; i <= 39; i++) { temp = (rotate_left(A, 5) + (B ^ C ^ D) + E + W[i] + 0x6ED9EBA1) & 0x0ffffffff; E = D; D = C; C = rotate_left(B, 30); B = A; A = temp; }
    for (i = 40; i <= 59; i++) { temp = (rotate_left(A, 5) + ((B & C) | (B & D) | (C & D)) + E + W[i] + 0x8F1BBCDC) & 0x0ffffffff; E = D; D = C; C = rotate_left(B, 30); B = A; A = temp; }
    for (i = 60; i <= 79; i++) { temp = (rotate_left(A, 5) + (B ^ C ^ D) + E + W[i] + 0xCA62C1D6) & 0x0ffffffff; E = D; D = C; C = rotate_left(B, 30); B = A; A = temp; }
    H0 = (H0 + A) & 0x0ffffffff; H1 = (H1 + B) & 0x0ffffffff; H2 = (H2 + C) & 0x0ffffffff; H3 = (H3 + D) & 0x0ffffffff; H4 = (H4 + E) & 0x0ffffffff;
  }
  var temp = cvt_hex(H0) + cvt_hex(H1) + cvt_hex(H2) + cvt_hex(H3) + cvt_hex(H4); return temp.toLowerCase();
}
*/