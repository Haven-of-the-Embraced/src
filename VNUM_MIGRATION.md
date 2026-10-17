# VNUM Consolidation & Migration Reference

All code-dependent rooms, mobiles, and objects previously scattered across 16 legacy `.are` files have been consolidated into two sanitized core system area files:
- **`area/limbo.are` (`1–99`)**: Core engine primitives (Void/Limbo rooms, currency, corpses, body parts, discipline/spell summons and tokens).
- **`area/codespecii.are` (`100–299`)**: Core sanctuary hub rooms (`100–113`), system NPCs & Garou spiritquest mobs (`100–131`), crafting/school/system objects (`100–142`), quest rewards (`150–170`), brewing components (`180–192`), harvestable trees & logs (`200–209`), and mineable veins, ores, & pickaxe (`210–230`).

---

## 1. Rooms Migration (`ROOM_VNUM_*` & System Rooms)

| Old VNUM | Original `.are` File | Macro / System Purpose | New VNUM | Target `.are` File |
|---:|---|---|---:|---|
| `1` | `limbo.are` | The Void (`fight.c` corpse handling) | `1` | `limbo.are` |
| `2` | `limbo.are` | `ROOM_VNUM_LIMBO` | `2` | `limbo.are` |
| `3700` | `barracks.are` | `ROOM_VNUM_SCHOOL` (Newbie Training Entrance) | `100` | `codespecii.are` |
| `701` | `chester.are` | `ROOM_VNUM_TEMPLE` (Central Temple of Recall) | `101` | `codespecii.are` |
| `703` | `chester.are` | `ROOM_VNUM_ALTAR` (Altar of Sacrifice) | `102` | `codespecii.are` |
| `705` | `chester.are` | `ROOM_VNUM_DONATE` (Donation Chamber) | `103` | `codespecii.are` |
| `1200` | `mezquita.are` | `ROOM_VNUM_CHAT` (Hall of Council & Discourse) | `104` | `codespecii.are` |
| `30790` | `umbra1.are` | `ROOM_VNUM_PARADOX` (The Paradox Realm) | `105` | `codespecii.are` |
| `4201` | `stygia.are` | `ROOM_VNUM_DEMISE` (The River Styx) | `106` | `codespecii.are` |
| `4300` | `stygia.are` | `ROOM_VNUM_HONOR` (The Fringes of the Tempest) | `107` | `codespecii.are` |
| `4400` | `stygia.are` | `ROOM_VNUM_CIRCLE` (The Sea of Souls) | `108` | `codespecii.are` |
| `4500` | `sacheart.are` | `ROOM_VNUM_BALANCE` (The Balance Sanitarium) | `109` | `codespecii.are` |
| — | `codespecii.are` | The Crafting Workshop and Forge | `110` | `codespecii.are` |
| `3705` | `barracks.are` | The Grand Training Barracks (`class_table` guild) | `111` | `codespecii.are` |
| — | `codespecii.are` | The Healer's Sanctum | `112` | `codespecii.are` |
| `26000` | `uquest.are` | The Questmaster's Office | `113` | `codespecii.are` |
| `3001` | `midgaard.are` | `db.c` / `save.c` fallback safe room | `101` | `codespecii.are` |
| `3003, 9619` | `midgaard.are`, `suceava.are` | `class_table` guild rooms (`const.c`) | `101, 111` | `codespecii.are` |
| `9621, 9706` | `suceava.are` | `act_obj.c` special exit transfer rooms | `100, 101` | `codespecii.are` |
| `701, 1954, 10995, 13959` | `chester.are`, `sanpolo.are`, `geneva.are`, `figueres.are` | `hometown_table` recall rooms (`tables.c`) | `101` | `codespecii.are` |
| `7000–7084, 15800–15810` | `clanredo.are`, `shivan.are` | `data/guild.dat` clan halls | `101` | `codespecii.are` |

---

## 2. Mobiles Migration (`MOB_VNUM_*`, `GROUP_VNUM_*` & System NPCs)

