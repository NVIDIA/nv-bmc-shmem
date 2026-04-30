gtest_dep = dependency('gtest', main: true, disabler: true, required: false)
gmock_dep = dependency('gmock', disabler: true, required: false)
if not gtest_dep.found() or not gmock_dep.found()
    gtest_proj = import('cmake').subproject('googletest', required: false)
    if gtest_proj.found()
        gtest_dep = declare_dependency(
            dependencies: [
                dependency('threads'),
                gtest_proj.dependency('gtest'),
                gtest_proj.dependency('gtest_main'),
            ],
        )
        gmock_dep = gtest_proj.dependency('gmock')
    else
        assert(
            not get_option('tests').enabled(),
            'Googletest is required if tests are enabled',
        )
    endif
endif


tests = [
    'sensor_map_test',
    'shmem_map_extra_test',
    'time_utils_test',
    'error_logger_test',
    'port_utils_test',
    'config_json_reader_test',
    'config_json_reader_null_test',
    'shm_sensormap_intf_test',
    'telemetry_mrd_producer_test',
    'telemetry_mrd_client_test',
]
foreach t : tests
    test(
        'test_' + t.underscorify(),
        executable(
            'test-' + t.underscorify(),
            t + '.cpp',
            dependencies: [gmock_dep, gtest_dep, conf_h_dep, nv_shmem_dep],
            include_directories: include_directories('..'),
        ),
    )
endforeach

# metric_report_utils_test needs the stub shm_common.h (in test/stubs/) placed
# first in the include path so it shadows <sdbusplus/bus.hpp> and avoids the
# lg2/sdbusplus log_convert ambiguity.
test(
    'test_metric_report_utils_test',
    executable(
        'test-metric_report_utils_test',
        'metric_report_utils_test.cpp',
        dependencies: [gmock_dep, gtest_dep, conf_h_dep, nv_shmem_dep],
        include_directories: [
            include_directories('stubs'),
            include_directories('..'),
        ],
    ),
)
