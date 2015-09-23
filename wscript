#! /usr/bin/env python
# encoding: utf-8

VERSION='0.0.1'
APPNAME='sandbox'

top = '.'
out = 'build'
buildflags = [
  '-std=c++1y',
  '-Wall',
  '-Wextra',
  '-Wshadow',
  '-Wnon-virtual-dtor',
  '-Wold-style-cast',
  '-Wcast-align',
  '-Wunused',
  '-Woverloaded-virtual',
  '-pedantic',
  '-Werror',
  '-march=native'
]

def options(opt):
  opt.load('compiler_cxx')
  opt.load('waf_unit_test')

def configure(conf):
  conf.load('compiler_cxx')
  conf.load('waf_unit_test')
  conf.define('APPNAME', APPNAME)
  conf.define('VERSION', VERSION)
  conf.env.append_value('CXXFLAGS', buildflags)

from waflib.Tools import waf_unit_test
def build(bld):
  bld.env.append_value('INCLUDES', bld.out_dir)
  bld.env.append_value('INCLUDES', 'inc')
  bld.env.append_value('INCLUDES', 'src')
  bld.env.append_value('INCLUDES', 'test')
  bld.env.append_value('INCLUDES', bld.bldnode.abspath())
  src = bld.path.ant_glob(['test/**/*.cpp', 'src/**/*.cpp'])
  bld(
    source       = src,
    target       = APPNAME,
    features     = 'cxx cxxprogram test',

    includes     = bld.env.INCLUDES,
    defines      = bld.env.DEFINES,

    lib          = bld.env.LIB,
    libpath      = bld.env.LIBPATH,
    stlib        = [],
    stlibpath    = [],
    linkflags    = bld.env.LINKFLAGS,
    rpath        = [],
    vnum         = VERSION,
    use          = '',

    install_path = '${PREFIX}/bin',
    cflags       = [],
    cxxflags     = bld.env.CXXFLAGS,
    dflags       = [],
  )
  bld.add_post_fun(waf_unit_test.summary)
  bld.add_post_fun(waf_unit_test.set_exit_code)

from waflib.Build import BuildContext
class debug(BuildContext):
  cmd = 'debug'
  variant = 'debug'
  fun = 'debug'

def debug(bld):
  bld.env.append_value('CXXFLAGS', ['-g', '-O0'])
  bld.env.append_value('DEFINES', 'DEBUG')
  build(bld)

class release(BuildContext):
  cmd = 'release'
  variant = 'release'
  fun = 'release'

def release(bld):
  bld.env.append_value('CXXFLAGS', ['-O3'])
  bld.env.append_value('DEFINES', 'NDEBUG')
  build(bld)
