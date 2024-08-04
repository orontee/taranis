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

icons_resources_c = custom_target(
  'generate-icons-resources-c',
  output: 'icon_resources.c',
  input: icons,
  command: [pbres_command, '-c', '@OUTPUT@', '-8', '@INPUT@']
)

convert_resources_command = find_program('../scripts/convert_ressources.sh',
                                         required: true)

icon_resources_cc = custom_target(
  'convert-resources-cc',
  output: 'icon_resources.cc',
  input: icons_resources_c,
  depends: [icons_resources_c],
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

sources = [about_cc, icon_resources_cc, l10n_cc] + files(
  'alerts.cc',
  'app.cc',
  'config.cc',
  'convert.cc',
  'currentconditionbox.cc',
  'dailyforecastbox.cc',
  'dailyforecastviewer.cc',
  'events.cc',
  'fonts.cc',
  'forecast.cc',
  'hourlyforecastbox.cc',
  'http.cc',
  'icons.cc',
  'locationbox.cc',
  'locationselector.cc',
  'logging.cc',
  'logo.cc',
  'main.cc',
  'menu.cc',
  'pager.cc',
  'service.cc',
  'stack.cc',
  'state.cc',
  'statusbar.cc',
  'ui.cc',
  'util.cc',
  'version.cc'
)
