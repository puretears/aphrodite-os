#!/bin/bash

VIM_PLUGIN="/home/puretears/.vim/plugin"
VIM_DOCS="/home/puretears/.vim/doc"
VIM_SYNTAX="/home/puretears/.vim/syntax"
C_SYNTAX="c.vim"

if [ ! -d "${VIM_PLUGIN}" ]; then
	mkdir "${VIM_PLUGIN}"
fi

if [ ! -d "${VIM_DOCS}" ]; then
	mkdir ${VIM_DOCS}
fi

if [ ! -d "${VIM_SYNTAX}" ]; then
	mkdir ${VIM_SYNTAX}
fi

if [ ! -e "${C_SYNTAX}" ]; then
	mkdir ${C_SYNTAX}
fi

echo "Install ctags..."

sudo aptitude install ctags
sudo aptitude install cscope

if [ ! -e "taglist_45.zip" ]; then
	wget http://www.vim.org/scripts/download_script.php?src_id=7701 -O ~/.vim/taglist_45.zip
fi


if [ ! -e "winmanager.zip" ]; then
	wget http://www.vim.org/scripts/download_script.php?src_id=754 -O ~/.vim/winmanager.zip 
fi

if [ ! -e "minibufexpl.vim" ]; then
	wget http://www.vim.org/scripts/download_script.php?src_id=3640 -O ~/.vim/plugin/minibufexpl.vim
fi

if [ ! -e "a.vim" ]; then
	wget http://www.vim.org/scripts/download_script.php?src_id=7218 -O ~/.vim/plugin/a.vim
fi

if [ ! -e "grep.vim" ]; then
	http://www.vim.org/scripts/download_script.php?src_id=7645 -o ~/.vim/plugin/grep.vim
fi

cd ~/.vim/
unzip ~/.vim/taglist_45.zip 
unzip ~/.vim/winmanager.zip


