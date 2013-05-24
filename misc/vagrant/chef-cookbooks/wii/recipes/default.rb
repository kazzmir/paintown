execute "apt-get-update" do
  command "apt-get update"
  ignore_failure true 
end

packages = %w{scons wget make libelf-dev python-dev python2.6-dev libtool subversion pkg-config}

home = "/home/vagrant"

ENV['DEVKITPRO'] = '/opt/devkitpro'
ENV['DEVKITPPC'] = ENV['DEVKITPRO']+'/devkitPPC'
ENV['PATH'] = ENV['PATH'] + 
              ':' + ENV['DEVKITPPC'] + '/bin' +
              ':' + ENV['DEVKITPRO'] + 'libogc/bin'
ENV['SDL'] = '1'
ENV['wii'] = '1'

packages.each do |pkg|
  package pkg do
    action :install
  end
end

bash "setup-profile" do
  user "root"
  code <<-EOH
    echo 'export DEVKITPRO=/opt/devkitpro' >> /etc/profile
    echo 'export DEVKITPPC=$DEVKITPRO/devkitPPC' >> /etc/profile
    echo 'export PATH=$PATH:$DEVKITPRO/libogc/bin:$DEVKITPPC/bin' >> /etc/profile
    echo 'export SDL=1' >> /etc/profile
    echo 'export wii=1' >> /etc/profile
  EOH
end

bash "co-paintown" do
  user "vagrant"
  cwd home
  code <<-EOH
    svn co --ignore-externals http://svn.code.sf.net/p/paintown/code/trunk paintown
  EOH
end

bash "grab-wii-toolchain" do
  user "root"
  cwd '/opt'
  code <<-EOH
    svn co http://svn.code.sf.net/p/paintown/code/build-environments/devkitpro
  EOH
end

bash "build-paintown" do
  user "vagrant"
  cwd home+"/paintown"
  code <<-EOH
    make
    elf2dol paintown paintown.dol
  EOH
end
