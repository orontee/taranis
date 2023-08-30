vcs_tag = ''
git = find_program('git', required : false)
if git.found()
  vcs_tag = run_command('git', 'rev-parse', '--short', 'HEAD').stdout().strip()
endif
if vcs_tag == ''
  vcs_tag = 'devel'
else
  vcs_tag = '@0@'.format(vcs_tag)
endif
conf = configuration_data()
conf.set('package_version', '@0@ (@1@)'.format(meson.project_version(), vcs_tag))

about_cc = configure_file(
  input: 'about.cc.in',
  output: 'about.cc',
  configuration: conf
)


sources = [about_cc] + files(
  'app.cc',
  'http.cc',
  'main.cc',
  'ui.cc',
  'util.cc'
)

