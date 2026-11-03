{
  checkout_options: { 'fetch-depth': 0 },
  configure_command: 'cmake -S . -B build-scan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER_TARGET=x86_64-w64-windows-gnu',
  documentation_steps: [
    {
      name: 'Install dependencies',
      run: 'sudo apt-get update\nsudo apt-get install -y doxygen graphviz\n',
    },
    {
      name: 'Configure',
      run: 'cmake -DCMAKE_BUILD_TYPE=Release -B "$GITHUB_WORKSPACE/build" --install-prefix "$GITHUB_WORKSPACE/build/prefix" -DBUILD_DOCS=ON',
    },
    {
      name: 'Build documentation',
      run: 'cmake --build "$GITHUB_WORKSPACE/build" --verbose --config Release --target docs',
    },
  ],
  env: {},
  setup_steps: [
    {
      name: 'Install build dependencies',
      run: 'sudo apt-get update\nsudo apt-get install -y gcc-mingw-w64-x86-64\n',
    },
    {
      name: 'Install current CMake',
      run: 'pipx install cmake',
    },
    {
      name: 'Locate MinGW runtime libraries',
      run: 'echo "LDFLAGS=-L$(dirname "$(x86_64-w64-mingw32-gcc -print-libgcc-file-name)")" >> "$GITHUB_ENV"',
    },
  ],
  workflow_file: 'pages.yml',
}
