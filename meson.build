project('walld', 'c')
cc = meson.get_compiler('c')
src = ['src/main.c', 'src/config.c', 'src/magic.c', 'src/list.c', 'src/loop.c']
deps = [dependency('libconfig'), dependency('libevent'), dependency('MagickWand'), cc.find_library('m', required : false)]
executable('walld', src, dependencies: deps, install: true)
docs = ['docs/walld.1', 'docs/walldrc.5']
install_man(docs)