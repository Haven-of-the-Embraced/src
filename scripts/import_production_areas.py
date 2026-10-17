#!/usr/bin/env python3
"""
Import external production area files into the Haven MUD repository, or reset
the repository back to its standalone core defaults (limbo.are + codespecii.are).

Usage:
    python3 scripts/import_production_areas.py /path/to/production/area [--area-lst /path/to/area.lst]
    python3 scripts/import_production_areas.py --reset
"""

import argparse
import os
import re
import shutil
import sys

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
AREA_DIR = os.path.join(REPO_ROOT, "area")
DATA_DIR = os.path.join(REPO_ROOT, "data")
AREA_LST_PATH = os.path.join(DATA_DIR, "area.lst")
CONFIG_CFG_PATH = os.path.join(DATA_DIR, "config.cfg")
LIMBO_BACKUP_PATH = os.path.join(DATA_DIR, "limbo.are.standalone.bak")

CORE_AREAS = {"limbo.are", "codespecii.are"}

# Unlinked legacy files not in production area.lst that collide with active areas
EXCLUDED_UNLINKED_AREAS = {"twocity4.are", "drachenx.are"}

PRODUCTION_VNUM_OVERRIDES = [
    ("VnumTemple", 701),
    ("VnumAltar", 703),
    ("VnumDonate", 705),
    ("VnumSchool", 3700),
    ("VnumChat", 1200),
    ("VnumParadox", 30790),
    ("VnumDemise", 4201),
    ("VnumHonor", 4300),
    ("VnumCircle", 4400),
    ("VnumBalance", 4500),
    ("VnumPrelude", 150),
    ("HometownChester", 701),
    ("HometownGeneva", 10995),
    ("HometownFigueres", 13959),
    ("HometownVenice", 1954),
    ("ObjVnumPit", 705),
    ("ObjVnumSchoolMace", 802),
    ("ObjVnumSchoolDagger", 827),
    ("ObjVnumSchoolSword", 826),
    ("ObjVnumSchoolSpear", 809),
    ("ObjVnumSchoolStaff", 808),
    ("ObjVnumSchoolAxe", 807),
    ("ObjVnumSchoolFlail", 806),
    ("ObjVnumSchoolWhip", 805),
    ("ObjVnumSchoolPolearm", 804),
    ("ObjVnumSchoolLance", 836),
    ("ObjVnumSchoolVest", 825),
    ("ObjVnumSchoolShield", 824),
    ("ObjVnumSchoolBanner", 812),
    ("ObjVnumMap", 715),
    ("ObjVnumGuide", 3733),
    ("ObjVnumTome", 3310),
    ("ObjVnumTree", 26500),
    ("ObjVnumVein", 26510),
    ("ObjVnumPlank", 28850),
    ("ObjVnumBlock", 28855),
    ("ObjVnumBandage", 3735),
    ("ObjVnumTassMushroom", 30500),
    ("MobVnumCityguard", 708),
    ("MobVnumFido", 711),
    ("MobVnumPatrolman", 2106),
    ("GroupVnumTrolls", 2100),
    ("GroupVnumOgres", 2101),
]

OVERRIDE_KEYS = {k for k, _ in PRODUCTION_VNUM_OVERRIDES}


def extract_objects_block(limbo_text, min_vnum=60):
    """Extract object definitions with VNUM >= min_vnum from #OBJECTS section."""
    m = re.search(r"^#OBJECTS\n(.*?)\n^#0\b", limbo_text, re.M | re.S)
    if not m:
        return ""
    objs_body = m.group(1)
    entries = re.split(r"(?=^#\d+\b)", objs_body, flags=re.M)
    kept = []
    for entry in entries:
        em = re.match(r"^#(\d+)\b", entry)
        if em and int(em.group(1)) >= min_vnum:
            kept.append(entry.rstrip("\n"))
    return "\n".join(kept)


def merge_limbo(src_limbo_path, dst_limbo_path):
    """
    Merge production limbo.are (1-50) with the repository's default harvesting
    objects (60-90) so both standalone defaults and production cross-area
    references work seamlessly.
    """
    with open(dst_limbo_path, "r", encoding="latin-1") as f:
        dst_text = f.read().replace("\r\n", "\n")

    with open(src_limbo_path, "r", encoding="latin-1") as f:
        src_text = f.read().replace("\r\n", "\n")

    extra_objs = extract_objects_block(dst_text, min_vnum=60)
    if extra_objs:
        # Insert extra_objs right before #0 of #OBJECTS in src_text
        obj_sec = re.search(r"(^#OBJECTS\n.*?\n)(^#0\b)", src_text, re.M | re.S)
        if obj_sec:
            src_text = (
                src_text[: obj_sec.start(2)]
                + extra_objs
                + "\n"
                + src_text[obj_sec.start(2) :]
            )

    with open(dst_limbo_path, "w", encoding="latin-1", newline="\n") as f:
        f.write(src_text)


