# Hello World Example

Starts a FreeRTOS task to print "Hello World"

See the README.md file in the upper level 'examples' directory for more information about examples.

## Dependencies

* `esp8266-rtos-sdk`
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
