import os 
import sys

OMP_THREADS_ENV = 'OMP_NUM_THREADS'

if len(sys.argv) < 2:
    print('No outfile')
    quit()

out_file = sys.argv[1]
cli_app = ''

if len(sys.argv) >= 3:
    cli_app = sys.argv[2] if sys.argv[2] != '-t' else ''

test_en = False
for arg in sys.argv:
    if arg == '-t':
        test_en = True

graphs = [
    'as-skitter',
    'orkut',
    'higgs',
    'livejournal',
    'pokec',
    'stackoverflow',
]

graph_prefix = {
    'bfs': '',
    'sssp': 'w_',
    'pr': ''
}

start_vertex = {
    'bfs' : {
        'as-skitter': {878248, 1093773, 1040066, 1529161, 1105468, 1543502},
        'orkut': {2062367, 767779, 1805450, 1060076, 424425, 641114},
        'higgs': {165486, 15147, 288568, 17220, 127341, 328483},
        'livejournal': {3903641, 4158378, 1486101, 467386, 1875102, 1966836},
        'pokec': {858951, 438160, 1385063, 793905, 310461, 300989},
        'stackoverflow': {5515818, 3554183, 2622510, 200094, 1323299, 1166567},
    },
    'sssp' : {
        'as-skitter': {1687785, 774220, 1672775, 324212, 411760, 554802},
        'orkut': {376633, 2503156, 1941442, 742190, 1461468, 2082824},
        'higgs': {132279, 206656, 418509, 197172, 213192, 11003},
        'livejournal': {2885369, 669594, 848203, 679576, 2735354, 2016861},
        'pokec': {247882, 246033, 1092414, 720811, 255164, 139421},
        'stackoverflow': {3214351, 2446213, 3198986, 1313602, 32228, 824045},
    },
    'pr' : {
        'as-skitter': {878248, 1093773},
        'orkut': {2062367, 767779},
        'higgs': {165486, 15147},
        'livejournal': {3903641, 4158378},
        'pokec': {858951, 438160},
        'stackoverflow': {5515818, 3554183},
    }
}

app_command = {
    'bfs': 'bin/BFS',
    'sssp': 'bin/SSSP',
    'pr': 'bin/PageRank'
}

def runCommand(command):
    if test_en:
        print(command)
    else:
        os.system(command)

def collectGraphs(app, threads, optionalArgs = []):

    for threadcount in threads:
        # Set OMP numthreads
        os.environ[OMP_THREADS_ENV] = str(threadcount)

        for graph in graphs:
            optional = ' '.join(optionalArgs)
            graphfile = 'graphs/' + graph_prefix[app] + graph + '.g'

            start_vtx = []
            if graph in start_vertex[app]:
                start_vtx = start_vertex[app][graph]

            for start_v in start_vtx:
                start_v = start_v + 1 # Need this because GraphMat edges start at 1 instead of 0

                for i in range(5): # Doing each start vertex 5 times

                    print(f'Running {app} with {threadcount} thread(s) on {graphfile} with start {start_v}...')

                    runCommand(f'{app_command[app]} {graphfile} {start_v} > tmp_out')

                    if app == 'pr':
                        runCommand(f'awk \'$1 == "Completed" {{print $2}}\' tmp_out | tail -n 1 > tmp_out2')
                        runCommand(f'echo "," >> tmp_out2')
                        runCommand(f'awk \'$1 == "PR" {{print $4}}\' tmp_out >> tmp_out2')
                        runCommand(f'tr -d \'\\n\' < tmp_out2 > tmp_out3')
                        runCommand(f'echo "" >> tmp_out3')
                        runCommand(f'echo -n "{app},{threadcount},{graph}," | cat - tmp_out3 >> {out_file}')

                    else:
                        runCommand(f'awk \'$1 == "Time" {{print $3}}\' tmp_out > tmp_out2')
                        runCommand(f'echo -n "{app},{threadcount},{graph},1," | cat - tmp_out2 >> {out_file}')

#threads = [4, 8, 14]
threads = [14]
opt_args = {
    'bfs': [],
    'sssp': [],
    'pr': ['-i 1']
}

apps = ['bfs', 'sssp', 'pr']

if cli_app == '':
    for app in apps:
        collectGraphs(app, threads, opt_args[app])
else:
    collectGraphs(cli_app, threads, opt_args[app])