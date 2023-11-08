# Generate sources for about dialog
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

# Generate source with icons
pbres_command = find_program(
  '../SDK_6.3.0/SDK-B288/usr/bin/pbres',
  required: true
)

sed_command = find_program('sed', required: true)
# dependency of convert_resources_command

icons_c = custom_target(
  'generate-icons-resources-c',
  output: 'icons.c',
  input: icons,
  command: [pbres_command, '-c', '@OUTPUT@', '-8', '@INPUT@']
)

convert_resources_command = find_program('../scripts/convert_ressources.sh',
                                         required: true)

icons_cc = custom_target(
  'convert-resources-cc',
  output: 'icons.cc',
  input: icons_c,
  depends: [icons_c],
  command: [convert_resources_command, '@INPUT@', '@OUTPUT@']
)

# Generate source with translations
generate_l10n_cc_command = find_program('../scripts/generate_l10n_cc.py',
                                        required: true)

l10n_cc = custom_target(
  'generate-l10n-cc',
  output: 'l10n.cc',
  input: 'l10n.cc.in',
  command: [generate_l10n_cc_command,
            '--output', '@OUTPUT@',
            '--template', '@INPUT@',
            '@SOURCE_ROOT@/po']
)

sources = [about_cc, icons_cc, l10n_cc] + files(
  'alerts.cc',
  'app.cc',
  'config.cc',
  'convert.cc',
  'events.cc',
  'hourlyforecastbox.cc',
  'http.cc',
  'locationbox.cc',
  'locationselector.cc',
  'logging.cc',
  'main.cc',
  'menu.cc',
  'service.cc',
  'state.cc',
  'ui.cc',
  'util.cc'
)
