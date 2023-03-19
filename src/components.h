#pragma once

class Components {
public:
    void addStatus(const String& label) {
        if (!sstat_p) return;
        *sstat_p = label;
    }
    void addCustom(const String& text) {
        if (!sptr) return;
        *sptr += '{';
        *sptr += text;
        *sptr += "},";
    }
    void addButtons(const String& names, const String& labels) {
        if (!sptr) return;
        *sptr += F("{'type':'buttons','name':[");
        addWithQuote(names);
        *sptr += F("],'label':[");
        addWithQuote(labels);
        *sptr += F("]},");
    }
    void addButtonsIcons(const String& names, const String& labels, int size = 45) {
        if (!sptr) return;
        *sptr += F("{'type':'buttons_i','name':[");
        addWithQuote(names);
        *sptr += F("],'label':[");
        addWithQuote(labels);
        *sptr += F("],'size':");
        *sptr += size;
        *sptr += F("},");
    }
    void addButton(const String& name, const String& label) {
        if (!sptr) return;
        *sptr += F("{'type':'buttons','name':['");
        *sptr += name;
        *sptr += F("'],'label':['");
        *sptr += label;
        *sptr += F("']},");
    }
    void addButtonIcon(const String& name, const String& label, int size = 45) {
        if (!sptr) return;
        *sptr += F("{'type':'buttons_i','name':['");
        *sptr += name;
        *sptr += F("'],'label':['");
        *sptr += label;
        *sptr += F("'],'size':");
        *sptr += size;
        *sptr += F("},");
    }
    
    void addLabel(const String& name, const String& label, const String& text, GPcolor color = GPcolor(0)) {
        if (!sptr) return;
        *sptr += F("{'type':'label','value':'");
        *sptr += text;
        *sptr += F("','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','color':'");
        *sptr += color.encode();
        *sptr += F("'},");
    }
    void addLabel(const String& name, const String& label, const String& text, PGM_P col_p) {
        if (!sptr) return;
        GPcolor col(FPSTR(col_p));
        addLabel(name, label, text, col);
    }
    
    void addLED(const String& name, const String& label, GPcolor color = GPcolor(0xff00), const String& icon = "") {
        if (!sptr) return;
        *sptr += F("{'type':'led','value':'");
        *sptr += icon;
        *sptr += F("','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','color':'");
        *sptr += color.encode();
        *sptr += F("'},");
    }
    void addLED(const String& name, const String& label, PGM_P col_p, const String& icon = "") {
        if (!sptr) return;
        GPcolor col(FPSTR(col_p));
        addLED(name, label, col, icon);
    }
    
    void addTitle(const String& label) {
        if (!sptr) return;
        *sptr += F("{'type':'title','value':'");
        *sptr += label;
        *sptr += F("'},");
    }
    
    void addSpacer(int size = 20) {
        if (!sptr) return;
        *sptr += F("{'type':'spacer','size':");
        *sptr += size;
        *sptr += F("},");
    }
    
    void addLine() {
        addSpacer(2);
    }
    void addTabs(const String& name, const String& labels, int selected) {
        if (!sptr) return;
        *sptr += F("{'type':'tabs','name':'");
        *sptr += name;
        *sptr += F("','label':[");
        addWithQuote(labels);
        *sptr += F("],'sel':");
        *sptr += selected;
        *sptr += F("},");
    }

    template <typename T>
    void addInput(const String& name, const String& label, T value) {
        if (!sptr) return;
        *sptr += F("{'type':'input','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','value':'");
        *sptr += value;
        *sptr += F("'},");
    }
    void addPass(const String& name, const String& label, const String& value) {
        if (!sptr) return;
        *sptr += F("{'type':'pass','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','value':'");
        *sptr += value;
        *sptr += F("'},");
    }
    template <typename T>
    void addSlider(const String& name, const String& label, T value, T minv, T maxv) {
        if (!sptr) return;
        *sptr += F("{'type':'slider','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','value':'");
        *sptr += value;
        *sptr += F("','min':'");
        *sptr += minv;
        *sptr += F("','max':'");
        *sptr += maxv;
        *sptr += F("'},");
    }
    void addSwitch(const String& name, const String& label, bool value) {
        if (!sptr) return;
        *sptr += F("{'type':'switch','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','value':");
        *sptr += value ? F("true") : F("false");
        *sptr += F("},");
    }
    void addDate(const String& name, const String& label, GPdate value) {
        if (!sptr) return;
        *sptr += F("{'type':'date','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','value':'");
        *sptr += value.encode();
        *sptr += F("'},");
    }
    void addTime(const String& name, const String& label, GPtime value) {
        if (!sptr) return;
        *sptr += F("{'type':'time','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','value':'");
        *sptr += value.encode();
        *sptr += F("'},");
    }
    void addSelect(const String& name, const String& label, const String& values, int selected) {
        if (!sptr) return;
        *sptr += F("{'type':'list','name':'");
        *sptr += name;
        *sptr += F("','label':'");
        *sptr += label;
        *sptr += F("','value':'");
        *sptr += values;
        *sptr += F("','sel':");
        *sptr += selected;
        *sptr += F("},");
    }
    void addUpdate(const String& names, uint16_t prd = 1000) {
        if (!sptr) return;
        *sptr += F("{'type':'update','value':'");
        *sptr += names;
        *sptr += F("','period':");
        *sptr += prd;
        *sptr += F("},");
    }

    String* sptr = nullptr;
    String* sstat_p = nullptr;
    
private:
    void addWithQuote(const String& s) {
        *sptr += '\'';
        for (uint16_t i = 0; i < s.length(); i++) {
            if (s[i] == ',') *sptr += "','";
            else *sptr += s[i];
        }
        *sptr += '\'';
    }
};
