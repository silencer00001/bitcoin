Some quirks to document when building bitcoin/core on centos:

berkeley db is not in the yum repos:

  download from:  http://download.oracle.com/berkeley-db/db-5.3.21.tar.gz

  Install optional dependencies:
  ```
  sudo yum install tcl
  sudo yum install java-1.7.0-openjdk java-1.7.0-openjdk-devel
  sudo yum install sharutils
  ```
  Install Berkeley DB
  ```
  cd build_unix
  ../dist/configure --prefix=/usr --enable-compat185 --enable-dbm --disable-static --enable-cxx
  make
  ```
  then run
  ```
  sudo make docdir=/usr/share/doc/db-5.3.21 install
  sudo chown -v -R root:root /usr/bin/db_* /usr/include/db{,_185,_cxx}.h /usr/lib/libdb*.{so,la} /usr/share/doc/db-5.3.21
  ```
  when you run db_recover or other commands, you may encounter “libdb-5.3.so: cannot open shared object file: No such file or directory"
  ```
  sudo /sbin/ldconfig
  ```
  Source:http://www.linuxfromscratch.org/blfs/view/svn/server/db.html

you must link during compile step

  ```
  ./configure --includedir=/usr/include/boost/ --with-incompatible-bdb
  ```

and then run ```make```.

