#!/usr/bin/env python3
"""Moog Muse patches (.mmp, the text files the Muse shows in its disk mode) to ARIA user presets (.aria).

    python3 tools/muse2aria.py OUT_FOLDER patch1.mmp patch2.mmp ...

Each Muse setting goes to ARIA's control for it, in ARIA's units (seconds to envelope times, semitones to the
oscillators' FREQ, seconds to delay times, ...); what ARIA does not have (the mod map's controller routings, the
envelopes' separate curves, the arpeggiator's extras) is left out, and the modulation map's LFO routings become the
LFOs' destinations. The loudness is evened out to that of ARIA's factory sounds (tools/muse_measure.c, via ctypes).
The .aria files go to Documents/ARIA/Presets; ARIA lists them after the factory sounds."""
import ctypes, math, os, re, subprocess, sys, tempfile
from xml.sax.saxutils import quoteattr

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

# ---------------------------------------------------------------- ARIA's parameter numbers, from dsp/aria.h
def aria_params():
    h = open(os.path.join(ROOT, 'dsp', 'aria.h')).read()
    tp = re.search(r'enum \{\s*(TP_MO_FREQ.*?)TP_COUNT', h, re.S).group(1)
    tnames = [n for n in re.findall(r'\b(TP_\w+)', re.sub(r'/\*.*?\*/', '', tp, flags=re.S))]
    T = len(tnames)
    idx = {n: i for i, n in enumerate(tnames)}
    gp = re.search(r'(GP_VC_MODE = GP_FIRST.*?)ARIA_NPARAM', h, re.S).group(1)
    gp = re.sub(r'/\*.*?\*/', '', gp, flags=re.S)
    k = 2 * T
    for tok in [t.strip() for t in gp.split(',') if t.strip()]:
        if tok.startswith('GP_ARP_STEP16'): continue
        name = tok.split('=')[0].strip()
        idx[name] = k
        k += 16 if name == 'GP_ARP_STEP1' else 1
    return idx, T

IDX, T = aria_params()
ENUM = {}
for m in re.finditer(r'enum \{([^}]*)\}', open(os.path.join(ROOT, 'dsp', 'aria.h')).read()):
    names = [x.split('=')[0].strip() for x in re.sub(r'/\*.*?\*/', '', m.group(1), flags=re.S).split(',')]
    for i, n in enumerate(names):
        if re.match(r'^(OCT|MOW|DEST1|DESTF|KB|ORD|LW|LD|VC|ARP|DIV)_\w+$', n): ENUM[n] = i

# ---------------------------------------------------------------- the .mmp reader
def read_mmp(path):
    txt = open(path, encoding='utf-8', errors='replace').read()
    out = {}
    def val(s):
        s = s.strip()
        if s in ('true', 'false'): return s == 'true'
        if s.startswith('"'): return s.strip('"')
        s = s.rstrip('L')
        try: return float(s)
        except ValueError: return s
    for m in re.finditer(r'(\w+)\s*:\s*\{\s*(value|timbreValues)\s*=\s*(.*?);\s*\}', txt, re.S):
        key, kind, raw = m.groups()
        if kind == 'value': out[key] = val(raw)
        else: out[key] = [val(x) for x in raw.strip('[] \n').split(',')]
    return out

# ---------------------------------------------------------------- conversions
def clamp(x, a=0.0, b=1.0): return max(a, min(b, x))
def env_time(sec): return clamp(math.log(max(sec, 0.0005) / 0.0005) / math.log(40000.0))     # ARIA: 0.5 ms * 40000^v
def delay_time(sec): return clamp(math.log(max(sec * 1000.0, 1.0)) / math.log(2000.0))        # ARIA: 2000^v ms
def muse_rate(v, lo, hi): return lo + (hi - lo) * v * v                                       # (an estimate of the Muse's rate knob)
def lfo_rate(hz): return clamp(math.log(max(hz, 0.02) / 0.02) / math.log(5000.0))              # ARIA LFO: 0.02 Hz * 5000^v
def plfo_rate(hz): return clamp(math.log(max(hz, 0.5) / 0.5) / math.log(40.0))                 # ARIA PITCH LFO: 0.5 Hz * 40^v
OCT = {"16'": 'OCT_16', "8'": 'OCT_8', "4'": 'OCT_4', "2'": 'OCT_2', "32'": 'OCT_16'}
MOW = {'sine': 'MOW_SINE', 'saw': 'MOW_SAW', 'ramp': 'MOW_RAMP', 'square': 'MOW_SQUARE', 'noise': 'MOW_NOISE', 'random': 'MOW_NOISE'}
LW = {'sine': 'LW_SINE', 'triangle': 'LW_TRI', 'saw': 'LW_SAW', 'ramp': 'LW_RAMP', 'square': 'LW_SQUARE',
      'random': 'LW_SH', 'sh': 'LW_SH', 'user': 'LW_SINE'}
