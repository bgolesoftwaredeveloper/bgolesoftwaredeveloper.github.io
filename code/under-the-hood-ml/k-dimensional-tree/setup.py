import numpy

from setuptools import setup, Extension
from Cython.Build import cythonize

extension_modules = [
    Extension(
        name="kd_tree",
        sources=["kd_tree.pyx"],
        include_dirs=[
            numpy.get_include()
        ],
        define_macros=[
            ("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")
        ],
    )
]

setup(
    name="kd_tree",
    ext_modules=cythonize(
        extension_modules,
        compiler_directives={
            "language_level": "3",
            "boundscheck": False,
            "wraparound": False,
            "cdivision": True,
        },
    ),
)