| Old VNUM | Original `.are` File | Macro / System Purpose | New VNUM | Target `.are` File |
|---:|---|---|---:|---|
| `1` | `limbo.are` | `MOB_VNUM_RAT` | `1` | `limbo.are` |
| `4` | `limbo.are` | `MOB_VNUM_VICISSITUDE_SZLACHTA` | `4` | `limbo.are` |
| `5` | `limbo.are` | `MOB_VNUM_VICISSITUDE_VOZHD` | `5` | `limbo.are` |
| `6` | `limbo.are` | `MOB_VNUM_VICISSITUDE_ZULO` | `6` | `limbo.are` |
| `22` | `limbo.are` | `MOB_VNUM_ALLIES` | `22` | `limbo.are` |
| `23` | `limbo.are` | `MOB_VNUM_DOPPLEGANGER` | `23` | `limbo.are` |
| `27` | `limbo.are` | `MOB_VNUM_HORDE_RATS` | `27` | `limbo.are` |
| `28` | `limbo.are` | `MOB_VNUM_MORTIS_SPIRIT` | `28` | `limbo.are` |
| `29` | `limbo.are` | `MOB_VNUM_MORTIS_ZOMBIE` | `29` | `limbo.are` |
| `30` | `limbo.are` | `MOB_VNUM_OBTEN_SERVANT` | `30` | `limbo.are` |
| `32` | `limbo.are` | `MOB_VNUM_HOMUNCULUS` | `32` | `limbo.are` |
| `33` | `limbo.are` | `MOB_VNUM_ATHANATOS` | `33` | `limbo.are` |
| `708` | `chester.are` | `MOB_VNUM_CITYGUARD` | `100` | `codespecii.are` |
| `711` | `chester.are` | `MOB_VNUM_FIDO` | `101` | `codespecii.are` |
| `2106` | `wyrma.are` | `MOB_VNUM_PATROLMAN` | `102` | `codespecii.are` |
| `3721` | `barracks.are` | Master-at-Arms (`ACT_TRAIN` / `ACT_PRACTICE`) | `103` | `codespecii.are` |
| — | `codespecii.are` | Sanctuary Healer (`ACT_IS_HEALER`) | `104` | `codespecii.are` |
| `26003` | `uquest.are` | Grand Questmaster (`spec_questmaster`) | `105` | `codespecii.are` |
| — | `codespecii.are` | Town Banker (`ACT_IS_CHANGER`) | `106` | `codespecii.are` |
| — | `codespecii.are` | Sanctuary Provisioner (Shopkeeper) | `107` | `codespecii.are` |
| `2100` | `wyrma.are` | `GROUP_VNUM_TROLLS` | `108` | `codespecii.are` |
| `2101` | `wyrma.are` | `GROUP_VNUM_OGRES` | `109` | `codespecii.are` |
| `30720–30731` | `umbra1.are` | `MOB_VNUM_SPIRITQUEST` (12 Garou Spiritquest Mobs) | `120–131` | `codespecii.are` |

---

## 3. Objects Migration (`OBJ_VNUM_*`, School Gear, Crafting, Quests & Brewing)

