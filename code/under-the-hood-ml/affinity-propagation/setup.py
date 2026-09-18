import numpy

from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [
    Extension(
        name="affinity_propagation",
        sources=["affinity_propagation.pyx"],
        include_dirs=[numpy.get_include()],
        define_macros=[("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")],
    )
]

setup(
    name="affinity_propagation",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)