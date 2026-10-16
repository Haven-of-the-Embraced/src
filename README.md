# Haven of The Embraced

Haven is a modified ROM 2.46b MUD. Public source lives under `src/`;
local world files, accounts, and runtime state live beside it and must stay private.

## Local Docker Development

Install Docker with Docker Compose v2 (Docker Desktop in Linux-container mode on
Windows). Run commands from the repository root.

```sh
docker compose build
```

The image compiles with `src/Makefile`. Only C sources, headers, the Makefile,
and the container entrypoint are allowed into the Docker build context. World
files and player accounts are not baked into the image.

### Runtime Files

A clone can build immediately, but **cannot boot without compatible runtime
data**. Docker does not replace that data. Obtain an authorized development
bundle and arrange it as follows:

```text
Haven/
  src/
  area/       World files referenced by data/area.lst
  data/       Runtime configuration, help, socials, command tables, etc.
  player/     Local player files (may be empty for new characters)
```

Use a complete, compatible Haven bundle, not just arbitrary stock ROM areas.
The server reads `data/area.lst`, not `area/area.lst`, and requires
`data/help.dat`. Other files in `data/` are used by the clan, social, command,
quest, and configuration loaders. The entrypoint checks basic prerequisites;
the MUD remains responsible for validating the bundle's contents.

There are two zip files included, [ExtractForTesting.zip](ExtractForTesting.zip) 
and the [ExtractIfNoAreas.zip](ExtractIfNoAreas.zip). For those with the Haven 
area files, the former can be extracted to provide an area for the test harness. 
For contibutors that need a running a MUD, unzipping the latter will provide
just enough to run the MUD.

If you already have the local ROM area bundle, initialize development runtime
files from PowerShell:

```powershell
./docker/initialize-runtime.ps1
```

The initializer extracts help records and socials from the existing area files,
copies music, and creates a world-only `data/area.lst` with its required `$`
terminator. It also creates `guild.dat` with no guilds, `quest.dat` with no quest
items, normal configuration defaults, and a development command table from the
compiled handler registry. Common player commands are enabled; all other handlers
require level 110. These are conservative development permissions, not a recovery
of the original server's command policy. Review them before using real accounts.

Existing files are preserved except for the recognized malformed placeholders
(`help.dat` containing only `help.are`, and the four-entry area list beginning
with missing `olmsg.are`); those are backed up with `.pre-init` suffixes before
replacement. Original files in `area/` are not modified. Notes, bans, and last-login
history are optional and can be created by the server when needed. Keep generated
data and backups private under the existing ignore rules.

**Known local bundle limitation:** the generated guild, social, and help data
load, but the boot test rejects `area/immort.are` at line 20 with
`Fread_number: bad format`. The stock ROM mobile layout differs from Haven's
modified loader. Runtime initialization does not convert world records; a
compatible Haven world bundle or a separately tested world-format migration is
still required for a complete boot. The empty guild registry also triggers the
loader's harmless `Fread_clans: no match` diagnostic.

A fresh public clone still needs authorized area assets. Do not fabricate empty
files just to bypass startup checks. No redistributable starter world is included.

The local `area/fane.are`, is a minimal Haven-native
world with seven rooms at VNUMs 666666-666672. Use `open doors` at the entrance.
This was created to test the test harness with a simple addition of a new exit
type: `climb`. At the altar, `climb chain` reaches the Chain Loft; use the same
command there to descend. Plain `up` and `down` cannot traverse these climb-only
exits.
To test it alone from PowerShell without changing the active area list:

```powershell
docker compose run --build --rm --no-deps --volume "${PWD}/data/fane-area.lst:/opt/haven/data/area.lst:ro" mud 4000 testrun
```

To use it as the active world, set `data/area.lst` to `fane.are` followed by `$`
on a separate line. Do not also load stock areas with overlapping vnums.

VNUM-bearing fields, loader variables, and script arguments use 32-bit `int`
storage instead of 16-bit `sh_int`; unrelated stats retain their existing types.
The hash tables have 4096 buckets. Integer text file formats remain unchanged.
The built-in login, recall, altar, and other fallback room constants now point
to the six Fane rooms, with some roles sharing a room. This replaces the old
refuge defaults; existing player files are not rewritten.

Climb-only exits use the `climb` OLC exit flag. In area files, add 256 to the
exit's existing door-format value (256 for an open climb route); the keyword
line names the climb target. OLC preserves this flag when saving the area.

Run the isolated high-VNUM regression test from PowerShell:

```powershell
docker build --target build -t haven-vnum-build .
docker run --rm --volume "${PWD}/area:/input-area:ro" --volume "${PWD}/data:/input-data:ro" --volume "${PWD}/tests:/tests:ro" haven-vnum-build sh /tests/test_vnums.sh
```

The test checks VNUM storage, lookups, resets, key references, closed doors,
movement, climb-only routes, random destinations, and an OLC save/reload cycle using disposable
copies. It does not modify local runtime data.

### Example Test Harness
#### Using new Vicissitude feature

`fleshcraft` creates one controlled companion at a time. Noting there was actually
a but with Fleshcraft Hellhound in which there were no dot requirement guardrails
so technically any vampire could create a Hellhound, albeit one with zero HP.

Each form unlocks at the listed Vicissitude dot, and remains available at higher
dots:

| Form | Dots | Blood | HP Cap | Hitroll Bonus | Damroll Bonus | Armor Change | Role |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Hellhound | 2 | 50 | 2000 x dots | +100 | +75 | -100 | Fast harasser |
| Szlachta | 3 | 60 | 3000 x dots | +100 | +100 | -500 | Balanced fighter |
| Mesnik | 4 | 70 | 3750 x dots | +75 | +200 | -650 | Burst damage bruiser |
| Vozhd | 5 | 80 | 5000 x dots | +400 | +400 | -1500 | Siege beast |

