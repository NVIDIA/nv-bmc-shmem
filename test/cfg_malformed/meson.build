fixture_dir = meson.current_source_dir() / '../fixtures'
malformed_path = fixture_dir / 'malformed.json'

malformed_conf = configuration_data()
malformed_conf.set('LOG_INTERVAL_SECONDS', 2700)
malformed_conf.set('MAX_LOG_ENTRIES', 10000)
malformed_conf.set_quoted('PLATFORMDEVICEPREFIX', '')
malformed_conf.set_quoted('PLATFORMSYSTEMID', 'system')
malformed_conf.set_quoted('SHM_MAPPING_JSON', malformed_path)
malformed_conf.set_quoted('SHM_NAMESPACE_CFG_JSON', malformed_path)
malformed_conf.set('libonly', false)
configure_file(output: 'config.h', configuration: malformed_conf)
