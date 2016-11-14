[![Build Status](https://travis-ci.org/FragJage/xPLDatabase.svg?branch=master)](https://travis-ci.org/FragJage/xPLDatabase)
[![Coverage Status](https://coveralls.io/repos/github/FragJage/xPLDatabase/badge.svg?branch=master&bust=1)](https://coveralls.io/github/FragJage/xPLDatabase?branch=master)
[![Coverity Scan Build](https://scan.coverity.com/projects/10420/badge.svg)](https://scan.coverity.com/projects/10420)
xPLDatabase
===========
xPL daemon to log xPL message values in sqlite or mysql database. 

Introduction
============
This daemon receive xPL message and record device and value in a database. 


Features
========
 - Filter for devices 
 - Support Sqlite and Mysql 
 - Can send stat message to obtain recorded values, max, min and average  
 
Portability
===========
TODO
Unit tests passed successfully on :
 - Windows Seven (CPU Intel Celeron)
 - Linux Ubuntu (CPU Intel Atom)
 - Linux Raspian on Raspberry Pi (CPU ARM)
 - Linux FunPlug on NAS DNS-320 (CPU ARM)

For more informations
=====================
TODO
See documentation in doc/html/index.html

Licence
=======
xPLDatabase is free software : you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

xPLDatabase is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with xPLDatabase. If not, see http://www.gnu.org/licenses/.