HP becomes `min(original max HP + growth, HP cap)`, with growth of 1000 for
Hellhound and 5000 for the other forms. Hitroll, damroll, and all four armor
values are adjusted relative to the source MOB, not replaced with absolute
values. Creation restores the companion to its new maximum HP. The existing
ten-point blood reserve is retained: creation needs at least `cost + 10` blood.
Insufficient dots or blood, unsuitable/protected targets, and an existing pet
must not spend blood or apply stat bonuses.

The Chain Loft (666672) contains a stationary canine (MOB 666667, keyword
`loftcanine`) and a human volunteer (MOB 666668, keyword `loftvolunteer`).
Hellhound accepts dogs/wolves; the other three forms accept humans/ghouls.
For example, use `fleshcraft mesnik loftvolunteer` with four dots and at least
80 blood. Live Testvamp is not automatically granted dots or extra blood.

Run the automated suite from the repository root in PowerShell:

```powershell
docker build --target build -t haven-vnum-build .
docker run --rm --volume "${PWD}/area:/input-area:ro" --volume "${PWD}/data:/input-data:ro" --volume "${PWD}/player:/input-player:ro" --volume "${PWD}/tests:/tests:ro" haven-vnum-build sh /tests/test_vnums.sh test_vicissitude
```

Required inputs are the current Fane area, compatible runtime data, and the
level-100 vampire file `player/Testvamp`. `tests/test_vicissitude.c` boots the
actual game database, verifies the two Loft reset fixtures, and loads Testvamp
through `load_char_obj()`. The harness sets
`pcdata->discipline[VICISSITUDE]` to dots 1 through 5 and supplies blood in memory,
then invokes the real `fleshcraft` command through `interpret()`.

Each run checks 76 cases, including HP growth and caps, exact bonuses and costs,
both accepted races, dot progression, blood boundaries, safe rooms, target
level limits, vampire/blood-curse requirements, one-pet limits, and controlled
target rejection. The runner executes twice, saving the area between runs so
the second process checks serialized MOB/reset data as well. All writes go to
disposable copies under `/tmp/haven`; the live player file and world are not
modified. No live-server connection or restart is needed to run the suite.

To write another ability test:

1. Add focused cases to the existing suite when they belong to Vicissitude;
  use a separate C suite and add its name to the runner's whitelist otherwise.
2. Arrange the required player-sheet values, resources, room, and fresh target.
  `new_target()` supplies known baseline stats for exact bonus assertions.
3. Invoke player commands using writable `char command[]` buffers, since
  `interpret()` modifies input. Test with a player, not an NPC that bypasses
  player restrictions.
4. Assert success state and rejected state, including unchanged resources and
  target stats on failure. Check both the exact unlock dot and one below it.
5. Extract temporary targets between cases, reset cooldowns/resources, and
  drive relevant update functions explicitly when testing timed effects.
  Do not depend on the live game loop or sleeps.
6. Rebuild the image after engine changes, then run the suite. Extend the
  independent expected-value table when changing the balance contract.

Use disposable development copies. Bind mounts are writable: saving players,
configuration, notes, or OLC changes modifies your local files. The container
creates `player/bk/`, `log/`, `gods/`, `memdump/`, and `import/` as needed.
On Linux, files created by the container may be owned by root.

### Protect Local Data

The root ignore rules exclude local runtime data, build products, and local
environment overrides:

```gitignore
/area/
/data/
/player/
/gods/
/log/
/memdump/
/import/
/src/obj/
/src/core*
*.o
.env
```

Verify with `git status --short --untracked-files=all` that no private data is
being staged. Ignore rules do not protect files already tracked by Git or erase
previously committed data. Keep source attribution and license notices intact.

### Start And Connect

```sh
docker compose up --build
```

Connect your MUD client to **127.0.0.1:4000**. Only the game port is published,
and only on localhost; the internal bot port is not published. Existing local
characters can log in using their existing credentials. Otherwise use the
MUD's character creation flow.

Edit source, then rerun `docker compose up --build` to rebuild and recreate the
container. Source is compiled into the image, not mounted over its executable.
For background operation and logs:

```sh
docker compose up --build -d
docker compose logs -f mud
```

Save characters in-game before stopping. For a clean server shutdown, use the
in-game administrator shutdown command, then:

```sh
docker compose down
```

Stopping Docker is not a substitute for saving in-game. Bind-mounted files
remain on disk after the container is removed.

### Local Overrides

Set these optional variables in an ignored `.env` file or in your shell:

```dotenv
HAVEN_PORT=4000
HAVEN_AREA_DIR=./area
HAVEN_DATA_DIR=./data
HAVEN_PLAYER_DIR=./player
```

Use `HAVEN_PORT` to change the host port if 4000 is occupied. Absolute paths
are supported for data directories; on Windows use forward slashes, for example
`C:/Users/me/haven-dev/area`. Required bind directories must already exist.

With a valid development bundle, run the server's short boot test using:

```sh
docker compose run --rm --no-deps mud 4000 testrun
```

This boots the real world and runs a few game loops before exiting. It can
write runtime state, so use development copies here too.

## Native Build

On Linux, install GCC, make, libc development headers, and libcrypt development
headers, then run:

```sh
make -C src
cd src
./rom 4000
```

Run from `src/` so the existing relative data paths resolve correctly. Without
an explicit port, this code defaults to 2000. See [src/README.md](src/README.md)
for project background, contribution guidelines, and licensing.