# Sqlite code generator for Qt

This project is a fork of qsqlcodegen, which can be found here: http://sourceforge.net/projects/qsqlcodegen/

## Building
You will need Qt 4.8 in order to build qt-sqlite-generator.  There is a pro file and a visual studio 2008 solution
provided.

## Notes
When you create database tables, the first field must be the key field.  The code assumes that ordinal 0 is the key field.
As you can see in my test database, I just use _id.

One more note.  If you are going to be inserting a large number of records, open a transaction and use commit when done.  This will
make an operation that takes 25 minutes without a transaction, take 5 seconds.

## License
The code is released under the MIT license.
