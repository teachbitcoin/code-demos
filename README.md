# Teach Bitcoin with Libbitcoin and Jupyter Notebook.

The code examples and exercises are written in jupyter notebook pages or native C++.

There are two types of jupyter notebook pages in this repository. Pages denoted with `BX` are command-line scripts which call the Libbitcoin-Explorer command-line utility and require a jupyter bash kernel to run. Pages denoted with `cpp` are written with Libbitcoin C++ libraries and require a jupyter C++ kernel to run.

Here are the required tools for the TeachBitcoin course.

**Libbitcoin-Explorer**
* Install with `install.sh` (Version3)
  * [Linux](https://github.com/libbitcoin/libbitcoin-explorer/blob/master/README.md#debianubuntu)
  * [Mac](https://github.com/libbitcoin/libbitcoin-explorer/blob/master/README.md#macintosh)
  * (The install script will install all required Libbitcoin C++ libraries required for the course.)
* Configure Libbitcoin-Explorer for testnet
  * [Instructions](https://github.com/libbitcoin/libbitcoin-explorer/wiki/Configuration-Settings)
  * [Example Testnet Config](https://gist.github.com/jachiang/fb5ff9ced998affd92ee58a580d3e052)
  * Try `bx fetch-height`, and compare with a testnet blockexplorer.

**Jupyter Notebook and Bash & C++ Kernels**

It is recommended to install jupyter notebook from a mini-conda package installer.

* [Install](https://conda.io/miniconda.html) the miniconda package-manager.
* [Install](https://github.com/QuantStack/xeus-cling/blob/master/README.md) the Jupyter Notebook and the c++ cling kernel.
* [Install](https://github.com/takluyver/bash_kernel/blob/master/README.rst) the Jupyter Bash Kernel.
* Try to run the notebook pages in this repository

**C++ tools**

* IDE of your choice.
* Or, any text editor with GCC linter
  * e.g. Atom & [linter-gcc](https://atom.io/packages/linter-gcc)
