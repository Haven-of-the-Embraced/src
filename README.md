# Haven of The Embraced

Haven of The Embraced (Haven) is a text-based Multi-User Dungeon (MUD) engine based on the ROM 2.4 / Merc / Diku codebase, set in a World of Darkness environment.

This repository is organized as a self-contained runtime and development workspace:
- `src/` — C source files, headers, and `Makefile`
- `area/` — Sanitized core system areas (`limbo.are` VNUMs `1–99` and `codespecii.are` VNUMs `100–299`)
- `data/` — Core engine data (`area.lst`, `command.dat`, `config.cfg`, `guild.dat`, `help.dat`, `music.txt`, `quest.dat`, `social.dat`)
- `player/`, `gods/`, `log/` — Runtime storage directories
- `docker/` — Docker entrypoint and Windows PowerShell runtime initializer (`initialize-runtime.ps1`)
- `scripts/` — Server management and optional integration scripts

See [`VNUM_MIGRATION.md`](VNUM_MIGRATION.md) for the complete reference table mapping legacy world VNUMs to the consolidated `limbo.are` (`1–99`) and `codespecii.are` (`100–299`) system VNUMs.

---

## Quick Start with Docker (Windows, macOS, and Linux)

### 1. (Windows Only) Normalize Runtime Files
If you cloned the repository on Windows with `core.autocrlf` enabled, run the PowerShell initializer once from the repository root to ensure `LF` line endings and verify runtime directories:
```powershell
powershell -ExecutionPolicy Bypass -File .\docker\initialize-runtime.ps1
```

### 2. Build and Start the Server
```sh
docker compose up --build
```
Connect your MUD client (e.g. Mudlet, TinTin++, MUSHclient) to **`127.0.0.1:4000`**.

### 3. Run the Automated Boot Test in Docker
```sh
docker compose run --rm --no-deps mud 4000 testrun
```

---

## Native Linux Build & Execution

Install GCC, `make`, `libc6-dev`, and `libcrypt-dev`, then compile and run from `src/`:

```sh
make -C src new -j2
cd src
./rom 4000
```

To execute the non-interactive boot and 3-loop verification test:
```sh
cd src
./rom 4055 testrun
```

---

## Environment Variables & Optional Integrations (`.env.template`)

Copy `.env.template` to `.env` (`cp .env.template .env`) if you use optional integrations in `scripts/`:
- **`RESEND_API_KEY` & `RESEND_FROM_EMAIL`** (used by `scripts/send_email.py`):
  1. Sign in at <https://resend.com/login> and verify your sending domain at <https://resend.com/domains>.
  2. Open <https://resend.com/api-keys>, click **Create API Key**, and copy the key (`re_...`) into `.env`.
- **`DISCORD_TOKEN`** (used by `scripts/discord_bot/bot.py`):
  1. Open the Discord Developer Portal at <https://discord.com/developers/applications>.
  2. Select your application → **Bot**, enable **Message Content Intent**, and copy the bot token into `.env`.

---

## License

Haven of The Embraced is covered under multiple licenses. ROM, Diku, and Merc licenses apply to code derived from those sources. Code written by the HavenMUD team is published under [CC BY-SA 2.0](https://creativecommons.org/licenses/by-sa/2.0/legalcode).
