// FULL DEMO
#include <Arduino.h>
// #define ATOMIC_FS_UPDATE

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

const char _ui[] PROGMEM = R"json({"type":"row","width":1,"data":[{"id":"inp","type":"input"},{"id":"sld","type":"slider"},{"id":"btn","type":"button"}]})json";
const char* fetch_bytes = "fetch bytes\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi dignissim tellus ligula. Vivamus id lacus ac tortor commodo aliquam. Mauris eget faucibus nunc. Vestibulum tempus eu lorem a dapibus. Nullam ac dapibus ex. Aenean faucibus dapibus porttitor. Sed vel magna id tellus mattis semper. Fusce a finibus ligula. In est turpis, viverra eget libero ut, pretium pellentesque velit. Praesent ultrices elit quis facilisis mattis. Donec eu iaculis est. Sed tempus feugiat ligula non ultricies. Cras a auctor nibh, sed sodales sapien.\n\nSed cursus quam vel egestas rhoncus. Curabitur dignissim lorem sed metus sollicitudin, non faucibus erat interdum. Nunc vitae lobortis dui, mattis dignissim orci. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Duis vel venenatis purus. Nunc luctus leo tincidunt felis efficitur ullamcorper. Aliquam semper rhoncus odio sed porta. Quisque blandit, dui vel imperdiet ultricies, dolor arcu posuere turpis, et gravida ante libero ut ex. Vestibulum sed scelerisque nibh, nec mollis urna. Suspendisse tortor sapien, congue at aliquam vitae, venenatis placerat enim. Interdum et malesuada fames ac ante ipsum primis in faucibus. Nam posuere metus a est commodo finibus. Donec luctus arcu purus, sit amet sodales dolor facilisis id. Nullam consectetur sapien vitae nisi gravida, sed finibus dui hendrerit. In id pretium odio, imperdiet lacinia massa. Morbi quis condimentum ligula.";
const char fetch_pgm[] PROGMEM = "fetch pgm\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi dignissim tellus ligula. Vivamus id lacus ac tortor commodo aliquam. Mauris eget faucibus nunc. Vestibulum tempus eu lorem a dapibus. Nullam ac dapibus ex. Aenean faucibus dapibus porttitor. Sed vel magna id tellus mattis semper. Fusce a finibus ligula. In est turpis, viverra eget libero ut, pretium pellentesque velit. Praesent ultrices elit quis facilisis mattis. Donec eu iaculis est. Sed tempus feugiat ligula non ultricies. Cras a auctor nibh, sed sodales sapien.\n\nSed cursus quam vel egestas rhoncus. Curabitur dignissim lorem sed metus sollicitudin, non faucibus erat interdum. Nunc vitae lobortis dui, mattis dignissim orci. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Duis vel venenatis purus. Nunc luctus leo tincidunt felis efficitur ullamcorper. Aliquam semper rhoncus odio sed porta. Quisque blandit, dui vel imperdiet ultricies, dolor arcu posuere turpis, et gravida ante libero ut ex. Vestibulum sed scelerisque nibh, nec mollis urna. Suspendisse tortor sapien, congue at aliquam vitae, venenatis placerat enim. Interdum et malesuada fames ac ante ipsum primis in faucibus. Nam posuere metus a est commodo finibus. Donec luctus arcu purus, sit amet sodales dolor facilisis id. Nullam consectetur sapien vitae nisi gravida, sed finibus dui hendrerit. In id pretium odio, imperdiet lacinia massa. Morbi quis condimentum ligula.";

#if 1
// #define ATOMIC_FS_UPDATE         // OTA обновление сжатым .gz файлом вместо .bin (для esp)

// BRIDGES
// #define GH_NO_STREAM  // отключить встроенный модуль связи stream
// #define GH_NO_HTTP    // отключить встроенный модуль связи http (для esp)
// #define GH_NO_WS      // отключить встроенный модуль связи ws (для esp)
// #define GH_NO_MQTT    // отключить встроенный модуль связи mqtt (для esp)

// #define GH_BRIDGE_AMOUNT 5       // количество мостов подключения
// #define GH_WS_PORT 81            // websocket порт
// #define GH_MQTT_RECONNECT 10000  // период переподключения встроенного MQTT (для esp)

