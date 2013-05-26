execute "apt-get-update" do
  command "apt-get update"
  ignore_failure true 
end

packages = %w{scons wget make python expect python-pip python-dev libtool subversion pkg-config}

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

bash "grab-android-sdk" do
  user "root"
  cwd '/opt'
  code <<-EOH
    mod=0
    time=$(date +%m%d --date="-$mod day")
    bundle=adt-bundle-linux-x86-2013$time
    value=`wget -q http://dl.google.com/android/adt/$bundle.zip`
    while [ $? -ne 0 ]; do
        mod=$((mod+1))
        time=$(date +%m%d --date="-$mod day")
        bundle=adt-bundle-linux-x86-2013$time
        value=`wget -q http://dl.google.com/android/adt/$bundle.zip`
    done
    unzip $bundle
    ln -s /opt/$bundle /opt/android-sdk
    android=android-ndk-r8e
    wget http://dl.google.com/android/ndk/$android-linux-x86.tar.bz2
    tar xvjf $android-linux-x86.tar.bz2
    ln -s /opt/$android /opt/android
  EOH
end

file '/opt/android-sdk/sdk/tools/install.sh' do
  mode 0755
  content <<-EOH
#!/usr/bin/expect -f
set timeout -1
spawn ./android update sdk --filter 2 --no-ui
expect {
    "\[y/n\]" { send "y\r"; expect { eof } } \
    eof
}
  EOH
  action :create
end

bash 'setup-android' do
  user "vagrant"
  cwd '/opt'
  code <<-EOH
    cd /opt/android-sdk/sdk/tools
    ./install.sh
    cd /opt/android
    build/tools/make-standalone-toolchain.sh --install-dir=/opt/android/android-toolchain --platform=android-9
    mkdir -p /opt/android/android-toolchain/user/armeabi /opt/android/android-toolchain/user/armeabi-v7a
    cp /opt/android/sources/cxx-stl/gnu-libstdc++/4.7/libs/armeabi/libgnustl_static.a /opt/android/android-toolchain/arm-linux-androideabi/lib/
    cd /opt
    git clone https://github.com/cdave1/freetype2-android
    cd freetype2-android/Android/jni
    sed -i 's/android-8/android-9/g' Application.mk
    /opt/android/ndk-build freetype2-static
    ln -s /opt/freetype2-android/include/ft2build.h /opt/android/android-toolchain/user/armeabi/include
    ln -s /opt/freetype2-android/include/freetype/ /opt/android/android-toolchain/user/armeabi/include
    ln -s /opt/freetype2-android/Android/obj/local/armeabi/libfreetype2-static.a /opt/android/android-toolchain/user/armeabi/lib
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

=begin
# Couldn't get this to work, had to use expect directly

execute 'install_pexpect' do
  command 'pip install pexpect'
  user 'root'
end

python 'setup-android' do
  user 'root'
  cwd '/opt/adt-android/sdk/tools'
  code <<-EOH
import pexpect
cmd = pexpect.spawn('./android update sdk --filter 2 --no-ui')
cmd.expect('Do you accept the license \'android-sdk-\'.*')
cmd.sendLine('y')
  EOH
end
=end
