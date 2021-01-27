from math import floor,ceil

systems=[
    # {"name":"BugBear",   "limbs":280,"lgx":8, "d":2,"fec":1,"lpr":2,"variance":3./8}
    # {"name":"GummyBear", "limbs":240,"lgx":9, "d":1,"fec":1,"lpr":2,"variance":1./2},
    # {"name":"TeddyBear", "limbs":240,"lgx":9, "d":2,"fec":1,"lpr":2,"variance":7./32},
    # {"name":"BabyBear",  "limbs":312,"lgx":10,"d":2,"fec":1,"lpr":2,"variance":11.0/32},
    # {"name":"MamaBear",  "limbs":312,"lgx":10,"d":3,"fec":1,"lpr":2,"variance":8.0/32},
    # {"name":"PapaBear",  "limbs":312,"lgx":10,"d":4,"fec":1,"lpr":2,"variance":7.0/32},
    # {"name":"WeeBear",   "limbs":260,"lgx":12,"d":2,"fec":0,"lpr":2,"variance":1.},
    # {"name":"MiddleBear","limbs":260,"lgx":12,"d":3,"fec":0,"lpr":2,"variance":3./4},
    # {"name":"GreatBear", "limbs":260,"lgx":12,"d":4,"fec":0,"lpr":2,"variance":1./2}

    #{"name":"DropBear",  "limbs":390,"lgx":8,"d":2,"fec":0,"lpr":4,"variance":1./4},
    #{"name":"DropBear",  "limbs":312,"lgx":10,"d":2,"fec":0,"lpr":4,"variance":5./4},
    
    #{"name":"GummyBear", "limbs":270,"lgx":8, "d":1,"fec":0,"lpr":4,"variance":7./32},
    # {"name":"TeddyBearPlus", "limbs":390,"lgx":8, "d":1,"fec":1,"lpr":4,"variance":7./32},
    
    #{"name":"PapaBear",  "limbs":312,"lgx":10,"d":4,"fec":0,"iv":8,"lpr":4,"variance": 8./32},


    {"name":"GummyBear",          "keygen_seed_bytes":24,"es_bytes":12,"limbs":120,"lgx":9,"cca":0,"d":1,"fec":1,"lpr":4,"variance":1.,"matrix_seed_bytes":8,"toy":True,"shared_secret_bytes":12},
    {"name":"TeddyBear",          "keygen_seed_bytes":24,"es_bytes":24,"limbs":240,"lgx":9,"cca":1,"d":1,"fec":1,"lpr":4,"variance":3./4.,"matrix_seed_bytes":8,"toy":True,"shared_secret_bytes":24},

    {"name":"KoalaEphem",     "keygen_seed_bytes":24,"es_bytes":24,"limbs":240,"lgx":9,"cca":0,"d":2,"fec":1,"lpr":4,"variance":21./32,"matrix_seed_bytes":16,"toy":True,"shared_secret_bytes":24},
    {"name":"Koala",          "keygen_seed_bytes":24,"es_bytes":24,"limbs":240,"lgx":9,"cca":1,"d":2,"fec":1,"lpr":4,"variance":11./32,"matrix_seed_bytes":16,"toy":True,"shared_secret_bytes":24},

    {"name":"DropBear",           "limbs":312,"lgx":10,"cca":1,"d":2,"fec":1,"lpr":4,"variance":8./4,"toy":True},

    #{"name":"BabyBearMinusEphem",      "limbs":312,"lgx":10,"cca":0,"d":2,"fec":0,"lpr":4,"variance":20./32},
    #{"name":"BabyBearMinus",           "limbs":312,"lgx":10,"cca":1,"d":2,"fec":0,"lpr":4,"variance":12./32},
    {"name":"BabyBearEphem",  "limbs":312,"lgx":10,"cca":0,"d":2,"fec":1,"lpr":4,"variance":32./32},
    {"name":"BabyBear",       "limbs":312,"lgx":10,"cca":1,"d":2,"fec":1,"lpr":4,"variance":18./32},
    #{"name":"MamaBearMinusEphem",      "limbs":312,"lgx":10,"cca":0,"d":3,"fec":0,"lpr":4,"variance":16./32},
    #{"name":"MamaBearMinus",           "limbs":312,"lgx":10,"cca":1,"d":3,"fec":0,"lpr":4,"variance": 9./32},
    {"name":"MamaBearEphem",  "limbs":312,"lgx":10,"cca":0,"d":3,"fec":1,"lpr":4,"variance":28./32},
    {"name":"MamaBear",       "limbs":312,"lgx":10,"cca":1,"d":3,"fec":1,"lpr":4,"variance":13./32},
    #{"name":"PapaBearMinusEphem",      "limbs":312,"lgx":10,"cca":0,"d":4,"fec":0,"lpr":4,"variance":14./32,"iv":0},
    #{"name":"PapaBearMinus",           "limbs":312,"lgx":10,"cca":1,"d":4,"fec":0,"iv":0,"lpr":4,"variance": 7./32,"iv":0},
    {"name":"PapaBearEphem",  "limbs":312,"lgx":10,"cca":0,"d":4,"fec":1,"lpr":4,"variance":24./32,"iv":0},
    {"name":"PapaBear",       "limbs":312,"lgx":10,"cca":1,"d":4,"fec":1,"lpr":4,"variance":10./32,"iv":0},
    #{"name":"PapaBearPlus",  "limbs":312,"lgx":10,"d":4,"fec":1,"iv":8,"lpr":4,"variance":3./8},
    
    #{"name":"WeeBear",   "limbs":260,"lgx":12,"d":2,"fec":0,"lpr":4,"variance":12./8.}, # Could be 14/8 but can't sample that
    #{"name":"MiddleBear","limbs":260,"lgx":12,"d":3,"fec":0,"lpr":4,"variance":10./8.},
    #{"name":"GreatBear", "limbs":260,"lgx":12,"d":4,"fec":0,"lpr":4,"variance":8./8.}
]

