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
    #'as-skitter',
    #'orkut',
    #'higgs',
    'livejournal',
    #'pokec',
    #'stackoverflow',
]

graph_prefix = {
    'bfs': '',
    'sssp': 'w_',
    'pr': ''
}

start_vertex = {
    'bfs' : {
        'as-skitter': {878248},
        'orkut': {2062367},
        'higgs': {165486,}
        'livejournal': {3903641},
        'pokec': {858951},
        'stackoverflow': {5515818},
    },
    'sssp' : {
        'as-skitter': {1538117},
        'orkut': {2503157},
        'higgs': {132280},
        'livejournal': {2885370, 669595, 848204, 679577, 2735355, 2016862},
        'pokec': {247883},
        'stackoverflow': {3214352},
    },
    'pr' : {}
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

    for threacount in threads:
        # Set OMP numthreads
        os.environ[OMP_THREADS_ENV] = str(threacount)

        for graph in graphs:
            optional = ' '.join(optionalArgs)
            graphfile = 'graphs/' + graph_prefix[app] + graph + '.g'


            start_vtx = []
            if graph in start_vertex[app]:
                start_vtx = start_vertex[app][graph]

            for start in start_vtx:

            #for i in range(5):

                print(f'Running {app} with {threacount} thread(s) on {graphfile} with start {start_vtx}...')

                runCommand(f'{app_command[app]} {graphfile} {start_vtx} > tmp_out')

                if app == 'pr':
                    runCommand(f'awk \'$1 == "Completed" {{print $2}}\' tmp_out | tail -n 1 > tmp_out2')
                    runCommand(f'echo "," >> tmp_out2')
                    runCommand(f'awk \'$1 == "PR" {{print $4}}\' tmp_out >> tmp_out2')
                    runCommand(f'tr -d \'\\n\' < tmp_out2 > tmp_out3')
                    runCommand(f'echo "" >> tmp_out3')
                    runCommand(f'echo -n "{app},{threacount},{graph}," | cat - tmp_out3 >> {out_file}')

                else:
                    runCommand(f'awk \'$1 == "Time" {{print $3}}\' tmp_out > tmp_out2')
                    runCommand(f'echo -n "{app},{threacount},{graph},1," | cat - tmp_out2 >> {out_file}')

#threads = [4, 8, 14]
threads = [14]
opt_args = {
    'bfs': [],
    'sssp': [],
    'pr': ['-i 1']
}

#apps = ['bfs', 'sssp', 'pr']
apps = ['bfs', 'sssp']

if cli_app == '':
    for app in apps:
        collectGraphs(app, threads, opt_args[app])
else:
    collectGraphs(cli_app, threads, opt_args[app])