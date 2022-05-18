import sys
import json
import re
import time


"""
This script is made to make configuration generation simple
"""


configs_dir = './resources/config/composite'
replacements = dict() 


alg_opts = [
    ('ea', 'alg_ea'),
    ('ga', 'alg_ga'),
]


# NA (number of algorithms), AC (algorithm config)
rbs_opts = [
    ('cart', 'rbs_cart'),
    ('rbs', 'rbs_one'),
]


# CL (cpu limit), SSC (solver service config), RSC (rb search config)
reduce_opts = [
    ('rec', 'reduce_recur'),
    ('sing', 'reduce_single'),
]


concur_to_ns_mt = {
    32 : (5, 6),
    16 : (3, 5),
    21 : (4, 5),
    11 : (2, 5),
}


def get_ns_mt(solver, concur, one_solver):
    if solver == 'p':
        if one_solver:
            return 1, concur - 1
        else:
            return concur_to_ns_mt[concur]
    elif solver == 'ps':
        assert(not one_solver)
        return concur_to_ns_mt[concur]
    elif solver == 'ms':
        assert(not one_solver)
        return concur - 1, 1
    elif solver == 's' or solver == 'm':
        assert(not one_solver)
        return concur, 1
    assert(False)


# NS (num solvers), MT (max threads in painless solver)
service_opts = [
    # (name, file, one_solver). name will be CONCUR / NS
    ('p', 'solver_pain_ns', True),
    ('p', 'solver_pain_ns', False),
    ('ps', 'solver_pain_s', False),
    ('s', 'solver_simp', False),
    ('m', 'solver_maple_ns', False),
    ('ms', 'solver_maple_s', False),
]


# WPCL (with painless cpu limit), NCL (painless cpu limit), NSOC (near solve config)
solve_opts = [
    # (name, file, { CONCUR -> (NCL, sharing_threads) })
    ('wp', 'with_painless', { 16 : (4, 1), 32 : (10, 1) }),
    ('wop', 'without_painless', { 16 : (0, 0), 32 : (0, 0) }),
]


def save_config(js, file_name):
    with open(f'{configs_dir}/{file_name}.json', 'w') as f:
        f.write(json.dumps(js, indent=4))


def get_content(file):
    with open(f'./scripts/parts/{file}.tjson') as f:
        return f.read()


def perform_replacement(text):
    while True:
        m = re.search('(@[A-Z]+)', text)
        if m:
            key = m.group(1)
            text = text.replace(key, replacements[key])
        else:
            break
    return text


def set(k, v):
    replacements[k] = v


def do_alg(concur, pref):
    for name, file in alg_opts:
        set('@AC', get_content(file))
        set('@NA', '4')
        content = perform_replacement('@ROOT')
        save_config(js=json.loads(content), file_name=f'{pref}_{name}')


def do_rbs(concur, pref):
    for name, file in rbs_opts:
        set('@RSC', get_content(file))
        do_alg(concur, f'{pref}_{name}')


def do_reduce(concur, pref):
    for name, file in reduce_opts:
        set('@CL', str(concur))
        set('@NSOC', get_content(file))
        do_rbs(concur, f'{pref}_{name}')


def do_service(concur, pref):
    for name, file, one_solver in service_opts:
        ns, mt = get_ns_mt(name, concur, one_solver)
        set('@SSC', get_content(file))
        set('@NS', str(ns))
        set('@MT', str(mt))
        do_reduce(concur, f'{pref}_{name}{mt}-{ns}')


def do_solve(concur):
    for name, file, concur_to_ncl in solve_opts:
        x, sh_th = concur_to_ncl[concur]
        set('@ROOT', get_content(file))
        set('@WPCL', str(concur))
        set('@NCL', str(x))
        do_service(concur - x - sh_th, f'{concur}_{name}')


if __name__ == '__main__':
    do_solve(16) 
    do_solve(32)
