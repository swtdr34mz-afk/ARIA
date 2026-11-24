#!/usr/bin/env python3
# SPDX-License-Identifier: MPL-2.0
# Copyright (C) 2026 swtdr34mz (ARIA)
"""ARIA for the MPC: builds the plugin folder with Akai's MPC Plugin SDK.

The SDK builds a project of C++ files in one src/ folder. This makes that project from this repository, so the MPC
plugin always has the same engine as the VST3 (nothing copied by hand):

  1. mpc/build/ARIA/          the SDK project: plugin.json and README.md from mpc/plugin/; in src/ AriaMpc.cpp and the
                              engine (dsp/aria.c, params.c, presets.c as .cpp: C that is C++ too, and their headers)
  2. params_data.cpp          the parameter tables, written out by mpc/gen/gen_params.c (built here as C with the
                              computer's own compiler: dsp/params_data.c uses C's designated initialisers)
  3. skins/                   the screen: the VST3's panel as the MPC's pages (mpc/skin/, kept in mpc/plugin/skins/)
  4. the SDK's tools/build.py native build + host simulator, arm32 and arm64, the skins, version.xml, validation

    python3 mpc/build.py --sdk /path/to/mpc-plugin-builder-sdk [--target native|arm32|arm64|all] [--toolchain zig|gcc]

The result: mpc/build/ARIA/build/swtdr34mz - VST - ARIA/ (copy it into "VST2 Plugins" on a drive of the MPC).
"""
import argparse
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
NAME = "ARIA"
ENGINE = {"dsp/aria.c": "aria.cpp", "dsp/params.c": "params.cpp", "dsp/presets.c": "presets.cpp",
          "dsp/aria.h": "aria.h", "dsp/presets.h": "presets.h"}


def run(argv, **kw):
    print("+ " + " ".join('"%s"' % a if " " in a else a for a in argv), flush=True)
    subprocess.check_call(argv, **kw)


def find_tool(env, names):
    if os.environ.get(env):
        return os.environ[env]
    for n in names:
        p = shutil.which(n)
        if p:
            return p
    sys.exit("no %s found (set %s)" % ("/".join(names), env))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--sdk", default=os.environ.get("MPC_SDK", os.path.join(ROOT, "..", "mpc-plugin-builder-sdk")))
    ap.add_argument("--target", default="all")
    ap.add_argument("--toolchain", default=None)
    ap.add_argument("--no-validate", action="store_true")
    a = ap.parse_args()
    sdk = os.path.abspath(a.sdk)
    sdk_build = os.path.join(sdk, "tools", "build.py")
    if not os.path.isfile(sdk_build):
        sys.exit("the MPC Plugin SDK is not at %s (--sdk or MPC_SDK)" % sdk)

    proj = os.path.join(HERE, "build", NAME)
    src = os.path.join(proj, "src")
    if os.path.isdir(src):
        shutil.rmtree(src)
    os.makedirs(src)
    for f in ("plugin.json", "README.md"):
        shutil.copy2(os.path.join(HERE, "plugin", f), os.path.join(proj, f))
    for f in os.listdir(os.path.join(HERE, "plugin", "src")):
        shutil.copy2(os.path.join(HERE, "plugin", "src", f), os.path.join(src, f))
    for f, to in ENGINE.items():
        shutil.copy2(os.path.join(ROOT, f), os.path.join(src, to))

    gen = os.path.join(HERE, "build", "gen")
    os.makedirs(gen, exist_ok=True)
    cc = find_tool("CC", ["cc", "gcc", "clang"])
    exe = os.path.join(gen, "gen_params" + (".exe" if os.name == "nt" else ""))
    run([cc, "-std=c11", "-O1", "-I" + os.path.join(ROOT, "dsp"), os.path.join(HERE, "gen", "gen_params.c"),
         os.path.join(ROOT, "dsp", "params.c"), os.path.join(ROOT, "dsp", "params_data.c"), "-lm", "-o", exe])
    pj = os.path.join(gen, "params.json")
    run([exe, os.path.join(src, "params_data.cpp"), pj])
    skins = os.path.join(proj, "skins")  # (the SDK copies a project's skins/ as they are, as "Plugin Skins")
    if os.path.isdir(skins):
        shutil.rmtree(skins)
    shutil.copytree(os.path.join(HERE, "plugin", "skins"), skins)
    stale = os.path.join(proj, "ui.json")   # (the generated pages of before: the skin replaces them)
    if os.path.isfile(stale):
        os.remove(stale)

    argv = [sys.executable, sdk_build, proj, "--target", a.target]
    if a.toolchain:
        argv += ["--toolchain", a.toolchain]
    if a.no_validate:
        argv.append("--no-validate")
    run(argv)
    out = os.path.join(proj, "build", "swtdr34mz - VST - " + NAME)
    if os.path.isdir(out):
        print("\nthe plugin folder: %s" % out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
