import numpy

from setuptools import setup, Extension
from Cython.Build import cythonize

extension_modules = [
    Extension(
        name="local_outlier_factor",
        sources=["local_outlier_factor.pyx"],
        include_dirs=[
            numpy.get_include()
        ],
        define_macros=[
            ("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")
        ],
    )
]

setup(
    name="local_outlier_factor",
    ext_modules=cythonize(
        extension_modules,
        compiler_directives={
            "language_level": "3",
            "boundscheck": False,
            "initializedcheck": False,
            "cdivision": True,
        },
    ),
)