KB = {'off': 'KB_OFF', 'half': 'KB_HALF', 'full': 'KB_FULL'}
ORD = {'serial': 'ORD_SERIAL', 'stereo': 'ORD_STEREO', 'parallel': 'ORD_PARALLEL', 'par': 'ORD_PARALLEL'}
# the mod map's destinations that an ARIA LFO can reach, and how deep a full Muse depth goes there
DEST = {'filter1Cutoff': ('LD_F1', 2.5), 'filter2Cutoff': ('LD_F2', 2.5), 'filterLinkedCutoff': ('LD_FILTERS', 2.5),
        'osc1Freq': ('LD_OSC1', 'pitch'), 'osc2Freq': ('LD_OSC2', 'pitch'), 'kb': ('LD_PITCH', 'pitch'),
        'osc1Pw': ('LD_PW', 1.0), 'osc2Pw': ('LD_PW', 1.0), 'timbreVol': ('LD_AMP', 1.0),
        'vcaPanSpread': ('LD_PAN', 1.0), 'vcaPan': ('LD_PAN', 1.0), 'modOscFreq': ('LD_MOFREQ', 1.0),
        'modOscPitch': ('LD_MOFREQ', 1.0), 'fmAmount': ('LD_FM', 1.0), 'oscFmAmount': ('LD_FM', 1.0)}


# the mod map's routings that make up the sound at rest, written into it: the envelopes onto a cutoff become that
# filter's envelope amount, velocity counts as a medium touch, the MACRO as the patch keeps it
MM_KEY = {'filter1Cutoff': ['filter1Cutoff'], 'filter2Cutoff': ['filter2Cutoff'],
          'filterLinkedCutoff': ['filter1Cutoff', 'filter2Cutoff'], 'filter1Res': ['filter1Resonance'],
          'filter2Res': ['filter2Resonance'], 'filter1EnvAmount': ['filter1EnvelopeAmount'],
          'filter2EnvAmount': ['filter2EnvelopeAmount'], 'osc1TriSaw': ['oscillator1TriRampAngle'],
          'osc2TriSaw': ['oscillator2TriRampAngle'], 'osc1Pw': ['oscillator1PulseWidth'],
          'osc2Pw': ['oscillator2PulseWidth'], 'osc1WaveMix': ['oscillator1WaveshapeMix'],
          'osc2WaveMix': ['oscillator2WaveshapeMix'], 'mixOsc1': ['mixerOsc1'], 'mixOsc2': ['mixerOsc2'],
          'mixModOsc': ['mixerModOsc'], 'mixNoise': ['mixerNoise'], 'mixRingMod': ['mixerRingMod'],
          'mixClipping': ['mixerClipping'], 'detune': ['voiceControlDetune'], 'vcaPanSpread': ['vcaPanSpread'],
          'timbreVol': ['timbreVolume'], 'modOscPitchAmount': ['modulationOscillatorPitchAmount'],
          'modOscFilterAmount': ['modulationOscillatorFilterAmount'], 'pitchLfoAmount': ['pitchLfoAmount']}
ENV_TO = {'filter1Cutoff': ['filter1EnvelopeAmount'], 'filter2Cutoff': ['filter2EnvelopeAmount'],
          'filterLinkedCutoff': ['filter1EnvelopeAmount', 'filter2EnvelopeAmount']}

