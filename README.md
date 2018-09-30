# Teach Bitcoin with Libbitcoin and Jupyter Notebook.

The code examples and exercises are written and executed in jupyter notebook pages. This makes them easier to demonstrate and edit in a developer training environment.

There are two types of notebook pages in this repository. Pages denoted with `BX` are command-line scripts which call the Libbitcoin-Explorer command-line utility and require a jupyter bash kernel to run. Pages denoted with `cpp` are written with Libbitcoin c++ libraries and require a jupyter c++ kernel to run.


You therefore need to install both Libbitcoin and Jupyter notebook kernels to follow the notebook pages in this repository.

**Installing the Libbitcoin Toolkit:**
* [Install](https://github.com/libbitcoin/libbitcoin-explorer/wiki/Download-BX) Libbitcoin-explorer (`BX` pages)
* [Install](https://github.com/libbitcoin/libbitcoin/blob/version2/README.md) Libbitcoin-system (`cpp` pages)
* [Install](https://github.com/libbitcoin/libbitcoin-protocol/blob/master/README.md) Libbitcoin-protocol (`cpp` pages with zmq connections)

<br>

**Installing Jupyter Notebook and Kernels**

It is recommended to install jupyter notebook from a mini-conda package installer.

* [Install](https://conda.io/miniconda.html) the miniconda package-manager.
* [Install](https://github.com/QuantStack/xeus-cling/blob/master/README.md) the Jupyter Notebook and the c++ cling kernel.
* [Install](https://github.com/takluyver/bash_kernel/blob/master/README.rst) the Jupyter Bash Kernel.
