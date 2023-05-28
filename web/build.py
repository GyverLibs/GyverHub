# GyverHUB Web Builder

version = '0.25b'
fa_url = 'https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.14.0/webfonts/fa-solid-900.ttf'

js_files = [
    'src/include/main.js',
    'src/include/utils.js',
    'src/include/devices.js',
    'src/include/connection.js',
    'src/include/fs.js',
    'src/include/parser.js',
    'src/include/ui.js',
    'src/include/test.js',
    'src/include/sort-paths.min.js',
    'src/include/mqtt.min.js',
    'src/include/pickr.min.js',
]

js_files_esp = [
    'src/include/main.js',
    'src/include/utils.js',
    'src/include/devices.js',
    'src/include/connection.js',
    'src/include/fs.js',
    'src/include/parser.js',
    'src/include/ui.js',
    'src/include/sort-paths.min.js',
    'src/include/pickr.min.js',
]

css_files = [
    'src/include/style.css',
    'src/include/nano.min.css',
]

sw_cache = '''
  '/',
  '/fa-solid-900.ttf',
  '/favicon.svg',
  '/index.html',
  '/script.js',
  '/style.css',
'''

copy_web = [
    'favicon.svg',
    'index.html',
    'manifest.json',
    'sw.js',
    'icons/icon-192x192.png',
    'icons/icon-256x256.png',
    'icons/icon-384x384.png',
    'icons/icon-512x512.png',
]

inc_min = '''
  <script src="script.js?__VER__="></script>
  <link href="style.css?__VER__=" rel="stylesheet">
'''

##############################################################

from rcssmin import cssmin  # pip install rcssmin
from rjsmin import jsmin    # pip install rjsmin
import gzip
import shutil
import os
import re
import base64

##############################################################

if os.path.exists('esp'): shutil.rmtree('esp')
if os.path.exists('host'): shutil.rmtree('host')
if os.path.exists('local'): shutil.rmtree('local')

os.mkdir('esp')
os.mkdir('host')
os.mkdir('host/icons')
os.mkdir('local')

###############################################################
###                           HOST                          ###
###############################################################

metrika = '''
  <script type="text/javascript">
    (function (m, e, t, r, i, k, a) {
        m[i] = m[i] || function () { (m[i].a = m[i].a || []).push(arguments) };
        m[i].l = 1 * new Date();
        for (var j = 0; j < document.scripts.length; j++) { if (document.scripts[j].src === r) { return; } }
        k = e.createElement(t), a = e.getElementsByTagName(t)[0], k.async = 1, k.src = r, a.parentNode.insertBefore(k, a)
    })
        (window, document, "script", "https://mc.yandex.ru/metrika/tag.js", "ym");
    ym(93507215, "init", {clickmap: true, trackLinks: true, accurateTrackBounce: true});
  </script>
  <noscript>
    <div><img src="https://mc.yandex.ru/watch/93507215" style="position:absolute; left:-9999px;" alt="" /></div>
  </noscript>
'''

shutil.copyfile('src/include/fa-solid-900.ttf', 'host/fa-solid-900.ttf')

for file in copy_web:
    shutil.copyfile('src/' + file, 'host/' + file)

with open('host/index.html', "r+") as f:
    data = f.read()
    data = re.sub(r'<!--INC-->([\s\S]*?)<!--\/INC-->', inc_min, data)
    data = re.sub(r'<!--METRIKA-->', metrika, data)
    data = re.sub(r'__VER__', version, data)
    data = re.sub(r'<!--([\s\S]*?)-->', '', data)
    data = re.sub(r'<!--\/([\s\S]*?)-->', '', data)
    data = "".join([s for s in data.strip().splitlines(True) if s.strip()])
    f.seek(0)
    f.write(data)
    f.truncate()

with open('host/sw.js', "r+") as f:
    data = f.read()
    data = re.sub(r'__VER__', version, data)
    data = re.sub(r'\'__CACHE__\'', sw_cache, data)
    f.seek(0)
    f.write(data)
    f.truncate()

# JS
js_min = ''
for file in js_files:
    with open(file, 'r') as f:
        read = f.read()
        if ('.min.' not in file): read = jsmin(read)
        js_min += read + '\n'

js_min = js_min.replace('__VER__', version)
js_min = re.sub(r'(^\s+)', '' , js_min, flags=re.MULTILINE)

with open('host/script.js', 'w') as f:
    f.write(js_min)

# CSS
css_min = ''
for file in css_files:
    with open(file, 'r') as f:
        read = f.read()
        if ('.min.' not in file): read = cssmin(read)
        css_min += read + '\n'

with open('host/style.css', 'w') as f:
    f.write(css_min)

#################################################################
###                           LOCAL                           ###
#################################################################
fa_b64 = 'data:font/truetype;charset=utf-8;base64,'
with open("src/include/fa-solid-900.ttf", "rb") as f:
    fa_b64 += (base64.b64encode(f.read())).decode('ascii')

icon_b64 = "<link rel='icon' href='data:image/svg+xml;base64,"
with open("src/favicon.svg", "rb") as f:
    icon_b64 += (base64.b64encode(f.read())).decode('ascii') + "'>"