def fold_modmap(m):
    def add(key, t, x, lo=0.0, hi=1.0):
        v = m.get(key)
        if isinstance(v, list) and isinstance(v[t], (int, float)) and not isinstance(v[t], bool):
            v[t] = max(lo, min(hi, v[t] + x))
    for t in (0, 1):
        macro = (m.get('macro') or [0, 0])[t]
        for s in range(1, 17):
            def at(k, d=None):
                x = m.get('%s%02d' % (k, s))
                return x[t] if isinstance(x, list) else d
            src, dst, dep = at('modMapSource', 'none'), at('modMapDest', 'none'), at('modMapFunctionDepth', 0.0) or 0.0
            if src == 'none' or dst == 'none' or abs(dep) < 1e-3: continue
            if src in ('vcaEnv', 'filterEnv') and dst in ENV_TO:
                for k in ENV_TO[dst]: add(k, t, dep, -1.0, 1.0)
            elif src == 'velocity' and (dst in ENV_TO or dst.endswith('EnvAmount')):
                for k in MM_KEY.get(dst, []): add(k, t, dep * 0.7, -1.0 if 'Env' in k else 0.0, 1.0)
            elif src in ('macroUni', 'macroBi') and dst in MM_KEY:
                x = macro if src == 'macroUni' else 2 * macro - 1
                for k in MM_KEY[dst]: add(k, t, x * dep, -1.0 if 'Envelope' in k else 0.0, 1.0)
    return m

