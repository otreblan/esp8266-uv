# esp8266 uv sensor

https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/linux-setup.html

## Dependencies

* `esp8266-rtos-sdk`
* `esptool`
* `xtensa-lx106-elf-gcc-bin`

## Build

``` bash
# Only the first time. Don't forget to relogin.
sudo usermod -aG dialout "$USER" || sudo usermod -aG uucp "$USER"


git clone https://github.com/otreblan/esp8266-uv

cd esp8266-uv
mkdir -p build

cd build
cmake ..

ESPPORT=/dev/ttyUSB0 make flash
```
