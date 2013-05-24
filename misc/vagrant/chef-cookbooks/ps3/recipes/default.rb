execute "apt-get-update" do
  command "apt-get update"
  ignore_failure true 
end

packages = %w{scons autoconf automake bison flex texinfo libncurses5-dev gcc make wget libelf-dev python-dev python2.6-dev zlib1g-dev libtool git-core subversion libgmp3-dev libssl-dev pkg-config}

home = "/home/vagrant"

ENV['PS3DEV'] = '/home/vagrant/ps3-toolchain'
ENV['PATH'] = ENV['PATH'] + ':' + 
              ENV['PS3DEV'] + '/bin:' +
              ENV['PS3DEV'] + '/ppu/bin:' +
              ENV['PS3DEV'] + '/spu/bin'
ENV['PSL1GHT'] = '$PS3DEV/psl1ght'
ENV['ps3'] = '1'

packages.each do |pkg|
  package pkg do
    action :install
  end
end

bash "setup-profile" do
  user "root"
  code <<-EOH
    echo 'export PS3DEV=/home/vagrant/ps3-toolchain' >> /etc/profile
    echo 'export PATH=$PATH:\$PS3DEV/bin' >> /etc/profile
    echo 'export PATH=$PATH:\$PS3DEV/ppu/bin' >> /etc/profile
    echo 'export PATH=$PATH:\$PS3DEV/spu/bin' >> /etc/profile
    echo 'export PSL1GHT=$PS3DEV/psl1ght' >> /etc/profile
    echo 'export ps3=1' >> /etc/profile
  EOH
end

bash "co-paintown" do
  user "vagrant"
  cwd home
  code <<-EOH
    svn co --ignore-externals http://svn.code.sf.net/p/paintown/code/trunk paintown
  EOH
end

bash "clone-ps3-toolchain" do
  user "vagrant"
  cwd home
  code <<-EOH
    git clone https://github.com/ps3dev/ps3toolchain.git ps3-toolchain
  EOH
end

bash "ps3-toolchain-build" do
  user "vagrant"
  cwd home+"/ps3-toolchain"
  code <<-EOH
    ./toolchain.sh
  EOH
end

bash "patch-build-psl1ght" do
  user "vagrant"
  cwd home+"/ps3-toolchain/build/ps3libraries/build/sdl_psl1ght"
  code <<-EOH
    patch -p1 < /home/vagrant/paintown/misc/SDL-ps3.diff
    make && make install
  EOH
end

bash "build-paintown" do
  user "vagrant"
  cwd home+"/paintown"
  code <<-EOH
    make
  EOH
end

=begin
4. Patch SDL for the PS3
  $ cd build/ps3libraries/build/sdl_psl1ght
  $ patch -p1 < /path/to/paintown/misc/SDL-ps3.diff
  $ make && make install
    
  5. Make symlinks to the ps3dev directory you just checked out in /opt
    ln -s /path/to/ps3dev /opt/ps3dev

  6. Set the build type environment variable to 'ps3' and build paintown
    export ps3=1
    cd /path/to/paintown
    make

  The pkg currently does not contain the data directory. Once the pkg is installed, copy the data directory to the install directory/USRDIR. Use the ftp program or something to transfer data to the ps3.

  Notes:
  * install using ps3load from payloader3: http://git.dashhacks.com/payloader3
    'ps3load' is in the psl1ght/tools directory from where it was built
  * use an ftp server to upload data: http://ps3.dashhacks.com/downloads/openps3ftp
    Put the data in /dev_hdd0/tmp/data if running from ps3load.

  From the payloader3 screen press 'x, triangle' to put it in ps3load/ethdebug mode. Then use socat to get the output
  $ socat udp-recv:18194 stdout
=end