def convert(m):
    m = fold_modmap(m)
    v = [0.0] * NP
    for p in range(NP): v[p] = LIB.mm_default(p)
    def setp(name, val, t=None):
        if isinstance(val, str): val = ENUM[val]
        p = IDX[name] + (t * T if t is not None else 0)
        v[p] = clamp(float(val), 0.0, LIB.mm_max(p))
    def g(key, t, default=None):
        x = m.get(key, default)
        return x[t] if isinstance(x, list) else x
    notes = []
    for t in (0, 1):
        # MOD OSC
        setp('TP_MO_FREQ', g('modulationOscillatorPitch', t, 0.4), t)
        setp('TP_MO_WAVE', MOW.get(g('modulationOscillatorWaveshape', t, 'sine'), 'MOW_SINE'), t)
        for k, n in (('Audio', 'TP_MO_AUDIO'), ('KbTrack', 'TP_MO_KBTRK'), ('KbReset', 'TP_MO_KBRESET'), ('Unipolar', 'TP_MO_UNI')):
            setp(n, 1 if g('modulationOscillator' + k, t, False) else 0, t)
        o1, o2 = g('modulationOscillatorPitchOsc1', t, False), g('modulationOscillatorPitchOsc2', t, False)
        setp('TP_MO_PITCH', g('modulationOscillatorPitchAmount', t, 0) if (o1 or o2) else 0, t)
        setp('TP_MO_PITCH_DEST', 'DEST1_BOTH' if o1 and o2 else ('DEST1_OSC2' if o2 else 'DEST1_OSC1'), t)
        f1, f2 = g('modulationOscillatorFilter1', t, False), g('modulationOscillatorFilter2', t, False)
        setp('TP_MO_FILTER', g('modulationOscillatorFilterAmount', t, 0) if (f1 or f2) else 0, t)
        setp('TP_MO_FILTER_DEST', 'DESTF_BOTH' if f1 and f2 else ('DESTF_F2' if f2 else 'DESTF_F1'), t)
        pw = g('modulationOscillatorPwmOsc1', t, False) or g('modulationOscillatorPwmOsc2', t, False)
        setp('TP_MO_PWM', g('modulationOscillatorPwmAmount', t, 0) if pw else 0, t)
        vca = g('modulationOscillatorVcaAmount', t, 0)
        if g('modulationOscillatorVcaPan', t, False): setp('TP_MO_PAN', vca, t); setp('TP_MO_VCA', 0, t)
        else: setp('TP_MO_VCA', vca, t); setp('TP_MO_PAN', 0, t)
        # OSCILLATORS
        for o in ('1', '2'):
            P = 'TP_O' + o + '_'
            setp(P + 'OCT', OCT.get(g('oscillator%sOctave' % o, t, "8'"), 'OCT_8'), t)
            setp(P + 'FREQ', 0.5 + g('oscillator%sPitch' % o, t, 0.0) / 14.0, t)        # (semitones, +-7)
            setp(P + 'TRISAW', g('oscillator%sTriRampAngle' % o, t, 1.0), t)
            setp(P + 'PW', g('oscillator%sPulseWidth' % o, t, 0.5), t)
            setp(P + 'MIX', g('oscillator%sWaveshapeMix' % o, t, 0.0), t)
        setp('TP_O2_SYNC', 1 if g('oscillatorsSync', t, False) else 0, t)
        setp('TP_FM_AMT', g('oscillatorsFmAmount', t, 0), t)
        setp('TP_FM_21', 1 if g('oscillator1Fm', t, False) else 0, t)
        setp('TP_FM_12', 1 if g('oscillator2Fm', t, False) else 0, t)
        # MIXER
        for k, n in (('Osc1', 'TP_MX_O1'), ('RingMod', 'TP_MX_RING'), ('Osc2', 'TP_MX_O2'), ('ModOsc', 'TP_MX_MOD'),
                     ('Noise', 'TP_MX_NOISE'), ('Clipping', 'TP_MX_OVERLOAD')):
            setp(n, g('mixer' + k, t, 0), t)
        # FILTERS
        for f in ('1', '2'):
            P = 'TP_F' + f + '_'
            setp(P + 'CUT', g('filter%sCutoff' % f, t, 1.0), t)
            setp(P + 'RES', g('filter%sResonance' % f, t, 0.0), t)
            setp(P + 'ENV', 0.5 + 0.5 * g('filter%sEnvelopeAmount' % f, t, 0.0), t)
            setp(P + 'KB', KB.get(g('filter%sKbTrack' % f, t, 'off'), 'KB_OFF'), t)
        setp('TP_F1_HP', 1 if g('filter1HighPass', t, False) else 0, t)
        # LINK: on the Muse filter 1's cutoff moves both and filter 2's is the distance to it; written out here
        if g('filtersLinkCutoffs', t, False):
            setp('TP_F2_CUT', clamp(g('filter1Cutoff', t, 1.0) + g('filter2Cutoff', t, 0.5) - 0.5), t)
        setp('TP_F_LINK', 0, t)
        setp('TP_F_ORDER', ORD.get(g('filtersOrder', t, 'serial'), 'ORD_SERIAL'), t)
        # ENVELOPES (the Muse's in seconds)
        for mu, ar in (('filterEnvelope', 'TP_FE_'), ('vcaEnvelope', 'TP_AE_')):
            setp(ar + 'A', env_time(g(mu + 'Attack', t, 0.001)), t)
            setp(ar + 'D', env_time(g(mu + 'Decay', t, 1.0)), t)
            setp(ar + 'S', g(mu + 'Sustain', t, 0.5), t)
            setp(ar + 'R', env_time(g(mu + 'Release', t, 0.5)), t)
            setp(ar + 'LOOP', 1 if g(mu + 'Loop', t, False) else 0, t)
            setp(ar + 'VEL', 1 if g(mu + 'Velocity', t, False) else 0, t)
        # VCA
        setp('TP_VCA_LEVEL', g('timbreVolume', t, 0.8), t)
        setp('TP_VCA_PAN', g('vcaPan', t, 0.5), t)
        setp('TP_VCA_SPREAD', g('vcaPanSpread', t, 0.0), t)
        # LFOs: rate and wave from the timbre's LFO, the destination from the mod map's strongest routing of it
        for l, n in (('1', 'TP_L1_'), ('2', 'TP_L2_')):
            hi = g('lfo%sRateHighLimit' % l, t, 40.0)
            hz = muse_rate(g('lfo%sRate' % l, t, 0.1), g('lfo%sRateLowLimit' % l, t, 0.01), min(hi, 40.0))
            setp(n + 'RATE', lfo_rate(hz), t)
            setp(n + 'WAVE', LW.get(g('lfo%sWaveshape' % l, t, 'triangle'), 'LW_TRI'), t)
            best = None
            for s in range(1, 17):
                src = g('modMapSource%02d' % s, t, 'none')
                dst = g('modMapDest%02d' % s, t, 'none')
                dep = g('modMapFunctionDepth%02d' % s, t, 0.0) or 0.0
                if src in ('lfo%sBi' % l, 'lfo%sUni' % l) and dst in DEST and abs(dep) > 1e-3:
                    if best is None or abs(dep) > abs(best[1]): best = (dst, dep)
            if best:
                dest, scale = DEST[best[0]]
                depth = abs(best[1]) * g('lfo%sAmount' % l, t, 1.0)
                # pitch: ARIA's depth^2 * 12 semitones; a Muse depth of 1 taken as +-24 semitones
                amt = math.sqrt(min(1.0, depth * 2.0)) if scale == 'pitch' else clamp(depth * scale)
                setp(n + 'DEST', dest, t); setp(n + 'AMT', amt, t)
            else:
                setp(n + 'AMT', 0, t)
        # PITCH LFO (vibrato), when it reaches an oscillator
        if g('pitchLfoOsc1', t, False) or g('pitchLfoOsc2', t, False):
            setp('TP_PL_AMT', g('pitchLfoAmount', t, 0), t)
        else:
            setp('TP_PL_AMT', 0, t)
        setp('TP_PL_RATE', plfo_rate(muse_rate(g('pitchLfoRate', t, 0.5), 0.01, 40.0)), t)
        setp('TP_PL_SHAPE', g('pitchLfoWaveshape', t, 0.0), t)
        # VOICE
        setp('TP_GLIDE', g('glide', t, 0), t)
        setp('TP_DETUNE', g('voiceControlDetune', t, 0), t)
        setp('TP_UNISON', 1 if g('voiceControlUnison', t, False) else 0, t)
        setp('TP_MONO', 1 if g('voiceControlMono', t, False) else 0, t)
        setp('TP_BEND', min(12, float(g('pitchWheelRange', t, '2') or 2)) / 12.0, t)
    # VOICE CONTROL: single, split or stack
    if m.get('timbreSplit'): setp('GP_VC_MODE', 'VC_SPLIT')
    elif m.get('timbreStack'): setp('GP_VC_MODE', 'VC_STACK')
    else: setp('GP_VC_MODE', 'VC_SINGLE')
    setp('GP_VC_SPLIT', m.get('voiceControlSplitPoint', 60) / 127.0)
    setp('GP_VC_VOICES_A', clamp(m.get('voiceControlTimbreACount', 4) - 1, 0, 6))
    # DELAY
    setp('GP_DL_TIME_L', delay_time(m.get('delayTimeLeft', 0.3)))
    setp('GP_DL_TIME_R', delay_time(m.get('delayTimeRight', 0.3)))
    setp('GP_DL_LINK', 0)
    setp('GP_DL_FB', m.get('delayFeedback', 0.35))
    setp('GP_DL_MIX', m.get('delayMix', 0) * m.get('delaySend', 1.0))
    setp('GP_DL_DIFF', 0.75 if m.get('delayCharacterDiffusion') == 'on' else 0.2)
    lpf = m.get('delayLpf', 11750.0)        # (a dark delay: the Muse's low-pass well down)
    setp('GP_DL_CHAR', clamp(0.5 + 0.12 * math.log2(lpf / 11750.0), 0.15, 0.6))
    setp('GP_DL_A', 1 if m.get('delayTimbreAlpha', True) else 0)
    setp('GP_DL_B', 1 if m.get('delayTimbreBeta', True) else 0)
    # ARPEGGIATOR (timbre A's)
    arp = m.get('arpOn', [False, False])
    setp('GP_ARP_ON', 1 if (any(arp) if isinstance(arp, list) else arp) else 0)
    order = {'up': 'ARP_UP', 'dn': 'ARP_DOWN', 'down': 'ARP_DOWN', 'ud': 'ARP_UPDOWN', 'updn': 'ARP_UPDOWN',
             'ord': 'ARP_ORDER', 'pat': 'ARP_ORDER', 'rnd': 'ARP_RANDOM', 'rand': 'ARP_RANDOM'}
    setp('GP_ARP_MODE', order.get(str(m.get('arpOrder', 'up')), 'ARP_UP'))
    try: setp('GP_ARP_OCT', clamp(int(float(m.get('arpRange', '1'))) - 1, 0, 3))
    except ValueError: pass
    setp('GP_ARP_GATE', m.get('arpGateLen', 0.5))
    setp('GP_ARP_SWING', clamp((m.get('arpSwing', 0.5) - 0.5) * 2.0))
    div = m.get('arpClockDiv', 0.5)         # (the Muse's knob, roughly: slow to fast)
    setp('GP_ARP_DIV', 'DIV_4' if div < 0.3 else 'DIV_8' if div < 0.45 else 'DIV_16' if div < 0.75 else 'DIV_32')
    return v, notes

