//The MIT License
//
//Copyright (c) 2008 Michael Simpson
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

This project was built on VS2008 with the commercial version of QT 4.4.  I don't have
any experience with MacOSx or KDE/Linux but I can't imagine that this source won't compile
on those platforms.

I pretty much just build it where I need it.  I only run it when I make schema changes to my database.
This project saves me ALOT of time when programming code that runs against Sqlite.

IMPORTANT!!!
When you create database tables, the first field must be the key field.  The code assumes that ordinal 0 is the key field.
As you can see in my test database, I just use _id.

One more note.  If you are going to be inserting a large number of records, open a transaction and use commit when done.  This will
make an operation that takes 25 minutes without a transaction, take 5 seconds.

Michael