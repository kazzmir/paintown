#!/bin/sh

## Paintown install script

## ./install.sh -d <data> -b <bin>
## Copy the 'data' directory into <data> and put a script to run the game
## in <bin>.

## Example:
## ./install.sh -d ~/my-data -b ~/bin
## Will put the game in the users home directory under my-data and put a script
## in the users bin directory.

## You will probably need to use 'sudo' to install into /usr anywhere
## sudo ./install.sh -d /usr/games -b /usr/bin

## Default data directory is /usr/share/games/paintown-x.x
## Default bin directory is /usr/games

data(){
	dir=$1

	if [ ! -f paintown ]; then
		echo "You must compile paintown first. Read the README for instructions or just type make"
		exit 0
	fi

	echo "Copying data to $dir"
	mkdir -p $dir

	find data -type d ! -regex '.*\.svn.*' -print0 | xargs -0 -I "{}" mkdir -p "$dir/{}"
	find data -type f ! -regex '.*\.svn.*' -print0 | xargs -0 -I "{}" cp "{}" "$dir/{}"
	cp paintown $dir/paintown-bin
}

bin(){
	data_dir=$1
	bin_dir=$2

    mkdir -p $bin_dir
	echo "$data_dir/paintown-bin -d $data_dir/data $*" >> $bin_dir/paintown
	chmod +x $bin_dir/paintown
}

echo "./install.sh -d <data> -b <bin>"
echo "Install data to <data> and the executable script in <bin>"

version=3.6.1
install_data=
install_bin=

if [ "x$DESTDIR" != "x" ]; then
    install_data=$DESTDIR/usr/share/paintown-$version
    install_bin=$DESTDIR/usr/bin/games
fi

while getopts d:b: o
do
	case $o in
		d) install_data="$OPTARG";;
		b) install_bin="$OPTARG";;
	esac
done

if [ "x$install_data" = "x" ]; then
	default_data=/usr/share/games/paintown-$version
	echo "Give a directory to install the data to: [$default_data]"
	read install_data
	if [ "x$install_data" = "x" ]; then
		install_data=$default_data
	fi
fi

if [ "x$install_bin" = "x" ]; then
	default_bin=/usr/games
	echo "Give a directory to create the paintown script in: [$default_bin]"
	read install_bin
	if [ "x$install_bin" = "x" ]; then
		install_bin=$default_bin
	fi
fi

data $install_data
bin $install_data $install_bin

echo "Done. Run $install_bin/paintown to play."
