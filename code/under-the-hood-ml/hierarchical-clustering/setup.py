import numpy

from setuptools import setup, Extension
from Cython.Build import cythonize

compiler_directives = {
    "language_level": "3",
    "boundscheck": False,
    "wraparound": False,
    "cdivision": True
}

extensions = [
    Extension(
        name="distance",
        sources=["distance.pyx"],
        include_dirs=[numpy.get_include()],
        define_macros=[("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")],
    ),

    Extension(
        name="hierarchical_clustering",
        sources=["hierarchical_clustering.pyx"],
        include_dirs=[numpy.get_include()],
        define_macros=[("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")],
    )
]

setup(
    name="hierarchical_clustering",
    ext_modules=cythonize(extensions, compiler_directives=compiler_directives),
)