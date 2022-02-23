# GameboyEMU

Prakses projekts, priekš kursa PT2 Gameboy Emulātora izstrāde.

Plāns ir izmantot `C/C++` un `meson` sistēmu, kā arī populāro bibliotēku `SDL2`

Palaišana:
```sh
meson build && cd builddir && ninja
./gameboyemu -h
```

# Izmantotā informācīja priekš Izstrādes.
-   Informācīja par [Memory Map](http://gameboy.mongenel.com/dmg/asmmemmap.html)
-   Tehniskā dokumentācija [gbdev.io](https://gbdev.io/pandocs/)
-   Gameboy Procesora [mašīn instrukcījas kodi](https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)
-   Gameboy Officiālā [dokumentācija](https://archive.org/details/GameBoyProgManVer1.1/page/n157/mode/2up)
-   Testēšanas [ROM](https://github.com/c-sp/gameboy-test-roms) faili.
# Izmantotā programatūra.
-   Bibleotēka [SDL2](https://www.libsdl.org/download-2.0.php).
-   [Meson](https://mesonbuild.com/) veidošanas sistēma.
