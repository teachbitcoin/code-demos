# Script for building toolkit for teachbitcoin code-demos and exercises.
# Only for AWS linux 2.

# Build BX
# Build dependencies
# sudo yum install autoconf automake libtool pkgconfig boost-devel
# sudo yum groupinstall "Development Tools"
# wget https://raw.githubusercontent.com/libbitcoin/libbitcoin-explorer/version3/install.sh
# sudo chmod +x install.sh
# sudo ./install.sh

# Download BX binary
wget https://github.com/libbitcoin/libbitcoin-explorer/releases/download/v3.2.0/bx-linux-x64-qrcode
sudo chmod +x bx-linux-x64-qrcode
sudo cp bx-linux-x64-qrcode /usr/local/bin/bx
sudo mkdir /usr/local/etc/libbitcoin
sudo cp bx_config_files/bx_testnet.cfg /usr/local/etc/libbitcoin/bx_testnet.cfg
sudo cp bx_config_files/bx_mainnet.cfg /usr/local/etc/libbitcoin/bx_mainnet.cfg
sudo cp bx_config_files/bx.cfg /usr/local/etc/libbitcoin/bx.cfg
echo "export BX_CONFIG='/usr/local/etc/libbitcoin/bx.cfg'" >> $HOME/.bash_profile

#Install miniconda, jupyter, cling kernel, bash kernel.
wget https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
sudo chmod +x Miniconda3-latest-Linux-x86_64.sh
./Miniconda3-latest-Linux-x86_64.sh
source $HOME/.bash_profile

conda install xeus-cling notebook -c QuantStack -c conda-forge

pip install bash_kernel
python -m bash_kernel.install
source $HOME/.bash_profile
