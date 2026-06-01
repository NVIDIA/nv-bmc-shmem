fixture_dir = meson.current_source_dir() / '../fixtures'

no_prod_conf = configuration_data()
no_prod_conf.set('LOG_INTERVAL_SECONDS', 2700)
no_prod_conf.set('MAX_LOG_ENTRIES', 10000)
no_prod_conf.set_quoted('PLATFORMDEVICEPREFIX', '')
no_prod_conf.set_quoted('PLATFORMSYSTEMID', 'system')
no_prod_conf.set_quoted(
    'SHM_MAPPING_JSON',
    fixture_dir / 'mapping_no_producers.json',
)
no_prod_conf.set_quoted(
    'SHM_NAMESPACE_CFG_JSON',
    fixture_dir / 'mapping_no_producers.json',
)
no_prod_conf.set('libonly', false)
configure_file(output: 'config.h', configuration: no_prod_conf)
