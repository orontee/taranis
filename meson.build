project(
  'taranis',
  'cpp',
  version: '1.8.0-pre',
  meson_version: '>= 0.61.2',
  default_options : [
    'cpp_std=c++17',
  ]
)

compiler = meson.get_compiler('cpp')

taranis_deps = [
  compiler.find_library('inkview', required: true),
  dependency('boost',
             modules: ['filesystem', 'date_time', 'log',
                       'log_setup', 'system', 'thread'],
             required: true, static: true),
  dependency('libcurl', required: true),
  dependency('jsoncpp', required: true),
  dependency('gsl', required: true, static: true),
  compiler.find_library('gslcblas', required: true, static: true),
  compiler.find_library('m', required: true, static: true),
  dependency('opencv', required: true),
]

i18n = import('i18n')

subdir('icons')

subdir('po')

subdir('src')

subdir('packaging')

executable(
  'taranis.app',
  sources,
  dependencies: taranis_deps,
  install: true,
  install_dir: '/sinstall',
)
