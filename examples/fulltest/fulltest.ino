// FULL DEMO
#include <Arduino.h>
// #define ATOMIC_FS_UPDATE

#define AP_SSID ""
#define AP_PASS ""

const char* fetch_bytes = "fetch bytes\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi dignissim tellus ligula. Vivamus id lacus ac tortor commodo aliquam. Mauris eget faucibus nunc. Vestibulum tempus eu lorem a dapibus. Nullam ac dapibus ex. Aenean faucibus dapibus porttitor. Sed vel magna id tellus mattis semper. Fusce a finibus ligula. In est turpis, viverra eget libero ut, pretium pellentesque velit. Praesent ultrices elit quis facilisis mattis. Donec eu iaculis est. Sed tempus feugiat ligula non ultricies. Cras a auctor nibh, sed sodales sapien.\n\nSed cursus quam vel egestas rhoncus. Curabitur dignissim lorem sed metus sollicitudin, non faucibus erat interdum. Nunc vitae lobortis dui, mattis dignissim orci. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Duis vel venenatis purus. Nunc luctus leo tincidunt felis efficitur ullamcorper. Aliquam semper rhoncus odio sed porta. Quisque blandit, dui vel imperdiet ultricies, dolor arcu posuere turpis, et gravida ante libero ut ex. Vestibulum sed scelerisque nibh, nec mollis urna. Suspendisse tortor sapien, congue at aliquam vitae, venenatis placerat enim. Interdum et malesuada fames ac ante ipsum primis in faucibus. Nam posuere metus a est commodo finibus. Donec luctus arcu purus, sit amet sodales dolor facilisis id. Nullam consectetur sapien vitae nisi gravida, sed finibus dui hendrerit. In id pretium odio, imperdiet lacinia massa. Morbi quis condimentum ligula.";
const char fetch_pgm[] PROGMEM = "fetch pgm\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi dignissim tellus ligula. Vivamus id lacus ac tortor commodo aliquam. Mauris eget faucibus nunc. Vestibulum tempus eu lorem a dapibus. Nullam ac dapibus ex. Aenean faucibus dapibus porttitor. Sed vel magna id tellus mattis semper. Fusce a finibus ligula. In est turpis, viverra eget libero ut, pretium pellentesque velit. Praesent ultrices elit quis facilisis mattis. Donec eu iaculis est. Sed tempus feugiat ligula non ultricies. Cras a auctor nibh, sed sodales sapien.\n\nSed cursus quam vel egestas rhoncus. Curabitur dignissim lorem sed metus sollicitudin, non faucibus erat interdum. Nunc vitae lobortis dui, mattis dignissim orci. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Duis vel venenatis purus. Nunc luctus leo tincidunt felis efficitur ullamcorper. Aliquam semper rhoncus odio sed porta. Quisque blandit, dui vel imperdiet ultricies, dolor arcu posuere turpis, et gravida ante libero ut ex. Vestibulum sed scelerisque nibh, nec mollis urna. Suspendisse tortor sapien, congue at aliquam vitae, venenatis placerat enim. Interdum et malesuada fames ac ante ipsum primis in faucibus. Nam posuere metus a est commodo finibus. Donec luctus arcu purus, sit amet sodales dolor facilisis id. Nullam consectetur sapien vitae nisi gravida, sed finibus dui hendrerit. In id pretium odio, imperdiet lacinia massa. Morbi quis condimentum ligula.";

#if 1
// #define ATOMIC_FS_UPDATE         // OTA обновление сжатым .gz файлом вместо .bin (для esp)

// BRIDGES
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
// #define GH_NO_HTTP_UPLOAD_PORTAL  // отключить упрощённую загрузку файлов с ip/hub/upload_portal (для esp)
#endif

// #define GH_NO_HTTP
// #define GH_NO_WS
// #define GH_NO_MQTT
#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP", "");

// async tcp
// #include <bridges/esp/async/http_ws.h>
// gh::BridgeHttpWs http_ws(&hub);

// async mqtt
// #include <bridges/esp/async/mqtt.h>
// gh::BridgeMqtt mqtt(&hub);

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

gh::Button btn;
gh::Log hlog;
bool dsbl, nolbl, notab;

