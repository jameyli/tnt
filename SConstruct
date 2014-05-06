import sys
import os

env = Environment()


env = Environment(ENV = {'PATH' : os.environ['PATH'],
                        'TERM' : os.environ['TERM'],
                        'HOME' : os.environ['HOME']})

Export('env')
Program('test_log', ['main.cpp', "logging.cpp", 'test.cpp'])
Library('libtnt.a', ['application_base.cpp'])