def level(v):
    """VCA LEVEL A and B so the loudest 100 ms of a chord sits where the factory sounds do (rms 0.15)."""
    mono = v[IDX['TP_MONO']] > 0.5
    pk = ctypes.c_float()
    arr = (ctypes.c_float * NP)(*v)
    for _ in range(5):
        rms = LIB.mm_measure(arr, 1 if mono else 0, ctypes.byref(pk))
        if rms < 1e-5: break
        gain = math.sqrt(0.15 / rms)
        if abs(gain - 1) < 0.02: break
        for t in (0, 1):
            p = IDX['TP_VCA_LEVEL'] + t * T
            want = arr[p] * gain
            arr[p] = clamp(want, 0.05, 1.0)
            if want > 1.0:                  # (the VCA at the top: the rest from the mixer, as one turns it up)
                for k in ('TP_MX_O1', 'TP_MX_O2', 'TP_MX_RING', 'TP_MX_MOD', 'TP_MX_NOISE'):
                    q = IDX[k] + t * T
                    arr[q] = clamp(arr[q] * min(want, 2.0))
    rms = LIB.mm_measure(arr, 1 if mono else 0, ctypes.byref(pk))
    return list(arr), rms, pk.value

def build_lib():
    so = os.path.join(tempfile.gettempdir(), 'libaria_muse.so')
    subprocess.check_call(['cc', '-O2', '-shared', '-fPIC', '-I' + os.path.join(ROOT, 'dsp'), '-o', so,
                           os.path.join(HERE, 'muse_measure.c')] +
                          [os.path.join(ROOT, 'dsp', f) for f in ('aria.c', 'params.c', 'params_data.c')] + ['-lm'])
    lib = ctypes.CDLL(so)
    lib.mm_default.restype = ctypes.c_float; lib.mm_max.restype = ctypes.c_float
    lib.mm_measure.restype = ctypes.c_float
    lib.mm_measure.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.c_int, ctypes.POINTER(ctypes.c_float)]
    return lib

