let Joystick = (function (cont, dpad, color, auto, exp, callback) {
    // based on https://github.com/bobboteck/JoyStick
    /*cont = EL('joy#' + cont);
    if (!cont) return;
    cont.style.touchAction = "none";
    let cv = document.createElement("canvas");
    let size = cont.clientWidth;*/

    //
    let cv = document.getElementById('#' + cont);
    if (!cv || !cv.parentNode.clientWidth) return;
    let size = cv.parentNode.clientWidth;
    //

    let ratio = window.devicePixelRatio;
    cv.style.width = size + 'px';
    cv.style.height = size + 'px';
    size *= ratio;
    cv.width = size;
    cv.height = size;
    cv.style.cursor = 'pointer';

    //cont.appendChild(cv);

    let cx = cv.getContext("2d");
    let r = size * 0.23;
    let R = size * 0.4;
    let centerX = size / 2;
    let centerY = size / 2;
    let movedX = centerX;
    let movedY = centerY;
    let pressed = 0;
    let dpressed = 0;

    if ("ontouchstart" in document.documentElement) {
        cv.addEventListener("touchstart", onTouchStart, false);
        if (!dpad) document.addEventListener("touchmove", onTouchMove, false);
        document.addEventListener("touchend", onTouchEnd, false);
    } else {
        cv.addEventListener("mousedown", onMouseDown, false);
        if (!dpad) document.addEventListener("mousemove", onMouseMove, false);
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
        if (dpad) onTouchMove(event);
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
        if (auto || dpad) {
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
        if (dpad) onMouseMove(event);
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
        if (auto || dpad) {
            movedX = centerX;
            movedY = centerY;
            redraw();
        }
        pressed = 0;
        document.body.style.userSelect = 'unset';
        document.body.style.cursor = 'default';
    }

    function redraw() {
        cx.clearRect(0, 0, size, size);
        movedX = constrain(movedX, r, size - r);
        movedY = constrain(movedY, r, size - r);
        let x = Math.round((movedX - centerX) / (size / 2 - r) * 255);
        let y = -Math.round((movedY - centerY) / (size / 2 - r) * 255);

        if (dpad) {
            if (Math.abs(x) < 150 && Math.abs(y) < 150) {
                x = 0, y = 0;
            } else {
                dpressed = 1;
                if (Math.abs(x) > Math.abs(y)) x = Math.sign(x), y = 0;
                else x = 0, y = Math.sign(y);
            }

            cx.beginPath();
            cx.arc(centerX, centerY, R * 1.15, 0, 2 * Math.PI, false);
            cx.lineWidth = R / 20;
            cx.strokeStyle = color;
            cx.stroke();

            cx.lineWidth = R / 10;
            let rr = R * 0.9;
            let cw = R / 4;
            let ch = rr - cw;
            let sh = [[1, 0], [-1, 0], [0, 1], [0, -1]];
            for (let i = 0; i < 4; i++) {
                cx.beginPath();
                cx.strokeStyle = (x == sh[i][0] && y == -sh[i][1]) ? adjust(color, 0.5) : color;
                cx.moveTo(centerX + ch * sh[i][0] - cw * sh[i][1], centerY + ch * sh[i][1] - cw * sh[i][0]);
                cx.lineTo(centerX + rr * sh[i][0], centerY + rr * sh[i][1]);
                cx.lineTo(centerX + ch * sh[i][0] + cw * sh[i][1], centerY + ch * sh[i][1] + cw * sh[i][0]);
                cx.stroke();
            }
            /*
            cx.beginPath();
            cx.strokeStyle = (x == 1) ? adjust(color, 0.5) : color;
            cx.moveTo(centerX + ch, centerY - cw);
            cx.lineTo(centerX + rr, centerY);
            cx.lineTo(centerX + ch, centerY + cw);
            cx.stroke();

            cx.beginPath();
            cx.strokeStyle = (x == -1) ? adjust(color, 0.5) : color;
            cx.moveTo(centerX - ch, centerY - cw);
            cx.lineTo(centerX - rr, centerY);
            cx.lineTo(centerX - ch, centerY + cw);
            cx.stroke();

            cx.beginPath();
            cx.strokeStyle = (y == 1) ? adjust(color, 0.5) : color;
            cx.moveTo(centerX - cw, centerY - ch);
            cx.lineTo(centerX, centerY - rr);
            cx.lineTo(centerX + cw, centerY - ch);
            cx.stroke();

            cx.beginPath();
            cx.strokeStyle = (y == -1) ? adjust(color, 0.5) : color;
            cx.moveTo(centerX - cw, centerY + ch);
            cx.lineTo(centerX, centerY + rr);
            cx.lineTo(centerX + cw, centerY + ch);
            cx.stroke();
            */
            if (dpressed) callback({ x: x, y: y });
            if (!x && !y) dpressed = 0;

        } else {
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
            if (exp) {
                x = ((x * x + 255) >> 8) * (x > 0 ? 1 : -1);
                y = ((y * y + 255) >> 8) * (y > 0 ? 1 : -1);
            }
            callback({ x: x, y: y });
        }
    }
    redraw();
});