| Old VNUM | Original `.are` File | Macro / System Purpose | New VNUM | Target `.are` File |
|---:|---|---|---:|---|
| `1–5` | `limbo.are` | `OBJ_VNUM_SILVER_ONE` .. `OBJ_VNUM_COINS` | `1–5` | `limbo.are` |
| `6` | `limbo.are` | `OBJ_VNUM_BREW_POTION` | `6` | `limbo.are` |
| `8` | `limbo.are` | `OBJ_VNUM_GOLDEN_BERRY` | `8` | `limbo.are` |
| `9` | `limbo.are` | `OBJ_VNUM_SKIN` | `9` | `limbo.are` |
| `10–17` | `limbo.are` | `OBJ_VNUM_CORPSE_NPC` .. `OBJ_VNUM_BRAINS` | `10–17` | `limbo.are` |
| `18` | `limbo.are` | `OBJ_VNUM_PLASM` | `18` | `limbo.are` |
| `19–26` | `limbo.are` | `OBJ_VNUM_TROPHY` .. `OBJ_VNUM_ROSE` | `19–26` | `limbo.are` |
| `29` | `limbo.are` | `OBJ_VNUM_EMPTY_VIAL` | `29` | `limbo.are` |
| `30` | `limbo.are` | `OBJ_VNUM_DUMMY` | `30` | `limbo.are` |
| `32` | `limbo.are` | `OBJ_VNUM_BROKEN_MIRROR` | `32` | `limbo.are` |
| `40` | `limbo.are` | `QUEST_OBJ` | `40` | `limbo.are` |
| `50` | `limbo.are` | `OBJ_VNUM_RIFT` | `50` | `limbo.are` |
| `100–101` | `codespecii.are` | Old oak workbench & sharp carving knife | `100–101` | `codespecii.are` |
| `28850–28854` (`102–106`) | `copenh.are` / `codespecii.are` | `OBJ_VNUM_PLANK` (yew, maple, ash, walnut, oak planks) | `102–106` | `codespecii.are` |
| `28855–28859` (`107–111`) | `copenh.are` / `codespecii.are` | `OBJ_VNUM_BLOCK` (yew, maple, ash, walnut, oak blocks) | `107–111` | `codespecii.are` |
| `28870–28872` (`112–114`) | `copenh.are` / `codespecii.are` | `crafted_item_table` (wooden bowl, walking stick, wooden cross) | `112–114` | `codespecii.are` |
| `115–117` | `codespecii.are` | Carving knife, carving plane, carving bench | `115–117` | `codespecii.are` |
| `705` | `chester.are` | `OBJ_VNUM_PIT` (Donation Pit) | `120` | `codespecii.are` |
| `715` | `chester.are` | `OBJ_VNUM_MAP` | `121` | `codespecii.are` |
| `3733` | `barracks.are` | `OBJ_VNUM_GUIDE` | `122` | `codespecii.are` |
| `3735` | `barracks.are` | `OBJ_VNUM_BANDAGE` | `123` | `codespecii.are` |
| `3310` | `midgaard.are` | `OBJ_VNUM_TOME` | `124` | `codespecii.are` |
| `30500` | `umbra1.are` | `OBJ_VNUM_TASS_MUSHROOM` | `125` | `codespecii.are` |
| `31833` | `holiday.are` | Carnival Doubloon (`fight.c`) | `126` | `codespecii.are` |
| `31832` | `holiday.are` | Carnival Bead (`fight.c`) | `127` | `codespecii.are` |
| `802` | `chester.are` | `OBJ_VNUM_SCHOOL_MACE` | `130` | `codespecii.are` |
| `827` | `chester.are` | `OBJ_VNUM_SCHOOL_DAGGER` | `131` | `codespecii.are` |
| `826` | `chester.are` | `OBJ_VNUM_SCHOOL_SWORD` | `132` | `codespecii.are` |
| `809` | `chester.are` | `OBJ_VNUM_SCHOOL_SPEAR` | `133` | `codespecii.are` |
| `808` | `chester.are` | `OBJ_VNUM_SCHOOL_STAFF` | `134` | `codespecii.are` |
| `807` | `chester.are` | `OBJ_VNUM_SCHOOL_AXE` | `135` | `codespecii.are` |
| `806` | `chester.are` | `OBJ_VNUM_SCHOOL_FLAIL` | `136` | `codespecii.are` |
| `805` | `chester.are` | `OBJ_VNUM_SCHOOL_WHIP` | `137` | `codespecii.are` |
| `804` | `chester.are` | `OBJ_VNUM_SCHOOL_POLEARM` | `138` | `codespecii.are` |
| `836` | `chester.are` | `OBJ_VNUM_SCHOOL_LANCE` | `139` | `codespecii.are` |
| `825` | `chester.are` | `OBJ_VNUM_SCHOOL_VEST` | `140` | `codespecii.are` |
| `824` | `chester.are` | `OBJ_VNUM_SCHOOL_SHIELD` | `141` | `codespecii.are` |
| `812` | `chester.are` | `OBJ_VNUM_SCHOOL_BANNER` | `142` | `codespecii.are` |
| `6601–6618` | `twocity7.are` | `reward_table` quest equipment (`quest.c`) | `150–167` | `codespecii.are` |
| `30432` | `justin3.are` | `reward_table` Potion of Escape to a True Friend | `168` | `codespecii.are` |
| `1103` | `immtemp.are` | `reward_table` XP Boost Potion | `169` | `codespecii.are` |
| `3099` | `midgaard.are` | `reward_table` Godly Potion of Mana | `170` | `codespecii.are` |
| `771` | `chester.are` | `brew_table` Flask of Spring Water | `180` | `codespecii.are` |
| `302` | `plains.are` | `brew_table` Dusk Poison Ivy | `181` | `codespecii.are` |
| `9507` | `suceava.are` | `brew_table` Antidote Tonic | `182` | `codespecii.are` |
| `9658` | `suceava.are` | `brew_table` Glass of Brandy | `183` | `codespecii.are` |
| `9659` | `suceava.are` | `brew_table` Mug of Beer | `184` | `codespecii.are` |
| `11105` | `haven2.are` | `brew_table` Large Eagle Feather | `185` | `codespecii.are` |
| `17701` | `sengir.are` | `brew_table` Black Lotus | `186` | `codespecii.are` |
| `17705` | `sengir.are` | `brew_table` Black Bottle Liquid | `187` | `codespecii.are` |
| `27225` | `fcanyn.are` | `brew_table` Herb White Mint | `188` | `codespecii.are` |
| `30814` | `rooms.are` | `brew_table` A Peach | `189` | `codespecii.are` |
| `30834` | `rooms.are` | `brew_table` A Cup of Coffee | `190` | `codespecii.are` |
| `30836` | `rooms.are` | `brew_table` A Glass of Milk | `191` | `codespecii.are` |
| `30856` | `rooms.are` | `brew_table` Strawberries | `192` | `codespecii.are` |
| `26500–26504` | `arena.are` | `OBJ_VNUM_TREE` (`+0..4`: yew, maple, ash, walnut, oak trees) | `200–204` | `codespecii.are` |
| `26505–26509` | `arena.are` | `OBJ_VNUM_TREE` (`+5..9`: yew, maple, ash, walnut, oak logs) | `205–209` | `codespecii.are` |
| `26510–26518` | `arena.are` | `OBJ_VNUM_VEIN` (`+0..8`: iron..zinc mineral veins) | `210–218` | `codespecii.are` |
| `26520–26528` | `arena.are` | `OBJ_VNUM_VEIN` (`+10..18`: iron..zinc ore chunks) | `220–228` | `codespecii.are` |
| `26530` | `arena.are` | Mining Pickaxe | `230` | `codespecii.are` |
