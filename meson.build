project('taranis', 'cpp',
        version: '0.1.0',
        meson_version: '>= 1.0.1',
        default_options : [
          'cpp_std=c++17',
        ])


configure_file(
  input: 'crossfile_arm.ini.in',
  output: 'crossfile_arm-v2.ini',
  configuration: {'pwd': meson.global_build_root()}
)


compiler = meson.get_compiler('cpp')

taranis_deps = [
  compiler.find_library('inkview', required: true),
  compiler.find_library('curl', required: true),
  compiler.find_library('jsoncpp', required: true),
  compiler.find_library('gsl', required: true, static: true),
  compiler.find_library('gslcblas', required: true, static: true),
  compiler.find_library('m', required: true, static: true),
]
executable('taranis.app', ['src/main.cc', 'src/http.cc'],
           dependencies : taranis_deps)