LIB = build_lib()
NP = LIB.mm_nparam()

def name_of(path):
    n = os.path.splitext(os.path.basename(path))[0]
    n = re.sub(r'^[0-9a-f]{8}-', '', n)            # (an upload's prefix)
    return re.sub(r'[^A-Z0-9 +\-]', '', n.replace('_', ' ').upper()).strip()[:20] or 'MUSE'

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(__doc__); sys.exit(1)
    out = sys.argv[1]
    os.makedirs(out, exist_ok=True)
    for path in sys.argv[2:]:
        m = read_mmp(path)
        v, _ = convert(m)
        v, rms, pk = level(v)
        name = name_of(path)
        bp = re.search(r'bank(\d+)[\\/]patch(\d+)', path)
        fname = ('%s-%s %s' % (bp.group(1), bp.group(2), name)) if bp else name
        attrs = ' '.join('p%d="%s"' % (i, ('%.6f' % x).rstrip('0').rstrip('.') or '0') for i, x in enumerate(v))
        xml = '<?xml version="1.0" encoding="UTF-8"?>\n\n<ARIAPRESET name=%s version="1" %s/>\n' % (quoteattr(name), attrs)
        fn = os.path.join(out, fname + '.aria')
        open(fn, 'w').write(xml)
        mode = ['SINGLE', 'SPLIT', 'STACK'][int(v[IDX['GP_VC_MODE']])]
        print('%-22s %-6s rms %.3f peak %.2f  -> %s' % (name, mode, rms, pk, fn))
