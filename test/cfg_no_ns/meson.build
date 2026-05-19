fixture_dir = meson.current_source_dir() / '../fixtures'

no_ns_conf = configuration_data()
no_ns_conf.set('LOG_INTERVAL_SECONDS', 2700)
no_ns_conf.set('MAX_LOG_ENTRIES', 10000)
no_ns_conf.set_quoted('PLATFORMDEVICEPREFIX', '')
no_ns_conf.set_quoted('PLATFORMSYSTEMID', 'system')
no_ns_conf.set_quoted('SHM_MAPPING_JSON', fixture_dir / 'mapping_no_namespaces.json')
no_ns_conf.set_quoted(
    'SHM_NAMESPACE_CFG_JSON',
    fixture_dir / 'namespace_invalid_entry.json',
)
no_ns_conf.set('libonly', false)
configure_file(output: 'config.h', configuration: no_ns_conf)
