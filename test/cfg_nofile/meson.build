nofile_conf = configuration_data()
nofile_conf.set('LOG_INTERVAL_SECONDS', 2700)
nofile_conf.set('MAX_LOG_ENTRIES', 10000)
nofile_conf.set_quoted('PLATFORMDEVICEPREFIX', '')
nofile_conf.set_quoted('PLATFORMSYSTEMID', 'system')
nofile_conf.set_quoted(
    'SHM_MAPPING_JSON',
    '/nonexistent_nvshmem_test/mapping.json',
)
nofile_conf.set_quoted(
    'SHM_NAMESPACE_CFG_JSON',
    '/nonexistent_nvshmem_test/namespace.json',
)
nofile_conf.set('libonly', false)
configure_file(output: 'config.h', configuration: nofile_conf)