void build_common(gh::Builder& b) {
    // b.Title("Common");
    {
        gh::Row r(b, 1, "Common");
        b.Switch(&dsbl).label("disabled");
        b.Switch(&nolbl).label("nolabel");
        b.Switch(&notab).label("notab");
    }
    if (b.changed()) b.refresh();
}
void build_widget(gh::Builder& b) {
    static bool nolabel, notab, square;
    static String label("label"), hint("hint"), suffix("suffix");
    // b.Title("Widget test");
    {
        gh::Row_ r("mycont", b, 1, "Widget test", gh::Colors::Aqua);
        b.Label("Some label").noTab(notab).noLabel(nolabel).square(square).label(label).hint(hint).suffix(suffix).size(3);
        b.Switch(&notab).label("notab");
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
    // b.Title("Layout test");
    {
        gh::SpoilerCol s(b, 0, "f0ad Layout test", gh::Colors::Default, 20);
        {
            gh::Row r(b, 1);
            b.Label("w1").label("test stace  x2");
            b.Label("w2").size(2);
        }
        {
            gh::Row r(b);
            b.Label("ww1").icon("");
            b.Label("ww2").icon("f0ad");
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
}
void build_pairs(gh::Builder& b) {
    {
        gh::Row r(b);
        b.Input_("input", &data).size(2);
        b.Slider_("slider", &data);
        b.Spinner_("spinner", &data);
        b.Switch_("switch", &data);
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
        if (b.Button().label("value").click()) hub.update("tit").value(rndText());
        if (b.Button().label("icon").click()) hub.update("tit").icon(rndIcon());
        if (b.Button().label("color").click()) hub.update("tit").color(rndColor());
        if (b.Button().label("align").click()) hub.update("tit").align(rndAlign());
        if (b.Button().label("font_size").click()) hub.update("tit").fontSize(random(10, 40));
    }
    b.Text(data).disabled(dsbl).noTab(notab).noLabel(nolbl);
    {
        gh::Row r(b);
        b.Display_("disp", "Some\n\"display\\\"").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("disp").value(rndText());
        if (b.Button().label("color").click()) hub.update("disp").color(rndColor());
        if (b.Button().label("font_size").click()) hub.update("disp").fontSize(random(10, 40));
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
        if (b.Button().label("value").click()) hub.update("gag").value(random(100));
        if (b.Button().label("range").click()) hub.update("gag").range(10, 90, 5);
        if (b.Button().label("unit").click()) hub.update("gag").unit("deg");
        if (b.Button().label("color").click()) hub.update("gag").color(rndColor());
    }
    {
        gh::Row r(b);
        b.GaugeRound_("gagr").disabled(dsbl).noTab(notab).noLabel(nolbl).size(2);
        if (b.Button().label("value").click()) hub.update("gagr").value(random(100));
        if (b.Button().label("range").click()) hub.update("gagr").range(10, 90, 5);
        if (b.Button().label("unit").click()) hub.update("gagr").unit("deg");
        if (b.Button().label("color").click()) hub.update("gagr").color(rndColor());
    }
    {
        gh::Row r(b);
        b.GaugeLinear_("gagl").disabled(dsbl).noTab(notab).noLabel(nolbl).size(2);
        if (b.Button().label("value").click()) hub.update("gagl").value(random(100));
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
        if (b.Button().label("value").click()) hub.update("inp").value(rndText());
        if (b.Button().label("color").click()) hub.update("inp").color(rndColor());
        if (b.Button().label("regex").click()) hub.update("inp").regex(GH_NUMBERS);
        if (b.Button().label("maxLen").click()) hub.update("inp").maxLen(5);
    }
    {
        gh::Row r(b);
        static String inp;
        b.Pass_("pass", &inp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("pass").value(rndText());
        if (b.Button().label("color").click()) hub.update("pass").color(rndColor());
        if (b.Button().label("regex").click()) hub.update("pass").regex(GH_NUMBERS);
        if (b.Button().label("maxLen").click()) hub.update("pass").maxLen(5);
    }
    {
        gh::Row r(b);
        static String inp;
        b.InputArea_("inpa", &inp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("inpa").value(rndText());
        if (b.Button().label("maxLen").click()) hub.update("inpa").maxLen(5);
        if (b.Button().label("rows").click()) hub.update("inpa").rows(random(1, 5));
    }
    {
        gh::Row r(b);
        static String tags("123;test;taggg");
        b.Tags_("tags", &tags).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("tags").value("kek;pek;123");
        if (b.Button().label("color").click()) hub.update("tags").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t stamp;
        b.Date_("date", &stamp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("date").value(random(100) * 1000000ul);
        if (b.Button().label("color").click()) hub.update("date").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t stamp;
        b.Time_("time", &stamp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("time").value(random(100) * 1000000ul);
        if (b.Button().label("color").click()) hub.update("time").color(rndColor());
    }
}
void build_active2(gh::Builder& b) {
    {
        gh::Row r(b);
        static uint32_t stamp;
        b.DateTime_("datet", &stamp).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("datet").value(random(100) * 1000000ul);
        if (b.Button().label("color").click()) hub.update("datet").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t sld;
        b.Slider_("sld", &sld).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3).range(1, 500, 10);
        if (b.Button().label("value").click()) hub.update("sld").value(random(100));
        if (b.Button().label("range").click()) hub.update("sld").range(1, 500, 10);
        if (b.Button().label("unit").click()) hub.update("sld").unit("deg");
        if (b.Button().label("color").click()) hub.update("sld").color(rndColor());
        if (b.Button().label("icon").click()) hub.update("sld").icon(rndIcon());
    }
    {
        gh::Row r(b);
        static uint32_t spn;
        b.Spinner_("spn", &spn).disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("spn").value(random(100));
        if (b.Button().label("range").click()) hub.update("spn").range(1, 5, 0.1);
        if (b.Button().label("unit").click()) hub.update("spn").unit("deg");
        // if (b.Button().label("color").click()) hub.update("spn").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint8_t num;
        b.Select_("sel", &num).text("kek;puk;123;hello;azaz").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);
        if (b.Button().label("value").click()) hub.update("sel").value(random(5));
        if (b.Button().label("text").click()) hub.update("sel").text("1;2;3;4;5");
        if (b.Button().label("color").click()) hub.update("sel").color(rndColor());
    }
    {
        gh::Row r(b);
        static uint32_t col;
        b.Color_("col", &col).disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").click()) hub.update("col").value(random(0xffffff));
    }
    {
        gh::Row r(b);
        b.Button_("btn", &btn).disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("icon").click()) hub.update("btn").icon(rndIcon());
        if (b.Button().label("color").click()) hub.update("btn").color(rndColor());
        if (b.Button().label("fontsize").click()) hub.update("btn").fontSize(random(10, 40));
    }
}
void build_active3(gh::Builder& b) {
    {
        gh::Row r(b);
        static bool sw;
        b.Switch_("sw", &sw).disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").click()) hub.update("sw").value(rndBool());
        if (b.Button().label("color").click()) hub.update("sw").color(rndColor());
    }
    {
        gh::Row r(b);
        static bool swi;
        b.SwitchIcon_("swi", &swi).disabled(dsbl).noTab(notab).noLabel(nolbl);
        if (b.Button().label("value").click()) hub.update("swi").value(rndBool());
        if (b.Button().label("color").click()) hub.update("swi").color(rndColor());
        if (b.Button().label("icon").click()) hub.update("swi").icon(rndIcon());
    }
    {
        gh::Row r(b);
        static uint8_t num;
        b.Tabs_("tab", &num).text(";f0ad;kek;puk;123;hello;azaz").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);//.fontSize(40);
        if (b.Button().label("value").click()) hub.update("tab").value(random(5));
        if (b.Button().label("text").click()) hub.update("tab").text("1;2;3;4;5");
        if (b.Button().label("color").click()) hub.update("tab").color(rndColor());
    }
    {
        gh::Row r(b);
        static gh::Flags f;
        b.Flags_("flag", &f).text(";kek;puk;123;hello;azaz").disabled(dsbl).noTab(notab).noLabel(nolbl).size(3);//.fontSize(40);
        // if (b.Button().label("value").click()) hub.update("flag").value(random(5));
        if (b.Button().label("text").click()) hub.update("flag").text("1;2;3;4;5");
        if (b.Button().label("color").click()) hub.update("flag").color(rndColor());
    }
    {
        gh::Row r(b);
        gh::Pos pos;
        b.Joystick(&pos);
        if (pos.changed()) Serial.println(String("joy: ") + pos.x + ',' + pos.y);
        b.Joystick(nullptr, true, true).color(gh::Colors::Red);
    }
    {
        gh::Row r(b);
        gh::Pos pos;
        b.Dpad(&pos);
        if (pos.changed()) Serial.println(String("joy: ") + pos.x + ',' + pos.y);
        b.Space();
    }
}
void build_ffile(gh::Builder& b) {
    {
        gh::Row r(b);
        b.uiRow(R"([{"id":"labelka","type":"label","value":"ui"}])");
        b.uiRow("/ui.json").size(3);

        static int spinka = 20;

        if (b.Dummy_("spinka", &spinka).click()) Serial.println(b.build.value);
        if (b.Dummy_("knopka").click()) {
            if (b.build.value == 0) hub.update("labelka").value(rndText());
            if (b.build.value == 0) hub.update("labelka2").value(rndText());
        }
    }

    b.Image_("img", "/image.jpg");
    if (b.beginRow()) {
        if (b.Button().label("refresh").click()) hub.update("img").update();
        if (b.Button().label("file").click()) hub.update("img").value("ag.png");
        b.endRow();
    }
    b.Space();

    b.Table_("table", "/table.csv", "10,20,30", "left,right,right");
    if (b.beginRow()) {
        if (b.Button().label("refresh").click()) hub.update("table").update();
        if (b.Button().label("text").click()) hub.update("table").value("kek,pek,hello");
        if (b.Button().label("file").click()) hub.update("table").value("/table.csv");
        b.endRow();
    }
    b.Space();

    b.TextFile_("textf", "/text.txt").rows(6);
    if (b.beginRow()) {
        if (b.Button().label("refresh").click()) hub.update("textf").update();
        if (b.Button().label("file").click()) hub.update("textf").value("/data.dat");
        if (b.Button().label("git").click()) hub.update("textf").value("https://github.com/GyverLibs/GyverHub-plugins/blob/main/plugins/test.js");
        b.endRow();
    }
    b.Space();

    b.HTML_("html", "/html.html");
    if (b.beginRow()) {
        if (b.Button().label("text").click()) hub.update("html").value(R"(<div><input type="text"><button>kek</button></div>)");
        if (b.Button().label("file").click()) hub.update("html").value("/html.html");
        b.endRow();
    }
}
void build_custom(gh::Builder& b) {
    b.Plugin("myinput", "/myWidget.js");
    b.Plugin("myinput_change", "https://github.com/GyverLibs/GyverHub-plugins/blob/main/plugins/test.js");

    {
        gh::Row r(b);
        b.Widget_("myinp", "myinput").size(4);
        if (b.Button().label("text").click()) hub.update("myinp").value("hello");
    }

    b.Widget_("myinp2", "myinput_change");
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
        int w = 400, h = 500;
        gh::Pos pos;
        b.Canvas_("cv", w, h, &pos).size(3);
        gh::Canvas cv(b);
        cv.background(200, 200, 200);
        cv.stroke(0);
        cv.strokeWeight(1);
        for (int x = 0; x <= w; x += 25) {
            cv.line(x, 0, x, h);
        }
        cv.strokeWeight(2);
        for (int x = 0; x <= w; x += 50) {
            cv.line(x, 0, x, h);
        }
        cv.stroke(255, 255, 255);
        cv.strokeWeight(1);
        for (int y = 0; y <= h; y += 25) {
            cv.line(0, y, w, y);
        }
        cv.strokeWeight(2);
        for (int y = 0; y <= h; y += 50) {
            cv.line(0, y, w, y);
        }

        cv.stroke(255, 0, 0);
        cv.point(75, 25);

        cv.noStroke();
        cv.fill(255, 0, 0);

        cv.rectMode(cv::CORNER);
        cv.rect(0, 0, 50, 50);

        cv.rectMode(cv::CORNERS);
        cv.rect(100, 0, 150, 50, 10);

        cv.rectMode(cv::CENTER);
        cv.rect(225, 25, 50, 50, 10, 10, 20, 20);

        cv.rectMode(cv::RADIUS);
        cv.rect(325, 25, 25, 25);

        cv.ellipseMode(cv::CORNER);
        cv.ellipse(0 + 50, 0 + 50, 50, 50);

        cv.ellipseMode(cv::CORNERS);
        cv.ellipse(100 + 50, 0 + 50, 150 + 50, 50 + 50);

        cv.ellipseMode(cv::CENTER);
        cv.ellipse(225 + 50, 25 + 50, 50, 50);

        cv.ellipseMode(cv::RADIUS);
        cv.ellipse(325 + 50, 25 + 50, 25, 25);

        cv.stroke(0, 255, 0);
        cv.strokeWeight(20);
        cv.strokeCap(cv::SQUARE);
        cv.line(50, 100 + 25, 100, 100 + 25);

        cv.strokeCap(cv::PROJECT);
        cv.line(50 + 100, 100 + 25, 100 + 100, 100 + 25);

        cv.strokeCap(cv::ROUND);
        cv.line(50 + 100 + 100, 100 + 25, 100 + 100 + 100, 100 + 25);

        cv.fill(0);
        cv.noStroke();
        cv.rectMode(cv::CENTER);
        cv.push();
        cv.translate(350, 150 + 25);
        cv.rotate(PI / 4);
        cv.square(0, 0, 50);
        cv.pop();

        cv.stroke(0, 0, 0, 100);
        cv.fill(255, 255, 0);
        cv.strokeWeight(15);
        cv.strokeJoin(cv::MITER);
        cv.rectMode(cv::CORNER);
        cv.rect(50, 150, 50, 50);

        cv.strokeJoin(cv::BEVEL);
        cv.rect(50 + 100, 150, 50, 50);

        cv.strokeJoin(cv::ROUND);
        cv.rect(50 + 100 + 100, 150, 50, 50);

        cv.textSize(25);
        cv.fill(150, 0, 150);
        cv.noStroke();
        cv.textAlign(cv::LEFT, cv::BOTTOM);
        cv.text("aqdAQD", 0, 250);

        cv.textAlign(cv::CENTER, cv::BOTTOM);
        cv.text("aqdAQD", 0 + 150, 250);

        cv.textAlign(cv::RIGHT, cv::BOTTOM);
        cv.text("aqdAQD", 0 + 100 + 100 + 100, 250);

        //
        cv.textAlign(cv::LEFT, cv::TOP);
        cv.text("aqdAQD", 0, 250);

        cv.textAlign(cv::CENTER, cv::TOP);
        cv.text("aqdAQD", 0 + 150, 250);

        cv.textAlign(cv::RIGHT, cv::TOP);
        cv.text("aqdAQD", 0 + 100 + 100 + 100, 250);

        //
        cv.textAlign(cv::LEFT, cv::CENTER);
        cv.text("aqdAQD", 0, 250 + 50);

        cv.textAlign(cv::CENTER, cv::CENTER);
        cv.text("aqdAQD", 0 + 150, 250 + 50);

        cv.textAlign(cv::RIGHT, cv::CENTER);
        cv.text("aqdAQD", 0 + 100 + 100 + 100, 250 + 50);

        //
        cv.textAlign(cv::LEFT, cv::BASELINE);
        cv.text("aqdAQD", 0, 250 + 50 + 50);

        cv.textAlign(cv::CENTER, cv::BASELINE);
        cv.text("aqdAQD", 0 + 150, 250 + 50 + 50);

        cv.textAlign(cv::RIGHT, cv::BASELINE);
        cv.text("aqdAQD", 0 + 100 + 100 + 100, 250 + 50 + 50);

        //
        cv.noFill();
        cv.stroke(0);
        cv.strokeWeight(5);
        cv.beginShape();
        cv.vertex(325, 225);
        cv.vertex(375, 225);
        cv.vertex(350, 225 + 50);
        cv.endShape();

        cv.beginShape();
        cv.vertex(325, 225 + 50);
        cv.vertex(375, 225 + 50);
        cv.vertex(350, 225 + 50 + 50);
        cv.endShape(true);

        cv.beginShape();
        cv.fill(255, 0, 0);
        cv.vertex(325, 225 + 50 + 50);
        cv.vertex(375, 225 + 50 + 50);
        cv.vertex(350, 225 + 50 + 50 + 50);
        cv.endShape(true);

        cv.noFill();
        cv.bezier(50, 400, 150, 350, 250, 450, 350, 400);

        cv.beginShape();
        cv.vertex(50, 450);
        cv.vertex(150, 450);
        cv.bezierVertex(200, 350, 200, 550, 250, 450);
        cv.endShape();

        cv.arc(300, 400, 75, 50, 0, HALF_PI);

        cv.triangle(0, 500, 25, 450, 50, 500);
        cv.quadrangle(25, 450, 50, 400, 25, 350, 0, 400);

        // for update
        cv.fill(0, 255, 0);
        cv.strokeWeight(2);

        if (pos.changed()) {
            Serial.println(String("canvas 1: ") + pos.x + ',' + pos.y);

            gh::CanvasUpdate cv("cv", &hub);
            cv.circle(pos.x, pos.y, 10);
            cv.send();
        }
        b.endRow();
    }
    {
        gh::Row r(b);
        b.Canvas_("cv2", 400, 300).size(4);
        gh::Canvas cv(b);
        cv.image("/image.jpg", 0, 0, 400);
        // cv.image("/errorimage.jpg", 0, 0, 400);
        // cv.image("http://errorimage.jpg", 0, 0, 400);
        {
            gh::Col c(b);
            if (b.Button().label("from web").click()) {
                gh::CanvasUpdate cv("cv2", &hub);
                cv.clear();
                cv.image("https://alexgyver.ru/wp-content/uploads/2021/03/kits.jpg", 0, 0, 400);
                cv.circle(100, 100, 50);
                cv.send();
            }
            if (b.Button().label("from fs").click()) {
                gh::CanvasUpdate cv("cv2", &hub);
                cv.clear();
                cv.image("/image.jpg", 0, 0, 400);
                cv.circle(100, 100, 50);
                cv.send();
            }
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
void build_location(gh::Builder& b) {
    {
        gh::Row r(b, 3);
        gh::Geo moscow("55.754994;37.623288");
        static gh::Geo geo = moscow;

        b.Map_("map", &geo).size(3);
        gh::Canvas map(b);
        map.fill(0x0000ff);
        map.circle(moscow, 200);  // круг 200 метров радиуса

        if (geo.changed()) {
            Serial.println(String("geo: ") + geo.lat + ',' + geo.lon);

            gh::CanvasUpdate cv("map", &hub);
            cv.fill(gh::Colors::Red);
            cv.circle(geo, 50);
            cv.send();
        }
        {
            gh::Col c(b);
            if (b.Button().label("moscow").click()) hub.update("map").value(moscow);
            if (b.Button().label("spb").click()) hub.update("map").value(gh::Geo("59.939769;30.310496"));
            if (b.Button().label("request").click()) hub.requestLocation(&b.build.client);
        }
    }
}
void updateTick() {
    if (hub.menu == 9) {
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
    b.Menu(F("Basic;Passive;Active;Pairs;From file;Widget;MQTT;Popup;Canvas;Update;Location"));

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
            build_custom(b);
            break;
        case 6:
            build_mqtt(b);
            break;
        case 7:
            build_popup(b);
            break;
        case 8:
            build_canvas(b);
            break;
        case 9:
            build_update(b);
            break;
        case 10:
            build_location(b);
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

    // async tcp
    // hub.addBridge(&http_ws);

    // async mqtt
    // mqtt.config("test.mosquitto.org", 1883);
    // hub.addBridge(&mqtt);
    
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

    hub.onPing([](gh::Client& client) {
        // hub.sendCLI("hello!");
    });

    hub.onRequest([](gh::Request& req) -> bool {
        return 1;
        Serial.print("Request: ");
        Serial.print(gh::readConnection(req.client.connection));
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

    hub.onLocation([](gh::Location loc) {
        Serial.println(String("Location: ") + loc.lat + ',' + loc.lon);
        // поставить маркер и подвинуть сюда карту
        hub.update("map").value(loc);
    });

#ifdef GH_ESP_BUILD
    hub.onReboot([](gh::Reboot r) {
        Serial.print("Reboot: ");
        Serial.println(gh::readReboot(r));
    });

    hub.onFetch([](gh::Fetcher& f) {
        if (f.path == "/fetch_file.txt") f.fetchFile("/fetch_file.txt");
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
    // Serial.println(hub.getUI());
}

void loop() {
    data.tick();
    hub.tick();

    if (btn.click()) Serial.println("btn click");
    if (btn.state()) {
        static gh::Timer tmr(500);
        if (tmr) Serial.println("btn hold");
    }

    updateTick();
}