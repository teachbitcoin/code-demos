# Teach Bitcoin with Libbitcoin and Jupyter Notebook.

The code examples and exercises are written in jupyter notebook pages.

It is highly recommended to install [Vagrant](https://www.vagrantup.com/downloads.html), and then run the code repository directly from the VM instance managed by Vagrant. To do so, go to the `code-demos` repository, and `vagrant up` to automatically setup an Ubuntu instance with Libbitcoin BX and jupyter notebook. To access the VM instance, simply `vagrant ssh` afterwards. Please run `jupyter notebook --ip 0.0.0.0` so the jupyter notebook server port can be forwarded to your host machine.

To run the cpp examples in this repository, you will need an alternative setup. To do so, please follow the steps below.

**Libbitcoin-Explorer**
* Install with `install.sh` (Version3)
  * [Linux](https://github.com/libbitcoin/libbitcoin-explorer/blob/master/README.md#debianubuntu)
  * [Mac](https://github.com/libbitcoin/libbitcoin-explorer/blob/master/README.md#macintosh)
  * (The install script will install all required Libbitcoin C++ libraries used in this repository.)
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
