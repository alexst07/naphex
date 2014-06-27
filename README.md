# Naphex

Naphex is a software for monitoring network packets, with Naphex is possible monitor the packets and create rules to handle with these packets, for example, it is possible is possible send determined kind of packet, depending of the received packet.

The Naphex works with Lua language, so the rules are created using Lua, it is easy add new protocols on Naphex, because all protocols works like plugins, it means, a dynamic library, so you can create a new protocol to Naphex easily.

## LICENSE

Naphex is under GPLv3, so if you do any modifications to Naphex, these modifications must be redistributed.

## BUILD

Make sure you have the prerequisites installed.

  * libpacp (http://www.tcpdump.org/)
  * Lua (http://www.lua.org/)

Then, create a folder for the build:

    $ mkdir build

From there run:

    $ cmake OPTIONS path_to_source_dir
    $ make MAKEOPTIONS

OPTIONS can be null, or have one of the following values:

  * -DCMAKE_BUILD_TYPE=Debug
  * -DCMAKE_BUILD_TYPE=Release

MAKEOPTIONS can be null or have the following value:

  * install: installs Naphex

Example:

    $ cmake ..
    $ make

## INSTALL

The default install prefix is /usr/local, if you don't intend to change it, just run, after build:

    $ cmke -DCMAKE_BUILD_TYPE=Debug ..
    $ make install

## CONTRIBUTING

  * Install Git.
  * Register at GitHub. Create your fork of Naphex repository (see https://help.github.com/articles/fork-a-repo for details).
  * Choose a task for youself. It could be a bugfix, or some new code.
  * Choose a base branch for your work.
  * Clone your fork to your computer.
  * Create a new branch (with a meaningful name) from the base branch you chose.
  * Modify/add the code following our Coding Style Guide (see http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml).
  * Run testsuite locally.
  * When you are done, push your branch to your GitHub fork; then create a pull request from your branch to the base branch (see https://help.github.com/articles/using-pull-requests for details).
  




