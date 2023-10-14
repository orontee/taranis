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
  'alerts.cc',
  'app.cc',
  'config.cc',
  'convert.cc',
  'currentconditionbox.cc',
  'events.cc',
  'http.cc',
  'locationbox.cc',
  'locationlist.cc',
  'logging.cc',
  'main.cc',
  'menu.cc',
  'service.cc',
  'state.cc',
  'ui.cc',
  'util.cc'
)

subdir('l10n')
