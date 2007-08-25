#!/bin/sh

data(){
	dir=$1

	echo "Copying data to $dir"
	mkdir -p $dir

	find data -type d ! -regex '.*\.svn.*' | xargs -i mkdir -p $dir/\{}
	find data -type f ! -regex '.*\.svn.*' | xargs -i cp \{} $dir/\{}
	cp paintown $dir/paintown-bin
}

bin(){
	data_dir=$1
	bin_dir=$2

	echo "$data_dir/paintown-bin -d $data_dir/data" >> $bin_dir/paintown
	chmod +x $bin_dir/paintown
}

echo "./install.sh -d <data> -b <bin>"
echo "Install data to <data> and the executable script in <bin>"

install_data=
install_bin=

while getopts d:b: o
do
	case $o in
		d) install_data="$OPTARG";;
		b) install_bin="$OPTARG";;
	esac
done

if [ "x$install_data" = "x" ]; then
	echo "Give a directory to install the data to: [/usr/share/games/paintown]"
	read install_data
	if [ "x$install_data" = "x" ]; then
		install_data=/usr/share/games/paintown
	fi
fi

if [ "x$install_bin" = "x" ]; then
	echo "Give a directory to create the paintown script in: [/usr/games]"
	read install_bin
	if [ "x$install_bin" = "x" ]; then
		install_bin=/usr/games
	fi
fi

data $install_data
bin $install_data $install_bin

echo "Done. Run $install_bin/paintown to play."
