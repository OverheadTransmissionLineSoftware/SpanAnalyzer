#!/bin/bash

# This script copies files from a release and installs to the local machine.
# System and user files are installed.

# To avoid overwriting existing files already stored on the system, either
# delete or change file extension on specific installation files.

# creates otls directory
DIR_OTLS=/opt/otls
sudo mkdir -p $DIR_OTLS
sudo chown $USER:$USER $DIR_OTLS

# copies executable
DIR_APP=$DIR_OTLS/span-analyzer
mkdir -p $DIR_APP
cp SpanAnalyzer $DIR_APP

# copies resource files
DIR_RESOURCES=$DIR_APP/res
mkdir -p $DIR_RESOURCES
cp res/*.htb $DIR_RESOURCES

# copies example file(s)
DIR_EXAMPLES=$DIR_APP/examples
mkdir -p $DIR_EXAMPLES
cp examples/*.cable $DIR_EXAMPLES
cp examples/*.spananalyzer $DIR_EXAMPLES

# copies user file(s)
DIR_USER=$HOME/.config/otls/span-analyzer
mkdir -p $DIR_USER
cp *.xml $DIR_USER

# creates a shortcut
ln -f -s $DIR_APP/SpanAnalyzer $HOME/Desktop/SpanAnalyzer