// MODULES
// #define GH_NO_PAIRS    // отключить поддержку типа Pairs
// #define GH_NO_GET      // отключить отправку на get-топик (mqtt)
// #define GH_NO_INFO     // отключить info билдер
// #define GH_NO_MODULES  // отключить "модули" (все включены)
// #define GH_NO_REQUEST  // отключить поддержку запросов

// FS
// #define GH_NO_FS                 // отключить файловую систему (для esp)
// #define GH_FETCH_CHUNK_SIZE 512  // размер чанка при скачивании с платы (для esp)
// #define GH_UPL_CHUNK_SIZE 512    // размер чанка при загрузке на плату (для esp)
// #define GH_FS_DEPTH 5            // глубина сканирования файловой системы (esp32)

// TRANSFER
// #define GH_NO_FETCH    // отключить скачивание файлов (для esp)
// #define GH_NO_UPLOAD   // отключить загрузку файлов (для esp)
// #define GH_NO_OTA      // отключить ОТА обновления (для esp)
// #define GH_NO_OTA_URL  // отключить ОТА по ссылке (для esp)
// #define GH_REBOOT_DELAY 1500    // задержка перед перезагрузкой после OTA (для esp)

// PORTAL
// #define GH_FILE_PORTAL     // загружать сайт из памяти есп (положи файлы в папку /hub/) (для esp)
// #define GH_INCLUDE_PORTAL  // включить сайт в память программы, ~50кБ (не нужно загружать файлы в память) (для esp)

// HTTP
// #define GH_PUBLIC_PATH "/www"          // путь к папке с файлами с HTTP доступом (для esp)
// #define GH_HTTP_PORT 80                // http порт (для esp)
// #define GH_CACHE_PRD "max-age=604800"  // период кеширования файлов для портала (для esp)

// #define GH_NO_HTTP_TRANSFER       // отключить загрузку/скачивание/ОТА по http (для esp)
// #define GH_NO_HTTP_DNS            // отключить DNS сервер (для esp)
// #define GH_NO_HTTP_OTA            // отключить ОТА обновления по http (для esp)
// #define GH_NO_HTTP_PUBLIC         // отключить доступ к файлам по http c ip/www (для esp)
// #define GH_NO_HTTP_FETCH          // отключить скачивание файлов по http (для esp)
// #define GH_NO_HTTP_UPLOAD         // отключить загрузку файлов по http (для esp)
// #define GH_NO_HTTP_UPLOAD_PORTAL  // упрощённую загрузку файлов с ip/hub/upload_portal (для esp)
#endif

#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP", "");

#include <PairsFile.h>
PairsFile data(&GH_FS, "/data.dat", 3000);
// Pairs data;

uint32_t rndColor() {
    uint32_t colors[] = {
        0xcb2839,
        0xd55f30,
        0xd69d27,
        0x37A93C,
        0x25b18f,
        0x2ba1cd,
        0x297bcd,
        0x825ae7,
        0xc8589a,
    };
    return colors[random(9)];
}
String rndText() {
    String s;
    uint8_t len = random(5, 12);
    for (uint8_t i = 0; i < len; i++) {
        s += (char)random('a', 'z');
    }
    return s;
}
gh::Align rndAlign() {
    static uint8_t i;
    if (++i >= 3) i = 0;
    return (gh::Align)i;
}
const char* rndIcon() {
    static uint8_t i;
    if (++i >= 4) i = 0;
    const char* icons[] = {
        "",
        "f0ad",
        "f5ce",
        "",
    };
    return icons[i];
}
bool rndBool() {
    static bool v;
    return v = !v;
}

gh::Log hlog;
bool dsbl, nolbl, notab;

