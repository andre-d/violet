import os

env = Environment()
Export('env')

def build(name):
    SConscript('src/%s/SConscript' % name, variant_dir='build/%s/%s/' % (name, mode))

modes = ['release', 'debug']

if env.GetOption('clean'):
    for mode in modes:
        build('game')
        build('library')
else:
    mode = ARGUMENTS.get('mode', 'debug')
    
    if mode not in modes:
        print 'Invalid mode %s' % mode
        exit(1)
    
    env['ENV']['TERM'] = os.environ['TERM']
    env['CXX'] = 'clang++'
    env.Append(CPPPATH=['#include'])
    env.Append(CPPFLAGS=['-Wall', '-Wextra', '-Werror', '-std=c++03', '-pedantic'])
    
    if mode == 'debug':
        env.Append(CPPFLAGS=['-g', '-O0'])
    
    build('library')
    
    env.Append(LIBPATH = ['#build/library/%s' % mode])
    build('game')

