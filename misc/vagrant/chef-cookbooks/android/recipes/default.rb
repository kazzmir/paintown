execute "apt-get-update" do
  command "apt-get update"
  ignore_failure true 
end

packages = %w{scons curl wget make python expect python-pip python-dev libtool subversion git-core pkg-config cmake}

home = "/home/vagrant"

ENV['ANDROID_TOOLCHAIN'] = '/opt/android/android-toolchain'
ENV['PATH'] = ENV['PATH'] + 
              ':/opt/android/build/tools' +
              ':/opt/android/android-toolchain/bin'
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
    echo 'export PATH=$PATH:/opt/android/build/tools:/opt/android/android-toolchain/bin' >> /etc/profile
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
chmod -R 755 /opt/$bundle
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
git clone http://git.code.sf.net/p/alleg/allegro allegro5
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
    make-standalone-toolchain.sh --install-dir=$ANDROID_TOOLCHAIN --platform=android-9
    mkdir -p $ANDROID_TOOLCHAIN/user/armeabi/include $ANDROID_TOOLCHAIN/user/armeabi-v7a/include $ANDROID_TOOLCHAIN/user/armeabi/lib $ANDROID_TOOLCHAIN/user/armeabi-v7a/include $ANDROID_TOOLCHAIN/user/armeabi-v7a/lib
    cp /opt/android/sources/cxx-stl/gnu-libstdc++/4.7/libs/armeabi/libgnustl_static.a $ANDROID_TOOLCHAIN/arm-linux-androideabi/lib
    cd /opt
    git clone https://github.com/cdave1/freetype2-android
    cd freetype2-android/Android/jni
    sed -i 's/android-8/android-9/g' Application.mk
    /opt/android/ndk-build freetype2-static
    ln -s /opt/freetype2-android/include/ft2build.h $ANDROID_TOOLCHAIN/user/armeabi/include $ANDROID_TOOLCHAIN/user/armeabi-v7a/include
    ln -s /opt/freetype2-android/include/freetype/ $ANDROID_TOOLCHAIN/user/armeabi/include $ANDROID_TOOLCHAIN/user/armeabi-v7a/include
    ln -s /opt/freetype2-android/Android/obj/local/armeabi/libfreetype2-static.a $ANDROID_TOOLCHAIN/user/armeabi/lib/libfreetype.a
    ln -s /opt/freetype2-android/Android/obj/local/armeabi-v7a/libfreetype2-static.a $ANDROID_TOOLCHAIN/user/armeabi-v7a/lib/libfreetype.a
    cd /opt
    git clone https://github.com/julienr/libpng-android.git
    cd libpng-android/jni
    sed -i 's/android-8/android-9/g' Application.mk
    sed -i 's/APP_ABI.*$/APP_ABI := all/g' Application.mk
    echo "APP_ABI := armeabi armeabi-v7a" >> Application.mk
    /opt/android/ndk-build
    ln -s /opt/libpng-android/jni/png.h $ANDROID_TOOLCHAIN/user/armeabi/include $ANDROID_TOOLCHAIN/user/armeabi-v7a/include
    ln -s /opt/libpng-android/jni/pngconf.h $ANDROID_TOOLCHAIN/user/armeabi/include $ANDROID_TOOLCHAIN/user/armeabi-v7a/include
    ln -s /opt/libpng-android/obj/local/armeabi/libpng.a $ANDROID_TOOLCHAIN/user/armeabi/lib
    ln -s /opt/libpng-android/obj/local/armeabi-v7a/libpng.a $ANDROID_TOOLCHAIN/user/armeabi-v7a/lib
    cd /opt/allegro5
    # make-standalone-toolchain.sh --platform=android-9 --install-dir=$ANDROID_TOOLCHAIN
    mkdir build
    cd build
    cmake .. -DANDROID_NDK_TOOLCHAIN_ROOT=$ANDROID_TOOLCHAIN -DWANT_ANDROID=on -DWANT_EXAMPLES=OFF -DWANT_DEMO=OFF -DCMAKE_BUILD_TYPE=Release -DWANT_OPENSL=on -DWANT_GLES2=on -DWANT_SHADERS=on
    make && make install
  EOH
end

template ENV['ANDROID_TOOLCHAIN'] + '/bin/libpng-config' do
  source "lib-config.erb"
  mode 0755
  variables({
    :version => '1.2',
    :prefix => ENV['ANDROID_TOOLCHAIN'] + '/user/armeabi-v7a',
    :include_dir => '',
    :libs => '-lpng',
    :all_libs => '-lpng -lz -lm '
  })
end

template ENV['ANDROID_TOOLCHAIN'] + '/bin/freetype-config' do
  source "lib-config.erb"
  mode 0755
  variables({
    :version => '2',
    :prefix => ENV['ANDROID_TOOLCHAIN'] + '/user/armeabi-v7a',
    :include_dir => 'freetype',
    :libs => '-lfreetype',
    :all_libs => '-lfreetype -lz '
  })
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