def read_config_base():
    """Read non-VNUM keys from data/config.cfg."""
    base_lines = []
    if os.path.exists(CONFIG_CFG_PATH):
        with open(CONFIG_CFG_PATH, "r", encoding="latin-1") as f:
            for line in f:
                stripped = line.strip()
                if not stripped or stripped == "End":
                    continue
                key = stripped.split()[0]
                if key not in OVERRIDE_KEYS:
                    base_lines.append(stripped)
    if not base_lines:
        base_lines = [
            "Slaughter 0",
            "Doubledam 0",
            "Doubleexp 0",
            "Nosun 0",
            "Spookums 0",
            "Arena 0",
            "Resolver 0",
            "Wizlock 0",
            "Newlock 0",
            "Gxp 0",
            "Gqp 0",
            "Xpmult 1",
            "Manualxp 0",
            "Maxon 0",
        ]
    return base_lines


def write_config(include_production_overrides=True):
    base_lines = read_config_base()
    with open(CONFIG_CFG_PATH, "w", encoding="latin-1", newline="\n") as f:
        for line in base_lines:
            f.write(line + "\n")
        if include_production_overrides:
            for key, val in PRODUCTION_VNUM_OVERRIDES:
                f.write(f"{key} {val}\n")
        f.write("End\n")


def resolve_area_list(src_area_dir, area_lst_arg=None):
    """Determine the ordered list of area files to load."""
    candidates = []
    if area_lst_arg:
        candidates.append(area_lst_arg)
    candidates.append(os.path.join(src_area_dir, "area.lst"))
    candidates.append(os.path.join(os.path.dirname(src_area_dir.rstrip("/")), "data", "area.lst"))

    for cand in candidates:
        if cand and os.path.isfile(cand):
            areas = []
            with open(cand, "r", encoding="latin-1") as f:
                for line in f:
                    name = line.strip()
                    if not name or name.startswith("$"):
                        break
                    areas.append(name)
            return areas, cand

    # Fallback: scan directory for .are files
    all_files = sorted(
        f for f in os.listdir(src_area_dir)
        if f.endswith(".are") and f not in EXCLUDED_UNLINKED_AREAS
    )
    return all_files, None


def do_reset():
    """Remove imported non-core area files and restore standalone defaults."""
    removed = 0
    for fname in os.listdir(AREA_DIR):
        if fname.endswith(".are") and fname not in CORE_AREAS:
            os.remove(os.path.join(AREA_DIR, fname))
            removed += 1

    if os.path.exists(LIMBO_BACKUP_PATH):
        shutil.move(LIMBO_BACKUP_PATH, os.path.join(AREA_DIR, "limbo.are"))

    with open(AREA_LST_PATH, "w", encoding="latin-1", newline="\n") as f:
        f.write("limbo.are\ncodespecii.are\n$\n")

    write_config(include_production_overrides=False)
    print(f"Reset complete: removed {removed} external area files and restored standalone defaults.")


def do_import(src_area_dir, area_lst_arg=None):
    src_area_dir = os.path.abspath(src_area_dir)
    if not os.path.isdir(src_area_dir):
        print(f"Error: source directory not found: {src_area_dir}", file=sys.stderr)
        sys.exit(1)

    areas, lst_source = resolve_area_list(src_area_dir, area_lst_arg)

    # Backup standalone limbo.are if not already backed up
    dst_limbo = os.path.join(AREA_DIR, "limbo.are")
    if not os.path.exists(LIMBO_BACKUP_PATH) and os.path.exists(dst_limbo):
        shutil.copy2(dst_limbo, LIMBO_BACKUP_PATH)

    src_limbo = os.path.join(src_area_dir, "limbo.are")
    if os.path.isfile(src_limbo):
        merge_limbo(src_limbo, dst_limbo)

    copied = 0
    ordered_areas = ["limbo.are", "codespecii.are"]

    for fname in areas:
        if fname in CORE_AREAS or fname in EXCLUDED_UNLINKED_AREAS:
            continue
        src_path = os.path.join(src_area_dir, fname)
        if not os.path.isfile(src_path):
            print(f"Warning: {fname} listed in area.lst but not found in {src_area_dir}, skipping.")
            continue
        dst_path = os.path.join(AREA_DIR, fname)
        with open(src_path, "r", encoding="latin-1") as f:
            content = f.read().replace("\r\n", "\n")
        with open(dst_path, "w", encoding="latin-1", newline="\n") as f:
            f.write(content)
        ordered_areas.append(fname)
        copied += 1

    with open(AREA_LST_PATH, "w", encoding="latin-1", newline="\n") as f:
        for fname in ordered_areas:
            f.write(fname + "\n")
        f.write("$\n")

    write_config(include_production_overrides=True)
    print(
        f"Imported {copied} external area files from {src_area_dir} "
        f"(total {len(ordered_areas)} areas in data/area.lst, "
        f"production VNUM overrides written to data/config.cfg)."
    )


def main():
    parser = argparse.ArgumentParser(
        description="Import production .are files and configure VNUM overrides, or reset to standalone defaults."
    )
    parser.add_argument(
        "source_dir",
        nargs="?",
        help="Path to directory containing production .are files",
    )
    parser.add_argument(
        "--area-lst",
        dest="area_lst",
        help="Optional explicit path to production area.lst",
    )
    parser.add_argument(
        "--reset",
        action="store_true",
        help="Remove imported external areas and reset data/area.lst and data/config.cfg to standalone defaults",
    )
    args = parser.parse_args()

    if args.reset:
        do_reset()
    elif args.source_dir:
        do_import(args.source_dir, args.area_lst)
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    main()
