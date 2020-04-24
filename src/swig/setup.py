#!/usr/bin/env python

from distutils.core import setup, Extension


SimpleMusicGen_module = Extension('_SimpleMusicGen',
                            sources=[r"SimpleMusicGen_wrap.cpp", "AudioAndMusic.cpp", "Drums.cpp", "Generators.cpp", "UtilFunctions.cpp"],
                            include_dirs=[r"C:\Users\MJ12\source\SimpleMusicGenerator\win\SDL2\include"],
                            library_dirs=[r"C:\Users\MJ12\source\SimpleMusicGenerator\win\SDL2\lib\x64"],
                            libraries=['SDL2']
)

setup (name = 'SimpleMusicGen',
       version = '0.1',
       author      = "MR",
       description = """Simple music gen""",
       ext_modules = [SimpleMusicGen_module],
       py_modules = ["SimpleMusicGen"],
       )


# Extension('foo', ['foo.c'], include_dirs=['/usr/include/X11'])
#          library_dirs=['/usr/X11R6/lib'],
#   libraries=['X11', 'Xt'])