#pragma once
#include <Arduino.h>

#define GHC_TMR_INTERVAL 0
#define GHC_TMR_TIMEOUT 1
#define GHC_TMR_INTERVAL_OFF 2
#define GHC_TMR_TIMEOUT_OFF 3

#ifdef GH_ESP_BUILD
#include <functional>
#endif

namespace gh {

class Timer {
#ifdef GH_ESP_BUILD
    typedef std::function<void()> TimerCallback;
#else
    typedef void (*TimerCallback)();
#endif

    typedef unsigned long (*Uptime)();

   public:
    // конструктор

    // пустой
    Timer() {}

    // указать время. Таймер сам запустится в режиме интервала!
    Timer(uint32_t ms) {
        setTime(ms);
        startInterval();
    }

    // указать время. Таймер сам запустится в режиме интервала!
    Timer(uint32_t ms, uint32_t seconds, uint32_t minutes = 0, uint32_t hours = 0, uint32_t days = 0) {
        setTime(ms, seconds, minutes, hours, days);
        startInterval();
    }

    // =========== MANUAL ===========
    // установить время
    void setTime(uint32_t time) {
        prd = time;
    }
    void setTime(uint32_t ms, uint32_t seconds, uint32_t minutes = 0, uint32_t hours = 0, uint32_t days = 0) {
        prd = seconds;
        if (minutes) prd += minutes * 60ul;
        if (hours) prd += hours * 3600ul;
        if (days) prd += days * 86400ul;
        if (prd) prd *= 1000ul;
        prd += ms;
    }

    // получить период
    uint32_t getTime() {
        return prd;
    }

    // установить функцию опроса времени millis/micros/свою unsigned long
    void setSource(Uptime source) {
        uptime = source;
    }

    // удерживать фазу (умолч. false) [false: tmr = uptime, true: tmr += prd]
    void keepPhase(bool keep) {
        phaseF = keep;
    }

    // запустить в режиме интервала (передать true для режима таймаута)
    // void start(bool timeout = false) {
    //     mode = timeout ? GHC_TMR_TIMEOUT : GHC_TMR_INTERVAL;
    //     restart();
    // }

    // запустить в режиме таймаута
    void startTimeout() {
        mode = GHC_TMR_TIMEOUT;
        restart();
    }

    // запустить в режиме таймаута
    void startTimeout(uint32_t time) {
        prd = time;
        startTimeout();
    }

    // запустить в режиме интервала
    void startInterval() {
        mode = GHC_TMR_INTERVAL;
        restart();
    }

    // запустить в режиме интервала
    void startInterval(uint32_t time) {
        prd = time;
        startInterval();
    }

    // перезапустить в текущем режиме
    void restart() {
        if (prd) {
            tmr = uptime();
            if (mode > 1) mode -= 2;
        }
    }

    // остановить
    void stop() {
        if (mode < 2) mode += 2;
    }

    // состояние (запущен?)
    bool state() {
        return (mode < 2);
    }

    // подключить функцию-обработчик вида void f()
    void attach(TimerCallback callback) {
        this->callback = callback;
    }

    // отключить функцию-обработчик
    void detach() {
        callback = nullptr;
    }

    // осталось времени
    uint32_t timeLeft() {
        return state() ? (uptime() - tmr) : 0;
    }

    // тикер, вызывать в loop. Вернёт true, если сработал
    bool tick() {
        if (state() && uptime() - tmr >= prd) {
            if (callback) callback();
            if (mode == GHC_TMR_INTERVAL) _reset();
            else stop();
            return 1;
        }
        return 0;
    }

    operator bool() {
        return tick();
    }

    // ========= JAVASCRIPT =========
    // // запуск в режиме интервала с указанием времени в мс
    // void setInterval(uint32_t ms) {
    //     prd = ms;
    //     startInterval();
    // }

    // // запуск в режиме интервала с указанием обработчика и времени в мс
    // void setInterval(TimerCallback callback, uint32_t ms) {
    //     setInterval(ms);
    //     this->callback = callback;
    // }

    // // запуск в режиме таймаута с указанием времени в мс
    // void setTimeout(uint32_t ms) {
    //     prd = ms;
    //     startTimeout();
    // }

    // // запуск в режиме таймаута с указанием обработчика и времени в мс
    // void setTimeout(TimerCallback callback, uint32_t ms) {
    //     setTimeout(ms);
    //     this->callback = callback;
    // }

   private:
    uint8_t mode = GHC_TMR_INTERVAL_OFF;
    uint32_t tmr = 0, prd = 0;
    TimerCallback callback = nullptr;
    Uptime uptime = millis;
    bool phaseF = false;

    void _reset() {
        if (phaseF) tmr += prd;
        else tmr = uptime();
    }
};

}  // namespace gh