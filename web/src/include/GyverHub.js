function getMaskList() {
  let list = [];
  for (let i = 0; i < 33; i++) {
    let imask;
    if (i == 32) imask = 0xffffffff;
    else imask = ~(0xffffffff >>> i);
    list.push(`${(imask >>> 24) & 0xff}.${(imask >>> 16) & 0xff}.${(imask >>> 8) & 0xff}.${imask & 0xff}`);
  }
  return list;
}
String.prototype.hashCode = function () {
  if (!this.length) return 0;
  let hash = new Uint32Array(1);
  for (let i = 0; i < this.length; i++) {
    hash[0] = ((hash[0] << 5) - hash[0]) + this.charCodeAt(i);
  }
  return hash[0];
}

class GyverHub {
  cfg = {
    prefix: 'MyDevices', client_id: new Date().getTime().toString(16).slice(-8),
    use_ws: false, use_hook: true, local_ip: '192.168.1.1', netmask: 24,
    use_bt: false,
    use_serial: false, ser_baud: 115200,
    use_mqtt: false, mq_host: 'test.mosquitto.org', mq_port: '8081', mq_login: '', mq_pass: '',
  };
  bt = new Bluetooth();
  ser = new Serial();
}

class Serial {
  onmessage(data) {
  }
  onopen() {
  }
  onclose() {
  }
  onerror(e) {
  }
  onchange() {
  }
  state() {
    return this.lock;
  }

  port = null;
  lock = false;
  reader = null;

  async select() {
    await this.close();
    const ports = await navigator.serial.getPorts();
    for (let port of ports) await port.forget();
    try {
      await navigator.serial.requestPort();
      this.onchange(true);
    } catch (e) {
      this.onerror('[Serial] ' + e);
      this.onchange(false);
    }
  }

  async start(baud) {
    if (this.lock) return this.onerror("[Serial] Already open");

    try {
      this.lock = true;
      const ports = await navigator.serial.getPorts();
      if (!ports.length) return this.onerror("[Serial] No port");
      this.port = ports[0];
      await this.port.open({ baudRate: baud });
      this.onopen();

      while (this.port.readable) {
        this.reader = this.port.readable.getReader();
        try {
          while (true && this.lock) {
            const { value, done } = await this.reader.read();
            if (done) break;
            const data = new TextDecoder().decode(value);
            this.onmessage(data);
          }
        } catch (e) {
          this.onerror('[Serial] ' + e);
        } finally {
          await this.reader.releaseLock();
          await this.port.close();
          this.onclose();
          break;
        }
      }
    } catch (e) {
      this.onerror('[Serial] ' + e);
    }
    this.reader = null;
    this.lock = false;
  }

  async close() {
    if (this.reader) this.reader.cancel();
    this.lock = false;
  }

  async send(text) {
    if (!this.lock) return this.onerror("[Serial] Not open");
    try {
      const encoder = new TextEncoder();
      const writer = this.port.writable.getWriter();
      await writer.write(encoder.encode(text + '\0'));
      writer.releaseLock();
    } catch (e) {
      this.onerror('[Serial] ' + e);
    }
  }
}

class Bluetooth {
  // based on https://github.com/loginov-rocks/Web-Bluetooth-Terminal
  constructor() {
    this._maxCharacteristicValueLength = 20;
    this._device = null;
    this._characteristic = null;
    this._serviceUuid = 0xFFE0;
    this._characteristicUuid = 0xFFE1;
    this._boundHandleDisconnection = this._handleDisconnection.bind(this);
    this._boundHandleCharacteristicValueChanged = this._handleCharacteristicValueChanged.bind(this);
  }

  onmessage(data) {
  }
  onopen() {
  }
  onclose() {
  }
  onerror(e) {
  }
  state() {
    return (this._device);
  }

  open() {
    return this._connectToDevice(this._device)
      .then(() => this.onopen())
      .catch(e => this._onerror(e));
  }

  close() {
    this._disconnectFromDevice(this._device);
    if (this._characteristic) {
      this._characteristic.removeEventListener('characteristicvaluechanged', this._boundHandleCharacteristicValueChanged);
      this._characteristic = null;
    }
    if (this._device) this.onclose();
    this._device = null;
  }

  send(data) {
    if (!this._characteristic) return this._onerror('No device');

    data += '\0';
    const chunks = this.constructor._splitByLength(data, this._maxCharacteristicValueLength);
    let promise = this._writeToCharacteristic(this._characteristic, chunks[0]);

    for (let i = 1; i < chunks.length; i++) {
      promise = promise.then(() => new Promise((resolve, reject) => {
        if (!this._characteristic) {
          this._onerror('Device has been disconnected');
          reject();
        }
        this._writeToCharacteristic(this._characteristic, chunks[i]).
          then(resolve).
          catch(reject);
      }));
    }
    return promise;
  }

  getName() {
    return this._device ? this._device.name : '';
  }

  // private
  _onerror(e) {
    this.onerror('[BT] ' + e);
  }
  
  _connectToDevice(device) {
    return (device ? Promise.resolve(device) : this._requestBluetoothDevice()).
      then((device) => this._connectDeviceAndCacheCharacteristic(device)).
      then((characteristic) => this._startNotifications(characteristic)).
      catch((error) => {
        this._onerror(error);
        return Promise.reject(error);
      });
  }

  _disconnectFromDevice(device) {
    if (!device) return;
    device.removeEventListener('gattserverdisconnected', this._boundHandleDisconnection);
    if (!device.gatt.connected) return;
    device.gatt.disconnect();
  }

  _requestBluetoothDevice() {
    return navigator.bluetooth.requestDevice({
      filters: [{ services: [this._serviceUuid] }],
    }).
      then((device) => {
        this._device = device; // Remember device.
        this._device.addEventListener('gattserverdisconnected', this._boundHandleDisconnection);
        return this._device;
      });
  }

  _connectDeviceAndCacheCharacteristic(device) {
    if (device.gatt.connected && this._characteristic) {
      return Promise.resolve(this._characteristic);
    }
    return device.gatt.connect().
      then((server) => {
        return server.getPrimaryService(this._serviceUuid);
      }).
      then((service) => {
        return service.getCharacteristic(this._characteristicUuid);
      }).
      then((characteristic) => {
        this._characteristic = characteristic; // Remember characteristic.
        return this._characteristic;
      });
  }

  _startNotifications(characteristic) {
    return characteristic.startNotifications().
      then(() => {
        characteristic.addEventListener('characteristicvaluechanged', this._boundHandleCharacteristicValueChanged);
      });
  }

  _stopNotifications(characteristic) {
    return characteristic.stopNotifications().
      then(() => {
        characteristic.removeEventListener('characteristicvaluechanged', this._boundHandleCharacteristicValueChanged);
      });
  }

  _handleDisconnection(event) {
    const device = event.target;
    this.onclose();
    this._connectDeviceAndCacheCharacteristic(device).
      then((characteristic) => this._startNotifications(characteristic)).
      then(() => {
        this.onopen();
      }).catch((e) => this._onerror(e));
  }

  _handleCharacteristicValueChanged(event) {
    const value = new TextDecoder().decode(event.target.value);
    this.onmessage(value);
  }

  _writeToCharacteristic(characteristic, data) {
    return characteristic.writeValue(new TextEncoder().encode(data));
  }

  static _splitByLength(string, length) {
    return string.match(new RegExp('(.|[\r\n]){1,' + length + '}', 'g'));
  }
}