execute "apt-get-update" do
  command "apt-get update"
  ignore_failure true 
end

packages = %w{scons curl wget make python expect python-pip python-dev libtool subversion git-core pkg-config cmake}

home = "/home/vagrant"

ENV['ANDROID_TOOLCHAIN'] = '/opt/android/android-toolchain'
ENV['PATH'] = ENV['PATH'] + 
              ':/opt/android/build/tools'
ENV['ALLEGRO5'] = '1'
ENV['PKG_CONFIG_PATH'] = '/opt/android/android-toolchain/user/armeabi-v7a/lib/pkgconfig/'
ENV['android'] = '1'

packages.each do |pkg|
  package pkg do
    action :install
  end
end

bash "setup-profile" do
  user "root"
  code <<-EOH
    echo 'export ANDROID_TOOLCHAIN=/opt/android/android-toolchain' >> /etc/profile
    echo 'export PATH=$PATH:/opt/android/build/tools' >> /etc/profile
    echo 'export ALLEGRO5=1' >> /etc/profile
    echo 'export android=1' >> /etc/profile
    echo 'export PKG_CONFIG_PATH=/opt/android/android-toolchain/user/armeabi-v7a/lib/pkgconfig/' >> /etc/profile
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
#value=`wget -q http://dl.google.com/android/adt/$bundle.zip`
value=`curl -f -s -O http://dl.google.com/android/adt/$bundle.zip`
while [ $? -ne 0 ]; do
    mod=$((mod+1))
    time=$(date +%m%d --date="-$mod day")
    bundle=adt-bundle-linux-x86-2013$time
#    value=`wget -q http://dl.google.com/android/adt/$bundle.zip`
    value=`curl -f -s -O http://dl.google.com/android/adt/$bundle.zip`
done
unzip $bundle
ln -s /opt/$bundle /opt/android-sdk
  EOH
end

bash 'grab-android-ndk' do
  user 'root'
  cwd '/opt'
  code <<-EOH
android=android-ndk-r8e
#wget -q http://dl.google.com/android/ndk/$android-linux-x86.tar.bz2
curl -f -s -O http://dl.google.com/android/ndk/$android-linux-x86.tar.bz2
tar xjf $android-linux-x86.tar.bz2
ln -s /opt/$android /opt/android
  EOH
end

bash 'co-allegro' do
  user 'root'
  cwd '/opt'
  code <<-EOH
svn co https://alleg.svn.sourceforge.net/svnroot/alleg/allegro/branches/5.1 allegro5
  EOH
end

file '/opt/android-sdk/sdk/tools/install.sh' do
  mode 0755
  content <<-EOH
#!/usr/bin/expect -f
set timeout -1
spawn ./android update sdk --filter 2 --no-ui
expect {
    "\\[y/n\\]" { send "y\\r"; expect { eof } } \\
    eof
}
  EOH
  action :create
end

bash 'setup-android' do
  user "root"
  cwd '/opt'
  code <<-EOH
    cd /opt/android-sdk/sdk/tools
    ./install.sh
    cd /opt/android
    make-standalone-toolchain.sh --install-dir=/opt/android/android-toolchain --platform=android-9
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
    cd /opt
    git clone https://github.com/julienr/libpng-android.git
    cd libpng-android/jni
    sed -i 's/android-8/android-9/g' Application.mk
    sed -i 's/APP_ABI.*$/APP_ABI := all/g' Application.mk
    /opt/android/ndk-build
    ln -s /opt/libpng-android/jni/png.h /opt/android/android-toolchain/user/armeabi/include
    ln -s /opt/libpng-android/jni/pngconf.h /opt/android/android-toolchain/user/armeabi/include
    ln -s /opt/libpng-android/obj/local/armeabi/libpng.a /opt/android/android-toolchain/user/armeabi/lib
    cd /opt/allegro5
    make-standalone-toolchain.sh --platform=android-9 --install-dir=$ANDROID_TOOLCHAIN
    mkdir build
    cd build
    cmake .. -DANDROID_NDK_TOOLCHAIN_ROOT=$ANDROID_TOOLCHAIN -DWANT_ANDROID=on -DWANT_EXAMPLES=OFF -DWANT_DEMO=OFF -DCMAKE_BUILD_TYPE=Release -DWANT_OPENSL=on -DWANT_GLES2=on -DWANT_SHADERS=on
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
