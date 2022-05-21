# GameboyEMU

Prakses projekts, priekš kursa PT2 Gameboy Emulātora izstrāde.

Plāns ir izmantot `C/C++` un `meson` sistēmu, kā arī populāro bibliotēku `SDL2`

Palaišana `Linux/Mac OS`:
```sh
meson build && cd build && ninja
./gbemu -h
```
Palaišana `Windows`:
```cmd
mkdir subprojects
meson wrap install sdl2
meson build
cd build
ninja
./gbemu.exe -h
```
Priekš `Windows` sistēmām izmantots [`Visual Studio Community 2019`](https://visualstudio.microsoft.com/downloads/) `x64 Native Tools Comand Prompt for VS 2019`.

# TODO Plāns 
-   [x] Izveidot primitivue sistēmu kas var ielādēt ROM failus. 
-   [x] Izveidot pamata Systemu.
-   [x] meson.build vajag atsevišķu dep tree priekš Windows.
-   [ ] Veidot pierakstus un dokumentāciju.
-   [x] Izveidot CPU Emulāciju. 
-   [ ] Izveidot emulātorām `upscale` funkcionalitāti.
-   [ ] Izveidot konfigurācījas sistēmu.
-   [ ] Vajag refakturet kodu

# Izmantotā informācīja priekš Izstrādes.
-   Informācīja par [Memory Map](http://gameboy.mongenel.com/dmg/asmmemmap.html)
-   Tehniskā dokumentācija [gbdev.io](https://gbdev.io/pandocs/)
-   Gameboy Procesora [mašīn instrukcījas kodi](https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)
-   Gameboy Officiālā [dokumentācija](https://archive.org/details/GameBoyProgManVer1.1/page/n157/mode/2up)
-   Testēšanas [ROM](https://github.com/c-sp/gameboy-test-roms) faili.
# Izmantotā programatūra.
-   Bibleotēka [SDL2](https://www.libsdl.org/download-2.0.php).
-   [Meson](https://mesonbuild.com/) veidošanas sistēma.
-   [VS2019](https://visualstudio.microsoft.com/) MSVC Kompilātors.

# Dependency list
-   SDL2
-   SDL2_TTF
-   freetype2
-   zlib