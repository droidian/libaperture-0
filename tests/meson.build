test_env = [
  'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
  'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
  'G_DEBUG=fatal-warnings',
  'MALLOC_CHECK_=2',
]

test_executable = executable('aperture-tests',
  'test-main.c',
  dependencies: libaperture_dep
)

test('aperture-tests',
  test_executable,
  env: test_env,
)