shutil.copyfile('src/index.html', 'local/GyverHUB.html')

inc_local = '<style>\n' + css_min.replace('url(fa-solid-900.ttf)', 'url(' + fa_b64 + ')') + '\n</style>\n'
inc_local += '<script>\n' + js_min + '\n</script>\n'

with open('local/GyverHUB.html', "r+") as f:
    data = f.read()
    data = re.sub(r'<!--INC-->([\s\S]*?)<!--\/INC-->', '__INC__', data)
    data = data.replace('__INC__', inc_local)
    data = re.sub(r'<!--ICON-->([\s\S]*?)<!--\/ICON-->', icon_b64, data)
    data = re.sub(r'<!--PWA-->([\s\S]*?)<!--\/PWA-->', '', data)
    data = re.sub(r'<!--METRIKA-->', '', data)
    data = re.sub(r'__VER__', version, data)
    data = re.sub(r'<!--([\s\S]*?)-->', '', data)
    data = re.sub(r'<!--\/([\s\S]*?)-->', '', data)
    data = "".join([s for s in data.strip().splitlines(True) if s.strip()])
    f.seek(0)
    f.write(data)
    f.truncate()

###############################################################
###                           ESP                           ###
###############################################################
# JS
js_min = ''
for file in js_files_esp:
    with open(file, 'r') as f:
        read = f.read()
        read = re.sub(r'\/\*NON-ESP\*\/([\s\S]*?)\/\*\/NON-ESP\*\/', '', read)
        read = re.sub(r'<!--NON-ESP-->([\s\S]*?)<!--\/NON-ESP-->', '', read)
        if ('.min.' not in file): read = jsmin(read)
        js_min += read + '\n'

js_min = js_min.replace('__VER__', version)
js_min = js_min.replace('__ESP__', '')
js_min = re.sub(r'(^\s+)', '' , js_min, flags=re.MULTILINE)

with open('esp/script.js', 'w') as f: f.write(js_min)
with open('esp/script.js', 'rb') as f_in, gzip.open('esp/script.js.gz', 'wb') as f_out: f_out.writelines(f_in)
os.remove("esp/script.js")

# CSS
css_min = ''
for file in css_files:
    with open(file, 'r') as f:
        read = f.read()
        read = re.sub(r'\/\*NON-ESP\*\/([\s\S]*?)\/\*\/NON-ESP\*\/', '', read)
        read = read.replace('url(fa-solid-900.ttf)', 'url(' + fa_url + ')')
        if ('.min.' not in file): read = cssmin(read)
        css_min += read + '\n'

with open('esp/style.css', 'w') as f: f.write(css_min)
with open('esp/style.css', 'rb') as f_in, gzip.open('esp/style.css.gz', 'wb') as f_out: f_out.writelines(f_in)
os.remove("esp/style.css")

# INDEX
shutil.copyfile('src/index.html', 'esp/index.html')
with open('esp/index.html', "r+") as f:
    data = f.read()
    data = re.sub(r'<!--INC-->([\s\S]*?)<!--\/INC-->', inc_min, data)
    data = re.sub(r'<!--PWA-->([\s\S]*?)<!--\/PWA-->', '', data)
    data = re.sub(r'<!--METRIKA-->', '', data)
    data = re.sub(r'__VER__', version, data)
    data = re.sub(r'<!--([\s\S]*?)-->', '', data)
    data = re.sub(r'<!--\/([\s\S]*?)-->', '', data)
    data = "".join([s for s in data.strip().splitlines(True) if s.strip()])
    f.seek(0)
    f.write(data)
    f.truncate()

with open('esp/index.html', 'rb') as f_in, gzip.open('esp/index.html.gz', 'wb') as f_out: f_out.writelines(f_in)
os.remove("esp/index.html")

# OTHER
'''
#with open('local/fa-solid-900.ttf', 'rb') as f_in, gzip.open('esp/fa-solid-900.ttf.gz', 'wb') as f_out: f_out.writelines(f_in)
#with open('local/test.html', 'rb') as f_in, gzip.open('esp/test.html.gz', 'wb') as f_out: f_out.writelines(f_in)
#with open('local/favicon.svg', 'rb') as f_in, gzip.open('esp/favicon.svg.gz', 'wb') as f_out: f_out.writelines(f_in)
with open('local/index.html', 'rb') as f_in, gzip.open('esp/index.html.gz', 'wb') as f_out: f_out.writelines(f_in)
with open('esp/index.html.gz', "rb") as f: ba = bytearray(f.read())

index_h = '#pragma once\n'
index_h += '#define hub_index_gz_len ' + str(len(ba)) + '\n\n'
index_h += 'const uint8_t hub_index_gz[] PROGMEM = {\n\t'
count = 0
for b in ba:
    index_h += "0x{:02x}".format(b) + ', '
    count += 1
    if (count % 16 == 0): index_h += '\n\t'

index_h += '\n};'
with open('esp/index.h', "w") as f:
    f.write(index_h)

os.remove("esp/index.html.gz")
'''