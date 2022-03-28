import sys
import json


configs_dir = './resources/config'


CPU = 16


replacements = {
    '@CL' : str(CPU),     # The global CPU limit
    '@NA' : '4',      # Number of algorithms in cartesian search
    '@AC' : None,   # The algorithm config
    '@SSC' : None,  # Solver service configuration
    '@RSC' : None,  # RB search configuration
    '@NS' : None,   # The number of solvers in service
    # @MT = @CL / @NS = Each solvers' maximum number of threads
}


alg_opts = [
    ('ea', 'alg_ea'),
    ('ga', 'alg_ga'),
]


rbs_opts = [
    ('cart', 'rbs_cart'),
    ('rbs', 'rbs_one'),
]


reduce_opts = [
    ('rec', 'reduce_recur'),
    ('sing', 'reduce_single'),
]


solver_opts = [
    (f'p{CPU}', 'solver_pain', 1),
    (f'p{int(CPU // 4)}', 'solver_pain', 4),
    ('s1', 'solver_simp', CPU),
]


def get_name(reduce, rbs, alg, solver, num_solvers):
    return f'{CPU}_{reduce}_{rbs}_{alg}_{solver}-{num_solvers}'


def save_config(js, file_name):
    with open(f'{configs_dir}/{file_name}.json', 'w') as f:
        f.write(json.dumps(js, indent=4))


def get_content(file):
    with open(f'./scripts/parts/{file}.tjson') as f:
        return f.read()


def perform_replacement(text):
    changed = True
    while changed:
        changed = False
        old = text
        for key, value in replacements.items():
            text = text.replace(key, value)
        changed = text != old
    return text


def main():
    for sn, slv, ns in solver_opts:
        replacements['@SSC'] = get_content(slv)
        replacements['@NS'] = str(ns)
        replacements['@MT'] = str(int(CPU // ns))
        for an, alg in alg_opts:
            replacements['@AC'] = get_content(alg)
            for rn, rbs in rbs_opts:
                replacements['@RSC'] = get_content(rbs)
                for rdn, reduce in reduce_opts:
                    save_config(
                        js=json.loads(perform_replacement(get_content(reduce))),
                        file_name=get_name(
                            reduce=rdn,
                            rbs=rn,
                            alg=an,
                            solver=sn,
                            num_solvers=ns,
                        ),
                    )


if __name__ == '__main__':
    main()