void build_common(gh::Builder& b) {
    b.Title("Common");
    {
        gh::Row r(b);
        b.Switch(&dsbl).label("disabled");
        b.Switch(&nolbl).label("nolabel");
        b.Switch(&notab).label("notab");
    }
    if (b.changed()) b.refresh();
}
void build_widget(gh::Builder& b) {
    static bool nolabel, notab, square;
    static String label("label"), hint("hint"), suffix("suffix");
    b.Title("Widget test");
    {
        gh::Row r(b);
        b.Label("Some label").noTab(notab).noLabel(nolabel).square(square).label(label).hint(hint).suffix(suffix).size(3);
        b.Switch(&notab).label("notab").size(1);
        b.Switch(&nolabel).label("nolabel");
        b.Switch(&square).label("square");
    }
    {
        gh::Row r(b);
        b.Input(&label).label("label");
        b.Input(&hint).label("hint");
        b.Input(&suffix).label("suffix");
    }
    if (b.changed()) b.refresh();
}
void build_layout(gh::Builder& b) {
    b.Title("Layout test");
    {
        gh::Row r(b);
        b.Label("w1").size(1);
        b.Label("w2").size(2);
    }
    {
        gh::Row r(b);
        b.Label("ww1");
        b.Label("ww2");
        {
            gh::Col c(b);
            b.Label("www1");
            b.Label("www2");
        }
    }
    {
        gh::Row r(b);
        b.Label("wwww1");
    }
}
void build_pairs(gh::Builder& b) {
    {
        gh::Row r(b);
        b.Input_("input", &data).size(2);
        b.Slider_("slider", &data);
        b.Spinner_("spinner", &data);
        b.Switch_("switch", &data).size(1);
    }
    b.Text(data);
    if (b.changed()) b.refresh();
}
void build_passive(gh::Builder& b) {
    build_common(b);
    b.Title("Passive");
    {
        gh::Row r(b);
        b.Title_("tit", "Some title").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("tit").value(rndText());
        if (b.Button().label("icon").click()) hub.update("tit").icon(rndIcon());
        if (b.Button().label("color").click()) hub.update("tit").color(rndColor());
        if (b.Button().label("align").click()) hub.update("tit").align(rndAlign());
        if (b.Button().label("fsize").click()) hub.update("tit").fontSize(random(10, 40));
    }
    b.Text(data).disabled(dsbl).noTab(notab).noLabel(nolbl);
    {
        gh::Row r(b);
        b.Display_("disp", "Some\n\"display\\\"").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("disp").value(rndText());
        if (b.Button().label("color").click()) hub.update("disp").color(rndColor());
        if (b.Button().label("fsize").click()) hub.update("disp").fontSize(random(10, 40));
        if (b.Button().label("rows").click()) hub.update("disp").rows(random(1, 5));
    }
    {
        gh::Row r(b);
        b.LED_("led").disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").click()) hub.update("led").value(rndBool());
        if (b.Button().label("color").click()) hub.update("led").color(rndColor());
    }
    {
        gh::Row r(b);
        b.Icon_("icon").disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").click()) hub.update("icon").value(rndBool());
        if (b.Button().label("color").click()) hub.update("icon").color(rndColor());
    }
    {
        gh::Row r(b);
        b.Gauge_("gag").disabled(dsbl).noTab(notab).noLabel(nolbl).size(2);
        if (b.Button().label("value").size(1).click()) hub.update("gag").value(random(100));
        if (b.Button().label("range").click()) hub.update("gag").range(10, 90, 5);
        if (b.Button().label("unit").click()) hub.update("gag").unit("deg");
        if (b.Button().label("color").click()) hub.update("gag").color(rndColor());
    }
    {
        gh::Row r(b);
        b.GaugeRound_("gagr").disabled(dsbl).noTab(notab).noLabel(nolbl).size(2);
        if (b.Button().label("value").size(1).click()) hub.update("gagr").value(random(100));
        if (b.Button().label("range").click()) hub.update("gagr").range(10, 90, 5);
        if (b.Button().label("unit").click()) hub.update("gagr").unit("deg");
        if (b.Button().label("color").click()) hub.update("gagr").color(rndColor());
    }
    {
        gh::Row r(b);
        b.GaugeLinear_("gagl").disabled(dsbl).noTab(notab).noLabel(nolbl).size(2);
        if (b.Button().label("value").size(1).click()) hub.update("gagl").value(random(100));
        if (b.Button().label("range").click()) hub.update("gagl").range(10, 90, 5);
        if (b.Button().label("unit").click()) hub.update("gagl").unit("deg");
        if (b.Button().label("color").click()) hub.update("gagl").color(rndColor());
    }
    {
        // gh::CSVFile t("/table.csv", 3, 3);
        gh::CSV t(3, 3);
        t.add("kek").add(123).add(3.14, 2).endRow();
        t.add(3).add(4).add(5).endRow();
        t.add(6).add(7).add(8).endRow();
        b.Table(t.str).disabled(dsbl).noTab(notab).noLabel(nolbl);
    }
    b.Log(&hlog).disabled(dsbl).noTab(notab).noLabel(nolbl);
}
void build_active1(gh::Builder& b) {
    build_common(b);
    b.Title("Active");
    {
        gh::Row r(b);
        static String inp;
        b.Input_("inp", &inp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("inp").value(rndText());
        if (b.Button().label("color").click()) hub.update("inp").color(rndColor());
        if (b.Button().label("regex").click()) hub.update("inp").regex(GH_NUMBERS);
        if (b.Button().label("maxLen").click()) hub.update("inp").maxLen(5);
    }
    {
        gh::Row r(b);
        static String inp;
        b.Pass_("pass", &inp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("pass").value(rndText());
        if (b.Button().label("color").click()) hub.update("pass").color(rndColor());
        if (b.Button().label("regex").click()) hub.update("pass").regex(GH_NUMBERS);
        if (b.Button().label("maxLen").click()) hub.update("pass").maxLen(5);
    }
    {
        gh::Row r(b);
        static String inp;
        b.InputArea_("inpa", &inp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("inpa").value(rndText());
        if (b.Button().label("maxLen").click()) hub.update("inpa").maxLen(5);
        if (b.Button().label("rows").click()) hub.update("inpa").rows(random(1, 5));
    }
    {
        gh::Row r(b);
        static uint32_t stamp;
        b.Date_("date", &stamp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("date").value(random(100) * 1000000ul);
        if (b.Button().label("color").click()) hub.update("date").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t stamp;
        b.Time_("time", &stamp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("time").value(random(100) * 1000000ul);
        if (b.Button().label("color").click()) hub.update("time").color(rndColor());
    }
}
void build_active2(gh::Builder& b) {
    {
        gh::Row r(b);
        static uint32_t stamp;
        b.DateTime_("datet", &stamp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("datet").value(random(100) * 1000000ul);
        if (b.Button().label("color").click()) hub.update("datet").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t sld;
        b.Slider_("sld", &sld).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("sld").value(random(100));
        if (b.Button().label("range").click()) hub.update("sld").range(1, 5, 0.1);
        if (b.Button().label("unit").click()) hub.update("sld").unit("deg");
        if (b.Button().label("color").click()) hub.update("sld").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t spn;
        b.Spinner_("spn", &spn).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("spn").value(random(100));
        if (b.Button().label("range").click()) hub.update("spn").range(1, 5, 0.1);
        if (b.Button().label("unit").click()) hub.update("spn").unit("deg");
        // if (b.Button().label("color").click()) hub.update("spn").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint8_t num;
        b.Select_("sel", &num).text("kek;puk;123;hello;azaz").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("sel").value(random(5));
        if (b.Button().label("text").click()) hub.update("sel").text("1;2;3;4;5");
        if (b.Button().label("color").click()) hub.update("sel").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t col;
        b.Select_("col", &col).disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").size(1).click()) hub.update("col").value(random(0xffffff));
    }
    {
        gh::Row r(b);
        static gh::Button btn;
        b.Button_("btn").disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("icon").size(1).click()) hub.update("btn").icon(rndIcon());
        if (b.Button().label("color").click()) hub.update("btn").color(rndColor());
        if (b.Button().label("fontsize").click()) hub.update("btn").fontSize(random(10, 40));
    }
}
void build_active3(gh::Builder& b) {
    {
        gh::Row r(b);
        static bool sw;
        b.Switch_("sw", &sw).disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").size(1).click()) hub.update("sw").value(rndBool());
        if (b.Button().label("color").click()) hub.update("sw").color(rndColor());
    }
    {
        gh::Row r(b);
        static bool swi;
        b.SwitchIcon_("swi", &swi).disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").size(1).click()) hub.update("swi").value(rndBool());
        if (b.Button().label("color").click()) hub.update("swi").color(rndColor());
        if (b.Button().label("icon").click()) hub.update("swi").icon(rndIcon());
    }
    {
        gh::Row r(b);
        static uint8_t num;
        b.Tabs_("tab", &num).text("kek;puk;123;hello;azaz").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").size(1).click()) hub.update("tab").value(random(5));
        if (b.Button().label("text").click()) hub.update("tab").text("1;2;3;4;5");
        if (b.Button().label("color").click()) hub.update("tab").color(rndColor());
    }
    {
        gh::Row r(b);
        static gh::Flags f;
        b.Flags_("flag", &f).text("kek;puk;123;hello;azaz").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        // if (b.Button().label("value").size(1).click()) hub.update("flag").value(random(5));
        if (b.Button().label("text").click()) hub.update("flag").text("1;2;3;4;5");
        if (b.Button().label("color").click()) hub.update("flag").color(rndColor());
    }
    {
        gh::Row r(b);
        b.Joystick();
        b.Joystick(nullptr, true, true);
    }
    {
        gh::Row r(b);
        b.Dpad();
        b.Space();
    }
}
void build_ffile(gh::Builder& b) {
    b.Image_("img", "/text.jpg");
    if (b.beginRow()) {
        gh::Update upd(&hub);
        upd.update("img");
        if (b.Button().label("refresh").click()) upd.widget.action();
        upd.send();
        b.endRow();
    }

    b.Table_("table", "/table.csv", "10,20,30", "left,right,right").hint("Some hint");  // FILE
    if (b.beginRow()) {
        gh::Update upd(&hub);
        upd.update("table");
        if (b.Button().label("refresh").click()) upd.widget.action();
        if (b.Button().label("text").click()) upd.widget.value("kek,pek,hello");
        if (b.Button().label("file").click()) upd.widget.value("/table.csv");  // FILE
        upd.send();
        b.endRow();
    }

    b.TextFile_("textf", "/data.dat");
    if (b.beginRow()) {
        gh::Update upd(&hub);
        upd.update("textf");
        if (b.Button().label("refresh").click()) upd.widget.action();
        if (b.Button().label("file").click()) upd.widget.value("/table.csv");  // FILE
        upd.send();
        b.endRow();
    }

    b.HTML("/html.html");  // FILE
    b.JS("/js.js");        // FILE
    b.CSS("/css.css");     // FILE

    static String inp0, inp1;
    static int sld0, sld1;
    b.addJSON_File("/ui0.json");
    b.addJSON_File("/ui1.json");
    b.Hook_("inp0", &inp0);
    b.Hook_("sld0", &sld0);
    b.Hook_("inp1", &inp1);
    b.Hook_("sld1", &sld1);

    static String inp;
    static int sld = 1234;
    b.addJSON_P(_ui);
    b.Hook_("inp", &inp);
    b.Hook_("sld", &sld);
}
void build_mqtt(gh::Builder& b) {
    static String s1, s2;
    if (b.Input_("mqinp", &s1).click()) {
        Serial.print("mqinp set to: ");
        Serial.println(b.build.value);
    }
    if (b.Dummy_("dummy", &s2).click()) {
        Serial.print("dummy set to: ");
        Serial.println(b.build.value);
    }
    if (b.Button().click()) hub.sendGet("dummy", String(random(100) / 10.0, 2));
}
void build_popup(gh::Builder& b) {
    b.beginRow();
    if (b.Button().label("push").click()) hub.sendPush(F("push!"));
    if (b.Button().label("notice").click()) hub.sendNotice(F("notice!"));
    if (b.Button().label("alert").click()) hub.sendAlert(F("alert!"));

    bool cfm;
    if (b.Confirm_("cfm", &cfm).text("confirm text").click()) {
        Serial.print("confirm: ");
        Serial.println(cfm);
    }

    String prom;
    if (b.Prompt_("prom", &prom).text("prompt text").click()) {
        Serial.print("prompt: ");
        Serial.println(prom);
    }
    if (b.Button().label("confirm").click()) hub.sendAction("cfm");
    if (b.Button().label("prompt").click()) hub.sendAction("prom");
    b.endRow();
}
void build_canvas(gh::Builder& b) {
    if (b.beginRow()) {
        gh::Canvas cv;
        gh::Pos pos;
        b.BeginCanvas_("cv", 400, 300, &cv, &pos);
        cv.stroke(0xff0000);
        cv.strokeWeight(5);
        cv.line(0, 0, -1, -1);
        cv.line(0, -1, -1, 0);
        b.EndCanvas();

        if (pos.changed()) {
            Serial.print(pos.x);
            Serial.print(',');
            Serial.println(pos.y);

            gh::CanvasUpdate cv("cv", &hub);
            cv.circle(pos.x, pos.y, 10);
            cv.send();
        }
        b.endRow();
    }
    {
        gh::Row r(b);
        gh::Canvas cv;
        gh::Pos pos;
        b.BeginCanvas_("cv2", 400, 300, &cv, &pos);
        cv.drawImage("/image.jpg", 0, 0, 400);
        b.EndCanvas();

        if (pos.changed()) {
            Serial.print(pos.x);
            Serial.print(',');
            Serial.println(pos.y);

            gh::CanvasUpdate cv("cv2", &hub);
            cv.circle(pos.x, pos.y, 10);
            cv.send();
        }
    }
}
void build_update(gh::Builder& b) {
    {
        gh::Row r(b);
        static String s;
        b.Input(&s);
        b.Input_("lbl3", &s);
    }
    {
        gh::Row r(b);
        b.Label_("lbl1");
        b.Label_("lbl2");
    }
    {
        gh::Row r(b);
        if (b.Button().click()) hub.update("lbl1").value("label 1");
        if (b.Button().click()) hub.sendUpdate("lbl2", "label2");
    }
}
void updateTick() {
    if (hub.menu == 8) {
        static gh::Timer tmr(2000);
        if (tmr) {
            gh::Update upd(&hub);
            upd.update("lbl1").value(rndText());
            upd.update("lbl2").value(rndText());
            upd.send();

            hub.sendUpdate("lbl3");
        }
    }
}

void build(gh::Builder& b) {
    if (b.build.isSet()) hlog.println(b.build.name);
    b.Menu(F("Basic;Passive;Active;Pairs;From file;MQTT;Popup;Canvas;Update"));

    switch (hub.menu) {
        case 0:
            build_widget(b);
            build_layout(b);
            break;
        case 1:
            build_passive(b);
            break;
        case 2:
            build_active1(b);
            build_active2(b);
            build_active3(b);
            break;
        case 3:
            build_pairs(b);
            break;
        case 4:
            build_ffile(b);
            break;
        case 5:
            build_mqtt(b);
            break;
        case 6:
            build_popup(b);
            break;
        case 7:
            build_canvas(b);
            break;
        case 8:
            build_update(b);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    hlog.begin();

#ifdef GH_ESP_BUILD
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    // hub.mqtt.config("test.mosquitto.org", 1883);
#endif

    hub.onBuild(build);

    hub.onCLI([](String str) {
        Serial.println(str);
        hub.sendCLI(str + "!");
    });

    hub.onInfo([](gh::Info& info) {
        switch (info.type) {
            case gh::Info::Type::Version:
                info.add("ur mom", "v3.14");
                break;
            case gh::Info::Type::Network:
                info.add(F("5G"), "50%");
                break;
            case gh::Info::Type::Memory:
                info.add(F("SD"), "10 GB");
                info.add(F("Int"), 100500);
                break;
            case gh::Info::Type::System:
                info.add(F("Battery"), 3.63, 2);
                break;
        }
    });

    hub.onRequest([](gh::Request& req) -> bool {
        Serial.print("Request: ");
        Serial.print(gh::readConnection(req.client.connection()));
        Serial.print(',');
        Serial.print(req.client.id);
        Serial.print(',');
        Serial.print(gh::readCMD(req.cmd));
        Serial.print(',');
        Serial.print(req.name);
        Serial.print(',');
        Serial.print(req.value);
        Serial.println();
        return 1;
    });

#ifdef GH_ESP_BUILD
    hub.onReboot([](gh::Reboot r) {
        Serial.println(gh::readReboot(r));
    });

    hub.onFetch([](gh::Fetcher& f) {
        // if (f.path == "/fetch_file.txt") f.fetchFile("/fetch_file.txt");
        if (f.path == "/fetch_bytes.txt") f.fetchBytes((uint8_t*)fetch_bytes, strlen(fetch_bytes));
        if (f.path == "/fetch_pgm.txt") f.fetchBytes_P((uint8_t*)fetch_pgm, strlen_P(fetch_pgm));
    });

    hub.onUpload([](String& path) {
        Serial.print("Uploaded: ");
        Serial.println(path);
        if (path == "/data.dat") data.begin();  // refresh from file
    });
#endif

    hub.begin();

    data.begin();
    // Serial.println(hub.getUI());
    // Serial.println(hub.getValues());
    hub.setBufferSize(2000);
}

void loop() {
    data.tick();
    hub.tick();

    updateTick();
}