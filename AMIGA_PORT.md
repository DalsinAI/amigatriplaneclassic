# Triplane Classic for AmigaOS 3.x

- Upstream: `https://github.com/vranki/triplane`
- Based on: `9944e62bdab9ac2a4d7b8d79f809010a4f807674` ("Merge pull request
  #15 from OTapio/smartpointterit", 10 Mar 2024), master's head.
- Licence: GPL 3 (`COPYING`)

Status: builds, and was flown in the lab (AmigaChrome).

## Target

AmigaOS 3.x / AmiCradle, Motorola 68040 + FPU, on acsdl12 (our SDL 1.2 on
ACGame and AHI), with its 8-channel MOD music (`Mix_LoadMUS_RW`).

## Build

    make -f amiga/Makefile.amigaos -j2 \
        STOVE=/path/to/os32-gcc16/prefix \
        NDK=/path/to/gfopensdk/OpenNDK/Include_H \
        ACSDL=/path/to/amicradle-guest/gameports/acsdl12 \
        ACSDLLIB=/path/to/acsdl12/build \
        ACGAME=/path/to/acgame ACGAMELIB=/path/to/acgame/build-amiga

STOVE is GFOpenAmigaGCC 16.2.0-open4 or later; acsdl12 is built first with
its own `Makefile.amigaos`, without its own copy of ACGame (`ACG=`): the
game links acgame.library's stubs (`libacgame.a`).
`package/build-amiga.sh` (STOVE and OUT set) makes the release archive,
the drawer below with `Libs/acgame.library` and `Install-Triplane` beside
it, fetching and building acgame, acsdl12 and zlib at pinned commits.
Flags: `-noixemul -m68040 -m68881 -O2 -fomit-frame-pointer -std=gnu++23`,
`-DTRIPLANE_DATA="PROGDIR:"`.

The game's data, `fokker.dks`, is built on the host: the Makefile compiles
`src/tools/dksbuild/dksbuild.cc` with `HOSTCXX` (default `g++`) into
`build/amigaos/host/dksbuild` and runs it on `data/fokker.lst`. By hand:

    g++ -O2 -w -o dksbuild src/tools/dksbuild/dksbuild.cc
    ./dksbuild data/fokker.lst        # writes fokker.dks here

Output: `build/amigaos/Triplane/` with `triplane` and `fokker.dks`. The
release drawer adds `ReadMe` (`amiga/ReadMe`), `COPYING`, the icons
`triplane.info`, `ReadMe.info` and `COPYING.info` from `amiga/icons/`, and
`amiga/icons/drawer.info` as `build/amigaos/Triplane.info` beside the
drawer. The Makefile has no dist target; copy them in after `make`. The
icons are GlowIcons made from the game's own application icon
(`data/application_icon/triplane.xpm`, as `amiga/icons/art/triplane.png`)
with AmiCradle's `port_icons.py`:

    python3 AMICRADLE/scripts/port_icons.py amiga/icons/icons.json --out amiga/icons

## Installing

Unpack the archive anywhere (on AmiCradle: `SYS:Utilities/OpenCompress x
triplane-...lha RAM:`), then double-click `Install-Triplane` beside the
`Triplane` drawer: it asks where to put the game (`Work:Games` by
default) and makes or updates the drawer; the settings and pilot roster
are not in the archive, so an update leaves them as they are. The
drawer's `Libs` holds the acgame.library the game was built against, and
the game opens that copy first.

## Playing

Run `triplane` from its drawer (Workbench or Shell). The data is read from
`PROGDIR:`; the settings and the pilot roster are written there too, so the
drawer must be on a writable disk, or in `TRIPLANE_HOME` if that is set.

There are no pilots at first: make one in Roster, then give it to a player
in Assign Pilots. Without that, the game shows a warning instead of
flying. Solo keys are upstream's: `x` up, `w` down, `s` roll, Tab power,
`2` guns, `1` bombs; set them in Plane Mechanics (Set Sologame Keys).

## Changes to upstream

- `src/io/dksfile.cpp`, `src/tools/dksbuild/dksbuild.cc`: the DKS
  directory's offset and size are `uint32_t`, little-endian, as the DOS
  game had them. Upstream used `unsigned long` (8 bytes on 64-bit hosts,
  so a host-built file did not match a 32-bit reader); dksbuild now writes
  them little-endian on any host, and the game swaps them with
  `SDL_SwapLE32` after reading the directory on the 68k.
- `src/io/dksfile.cpp` (`__amigaos__`): the data file is
  `TRIPLANE_DATA` + name, i.e. `PROGDIR:fokker.dks` (no `/` after a
  device's colon).
- `src/settings.cpp` (`__amigaos__`): settings and the roster in
  `TRIPLANE_HOME` (a `/` added unless it ends in `:` or `/`), else
  `PROGDIR:`.
- `src/triplane.cpp`: `-playback` stops with a message when `record.dta`
  or `record.000` is missing, instead of `fread` on a NULL file.
- `src/triplane.cpp` (c86559c): planes 4-15 took their pictures with
  `std::move` from planes 0-3, which left 0-3 empty (and 8-15 were moved
  from those empty ones). The players' planes were drawn from a null
  pointer: a crash on most systems, a block of rubbish over the airfield
  on the Amiga. Each now gets a copy of its colour's picture.

New: `amiga/` (`Makefile.amigaos`; `pthread_stubs.c`, the few POSIX thread
calls libstdc++ asks for, one thread; `stack.c`, a 256 KB stack; `ReadMe`;
`icons/`) and this file.

## Known limits

- Flicker with `ACGame/OpenGPU 1`: acsdl12 flushes the palette during
  `SDL_Delay`, and the cleared frame shows between pictures. Set
  `SetEnv SAVE ACGame/OpenGPU 0`.
- The mouse needs acsdl12 with the change in
  DalsinAI/amicradle-guest#624 (branch `team2/acsdl12-mouse`); build
  against that acsdl12 until it is merged.
