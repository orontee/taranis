project(
  'taranis',
  'cpp',
  version: '1.2.0-pre',
  meson_version: '>= 0.61.2',
  default_options : [
    'cpp_std=c++17',
  ]
)

pbres_command = find_program(
  './SDK_6.3.0/SDK-B288/usr/bin/pbres',
  required: true
)

sed_command = find_program('sed')

subdir('icons')

icons_c = custom_target(
  'generate-icons-resources-c',
  output: 'icons.c',
  input: icons,
  command: [pbres_command, '-c', '@OUTPUT@', '-8', '@INPUT@']
)

icons_cc = custom_target(
  'convert-resources-cc',
  output: 'icons.cc',
  input: icons_c,
  command: ['./scripts/convert_ressources.sh', '@INPUT@', '@OUTPUT@']
)

compiler = meson.get_compiler('cpp')

taranis_deps = [
  compiler.find_library('inkview', required: true),
  compiler.find_library('boost_system', required: true, static: true),
  compiler.find_library('boost_filesystem', required: true, static: true),
  compiler.find_library('boost_date_time', required: true, static: true),
  compiler.find_library('boost_log', required: true, static: true),
  compiler.find_library('boost_log_setup', required: true, static: true),
  compiler.find_library('boost_thread', required: true, static: true),
  compiler.find_library('curl', required: true),
  compiler.find_library('jsoncpp', required: true),
  compiler.find_library('gsl', required: true, static: true),
  compiler.find_library('gslcblas', required: true, static: true),
  compiler.find_library('m', required: true, static: true),
  compiler.find_library('pthread', required: true),
]

subdir('src')

sources += [icons_cc]

executable(
  'taranis.app',
  sources,
  dependencies: taranis_deps
)
