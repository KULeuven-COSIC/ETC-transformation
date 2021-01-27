from textwrap import dedent

import copy
import os
import argparse
import re
import sys
import template_data
import math

parser = argparse.ArgumentParser(description='Generate Decaf headers and other such files.')
parser.add_argument('-o', required = True, metavar="output", help = "Output")
parser.add_argument('files', metavar='file', type=str, nargs=1, help='a file to fill')
parser.add_argument('--systems', metavar='systems', type=str, nargs=1, default=[], help='systems to implement')
parser.add_argument('--namespace', metavar='namespace', type=str, nargs=2, default=["",""], help='prefix and postfix for namespacer')
args = parser.parse_args()

if len(args.systems):
    args.systems = [u.upper() for u in re.split("\W+",args.systems[0])]
    template_data.systems = [t for t in template_data.systems if t["name"].upper() in args.systems]
    found = set(t["name"].upper() for t in template_data.systems)
    if len(found) != len(set(args.systems)):
        print >> sys.stderr, "Unrecognized system(s):", [s for s in args.systems if s not in found]
        sys.exit(1)
else:
    template_data.systems = [t for t in template_data.systems if not t["toy"]]

def getparens(s,start,open="(",close=")"):
    parens = 1
    position=start
    while parens > 0:
        if s[position] == open: parens += 1
        elif s[position] == close: parens -= 1
        position += 1
    return s[start:position-1],position

env0 = {
    "re":re,
    "field": [
        {"name":"small","bits":2160},
        {"name":"big","bits":3120}
    ],
    "_":None,
    "ns":args.namespace
}
for p in dir(template_data): env0[p] = getattr(template_data,p)
for p in dir(math):
    if p not in env0: env0[p] = getattr(math,p)


oncelers = set()
namespaces = {}
def process(data,env=env0,stripnnn=False,replacements={}):
    global oncelers
    global namespaces
    
    replacements = copy.copy(replacements) # so we can push/pop
    def adjust_namespaces():
        if "name" not in env: return
        for sym,ns in iter(namespaces.items()):
            name = args.namespace[0] + env["name"] + args.namespace[1]
            if name in ns:
                if re.match(r"^[A-Z0-9_]+$",sym): name = name.upper()
                replacements["\\b"+sym+"\\b"]=name+"_"+sym
    adjust_namespaces()
    
    def process_replacements(data1):
        for search,replace in iter(replacements.items()):
            data1 = re.sub(search,replace,data1)
        return data1
    
    ret =''
    prev = 0
    while True:
        per = re.search("\$for\s*\(\s*(\w+)\s+in\s*"+
                        "|\$set\s*\(\s*(\w+)\s*=\s*"+
                        "|\$define\s*\(\s*(\w+)\s*,\s*"+
                        "|\$namespace\(([\w\s,]+)\)"+
                        "|(\$\()"+
                        "|(\$dedup)"+
                        "|(\$include\s*\()"+
                        "|(\$process\s*\()"+
                        "|(\$if\s*\()"+
                        "|(//\$.*|/\*\$.*?\*/)",data[prev:])
        if per is None: break
        ret += process_replacements(data[prev:prev+per.start()])
        if per.group(1):
            iterator,prev = getparens(data,start=prev+per.end())
            prev = data.find("{",prev)+1
            work,prev = getparens(data,prev,"{","}")
            ev = eval(iterator,env)
            for obj in ev:
                env2 = env.copy()
                env2['_'] = env2[per.group(1)] = obj
                ret += process(work,env2,replacements=replacements)
        elif per.group(2): # set
            definition,prev = getparens(data,start=prev+per.end())
            ev = eval(definition,env)
            env[per.group(2)] = ev
            if (per.group(2)) == "name": adjust_namespaces()
        elif per.group(3): # define
            search=per.group(3)
            repl,prev = getparens(data,prev+per.end())
            repl = process(repl,env,replacements=replacements)
            replacements["\\b"+search+"\\b"]=repl
        elif per.group(4): # namespace
            prev+=per.end()
            loc = per.group(4)
            if loc not in namespaces: namespaces[loc] = set()
            name = args.namespace[0] + env["name"] + args.namespace[1]
            namespaces[loc].add(name)
            if re.match(r"^[A-Z0-9_]+$",loc): name = name.upper()
            replacements["\\b"+loc+"\\b"]=name+"_"+loc
            ret += name+"_"+loc
        elif per.group(5): # eval
            ev,prev = getparens(data,prev+per.end())
            ev = eval(ev,env)
            if ev is not None: ret += str(ev)
        elif per.group(6): # dedup
            prev = data.find("{",prev)+1
            work,prev = getparens(data,prev,"{","}")
            work = process(work,env,replacements=replacements)
            if work not in oncelers:
                oncelers.add(work)
                ret += work
        elif per.group(7): # include
            incl,prev = getparens(data,prev+per.end())
            incl = open(eval(incl),"r").read()
            ret += process(incl,env,replacements=replacements)
            adjust_namespaces()
        elif per.group(8): # process but don't include
            incl,prev = getparens(data,prev+per.end())
            incl = open(eval(incl),"r").read()
            process(incl,env,replacements=replacements)
            adjust_namespaces()
        elif per.group(9): # if
            cond,prev = getparens(data,start=prev+per.end())
            prev = data.find("{",prev)+1
            work,prev = getparens(data,prev,"{","}")
            ev = eval(cond,env)
            if ev:
                ret += process(work,env,replacements=replacements)
        else:
            prev += per.end()
            
    ret += process_replacements(data[prev:])
    if stripnnn: ret = re.sub(r"(\s*\n){3,}","\n\n",ret)
    return ret

input = open(args.files[0],"r").read()
data = process(input,stripnnn=True) + "\n"

if args.o == "-": print(data)
else:
    with open(args.o,"w") as f: f.write(data)

