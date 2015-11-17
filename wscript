#! /usr/bin/env python
# encoding: utf-8

VERSION='0.0.1'
APPNAME='sandbox'

top = '.'
out = 'build'
flags = [
  '-std=c++14',
#  '-Wall',
#  '-Wextra',
#  '-Wshadow',
#  '-Wconversion',
#  '-Wnon-virtual-dtor',
#  '-Wold-style-cast',
#  '-Wcast-align',
#  '-Wunused',
#  '-Woverloaded-virtual',
#  '-pedantic',
#  '-pedantic-errors',
#  '-Werror'
]

def options(opt):
  opt.load('compiler_cxx')
  opt.load('waf_unit_test')

def configure(conf):
  conf.load('waf_unit_test')
  conf.define('APPNAME', APPNAME)
  conf.define('VERSION', VERSION)

  conf.setenv('debug')
  conf.load('compiler_cxx')
  conf.env.CXXFLAGS += flags
  conf.env.CXXFLAGS += ['-g', '-O0']
  conf.env.DEFINES += ['DEBUG']

  conf.setenv('release')
  conf.load('compiler_cxx')
  conf.env.CXXFLAGS += flags
  #conf.env.CXXFLAGS += ['-O3', '-march=native', '-fPIC', '-fno-rtti']
  #conf.env.LINKFLAGS += ['-flto']
  conf.env.DEFINES += ['NDEBUG']

from waflib.Tools import waf_unit_test
def build(bld):
  if not bld.variant:
    bld.fatal('try "waf --help"')
  bld.env.INCLUDES += ['inc', 'src']
  bld(
    source       = bld.path.ant_glob(['src/**/*.cpp']),
    target       = APPNAME,
    features     = 'cxx cxxstlib',
    vnum         = VERSION,
  )
  bld.env.INCLUDES += ['test']
  bld(
    source       = bld.path.ant_glob(['test/**/*.cpp']),
    target       = 'ut_' + APPNAME,
    features     = 'cxx cxxprogram test',
    use          = APPNAME
  )
  bld.add_post_fun(waf_unit_test.summary)
  bld.add_post_fun(waf_unit_test.set_exit_code)

from waflib.Build import BuildContext
from waflib.Build import CleanContext
from waflib.Build import InstallContext
from waflib.Build import UninstallContext

for ctx in (BuildContext, CleanContext, InstallContext, UninstallContext):
  name = ctx.__name__.replace('Context','').lower()
  class debug(ctx):
    cmd = name + '_debug'
    variant = 'debug'
  class release(ctx):
    cmd = name
    variant = 'release'
