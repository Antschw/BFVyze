from setuptools import setup, find_packages

setup(
    name="BFVyze_py",
    version="0.1.0",
    description="Python backends for overlay BFV: OCR processing and ZeroMQ communication.",
    author="Antschw",
    author_email="antoine97410@hotmail.fr",
    url="https://github.com/Antschw/BFVyze",
    packages=find_packages(),
    install_requires=[
        "pyzmq>=22.0.0",
        "opencv-python>=4.5.0",
        "numpy>=1.21.0",
        "pytesseract>=0.3.8",
        "requests>=2.26.0"
    ],
    entry_points={
        "console_scripts": [
            "BFVyze_py=main:main"
        ]
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.7',
)
