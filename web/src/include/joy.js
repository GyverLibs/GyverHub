let Joystick = (function (cont, color, auto, exp, callback) {
    // based on https://github.com/bobboteck/JoyStick
    cont = EL('joy#' + cont);
    if (!cont) return;
    cont.style.touchAction = "none";

    let ratio = window.devicePixelRatio;
    let cv = document.createElement("canvas");
    let size = cont.clientWidth;
    cv.style.width = size + 'px';
    cv.style.height = size + 'px';
    size *= ratio;
    cv.width = size;
    cv.height = size;
    cv.style.cursor = 'pointer';
    cont.appendChild(cv);

    let cx = cv.getContext("2d");
    let r = size * 0.23;
    let R = size * 0.4;
    let centerX = size / 2;
    let centerY = size / 2;
    let movedX = centerX;
    let movedY = centerY;
    let pressed = 0;

    if ("ontouchstart" in document.documentElement) {
        cv.addEventListener("touchstart", onTouchStart, false);
        document.addEventListener("touchmove", onTouchMove, false);
        document.addEventListener("touchend", onTouchEnd, false);
    } else {
        cv.addEventListener("mousedown", onMouseDown, false);
        document.addEventListener("mousemove", onMouseMove, false);
        document.addEventListener("mouseup", onMouseUp, false);
    }

    function adjust(color, ratio) {
        return '#' + color.replace(/^#/, '').replace(/../g, color => ('0' + Math.min(255, Math.max(0, Math.floor(parseInt(color, 16) * (ratio + 1)))).toString(16)).substr(-2));
    }
    function constrain(val, min, max) {
        return val < min ? min : (val > max ? max : val);
    }

    function onTouchStart(event) {
        pressed = 1;
    }

    function onTouchMove(event) {
        if (pressed && event.targetTouches[0].target === cv) {
            movedX = event.targetTouches[0].pageX * ratio;
            movedY = event.targetTouches[0].pageY * ratio;
            if (cv.offsetParent.tagName.toUpperCase() === "BODY") {
                movedX -= cv.offsetLeft * ratio;
                movedY -= cv.offsetTop * ratio;
            } else {
                movedX -= cv.offsetParent.offsetLeft * ratio;
                movedY -= cv.offsetParent.offsetTop * ratio;
            }
            redraw();
        }
    }

    function onTouchEnd(event) {
        if (auto) {
            movedX = centerX;
            movedY = centerY;
            redraw();
        }
        if (!event.targetTouches.length) pressed = 0;
    }

    function onMouseDown(event) {
        pressed = 1;
        document.body.style.userSelect = 'none';
        document.body.style.cursor = 'pointer';
    }

    function onMouseMove(event) {
        if (pressed) {
            movedX = event.pageX * ratio;
            movedY = event.pageY * ratio;
            if (cv.offsetParent.tagName.toUpperCase() === "BODY") {
                movedX -= cv.offsetLeft * ratio;
                movedY -= cv.offsetTop * ratio;
            } else {
                movedX -= cv.offsetParent.offsetLeft * ratio;
                movedY -= cv.offsetParent.offsetTop * ratio;
            }
            redraw();
        }
    }

    function onMouseUp(event) {
        if (auto) {
            movedX = centerX;
            movedY = centerY;
            redraw();
        }
        pressed = 0;
        document.body.style.userSelect = 'unset';
        document.body.style.cursor = 'default';
    }

    function redraw() {
        movedX = constrain(movedX, r, size - r);
        movedY = constrain(movedY, r, size - r);
        cx.clearRect(0, 0, size, size);

        cx.beginPath();
        cx.arc(centerX, centerY, R, 0, 2 * Math.PI, false);
        let grd = cx.createRadialGradient(centerX, centerY, R * 2 / 3, centerX, centerY, R);
        grd.addColorStop(0, '#00000005');
        grd.addColorStop(1, '#00000030');
        cx.fillStyle = grd;
        cx.fill();

        /*cx.beginPath();
        cx.arc(movedX, movedY, r, 0, 2 * Math.PI, false);
        grd = cx.createLinearGradient(movedX, movedY - r, movedX, movedY + r);
        grd.addColorStop(1, adjust(color, -0.4));
        grd.addColorStop(0, color);
        cx.fillStyle = grd;
        cx.fill();
    
        cx.beginPath();
        let r1 = r * 0.8;
        cx.arc(movedX, movedY, r1, 0, 2 * Math.PI, false);
        grd = cx.createLinearGradient(movedX, movedY - r1, movedX, movedY + r1);
        grd.addColorStop(0, adjust(color, -0.4));
        grd.addColorStop(1, color);
        cx.fillStyle = grd;
        cx.fill();*/

        cx.beginPath();
        cx.arc(movedX, movedY, r, 0, 2 * Math.PI, false);
        grd = cx.createRadialGradient(movedX, movedY, 0, movedX, movedY, r);
        grd.addColorStop(0, adjust(color, 0.4));
        grd.addColorStop(1, color);
        cx.fillStyle = grd;
        cx.fill();

        if (!pressed) return;
        let x = Math.round((movedX - centerX) / (size / 2 - r) * 255);
        let y = -Math.round((movedY - centerY) / (size / 2 - r) * 255);

        if (exp) {
            x = ((x * x + 255) >> 8) * (x > 0 ? 1 : -1);
            y = ((y * y + 255) >> 8) * (y > 0 ? 1 : -1);
        }
        callback({ x: x, y: y });
    }
    redraw();
});
