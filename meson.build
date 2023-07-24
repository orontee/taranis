project('taranis', 'cpp',
        version: '0.1.0',
        meson_version: '>= 1.0.1',
        default_options : [
          'cpp_std=c++17',
        ])

compiler = meson.get_compiler('cpp')
inkview_dep = [
  compiler.find_library('inkview', required : true),
  compiler.find_library('curl', required : true),
  compiler.find_library('jsoncpp', required : true),
]
executable('taranis.app', ['src/main.cc', 'src/http.cc'],
           dependencies : inkview_dep)
