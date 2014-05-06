import sys
import os

env = Environment(ENV = {'TERM' : os.environ['TERM']})

env.Program('test_log', ['main.cpp', "logging.cpp", 'test.cpp'])
env.Library('libtnt.a', ['application_base.cpp', "logging.cpp"])