# make cached version
for s in list(systems):
    if "cached" not in s: s["cached"]=0
    ss = dict(s) # copy
    ss["cached"] = 1
    ss["realname"] = s["realname"] = s["name"]
    ss["name"] = ss["name"]+"C"
    systems.append(ss)

# supercop names
for s in list(systems):
    if "toy" in s and s["toy"]: continue
    ss = dict(s) # copy
    ss["name"] = "threebears" + str(ss["limbs"]*ss["d"]) + "r2" + ["cpa","cca"][s["cca"]]+["","x"][ss["cached"]]
    systems.append(ss)

fec_bits = 18 # Melas
implicit_reject = False
cache = False

extra_requirements=[]

def require(x,systems_local,value=True): return any(value==bool(s[x]) for s in systems_local) or x in extra_requirements

for s in systems:
    if "toy" not in s: s["toy"] = False
    if "iv" not in s: s["iv"] = 0
    if "targhi_unruh_bytes" not in s: s["targhi_unruh_bytes"] = 0
    if "matrix_seed_bytes" not in s: s["matrix_seed_bytes"] = 24
    if "shared_secret_bytes" not in s: s["shared_secret_bytes"] = 32
    if "keygen_seed_bytes" not in s: s["keygen_seed_bytes"] = 40
    s["bits"] = ((s["limbs"]*s["lgx"]+7)//8)*8
    
    s["pk_bytes"] = int(s["bits"]*s["d"]//8 + s["matrix_seed_bytes"])
    
    if s["cca"]:
        s["cached_sk_bytes"] = 2*int(s["bits"]*s["d"]/8) + s["keygen_seed_bytes"]+s["pk_bytes"]
    else:
        s["cached_sk_bytes"] = int(s["bits"]*s["d"]/8) + s["matrix_seed_bytes"]
        
    
    if s["cached"]: s["sk_bytes"] = s["cached_sk_bytes"]
    else: s["sk_bytes"] = s["keygen_seed_bytes"]
    if s["cca"] and implicit_reject and cache: s["sk_bytes"] += s["keygen_seed_bytes"]    
    if s["cca"] and cache: s["sk_bytes"] += s["pk_bytes"]

    if "es_bytes" not in s:
        s["es_bytes"] = min(int(floor((s["limbs"]-s["fec"]*fec_bits)/8.0)),32)
        
    s["capsule_bytes"] = int(
        ceil(s["d"]*(s["bits"]//8))
        + s["iv"]
        + ceil(
                (s["es_bytes"]*8 + s["fec"]*fec_bits)*s["lpr"]/8.
        )
        + s["targhi_unruh_bytes"]
    )
    s["ss_bytes"] = s["shared_secret_bytes"]
        

