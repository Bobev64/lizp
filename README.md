# This repo follows along with a book

Most of the code in this repo follows along with the book [Build Your Own Lisp](http://buildyourownlisp.com/contents)

Sources | Role
--------|-------
[Build Your Own Lisp](http://buildyourownlisp.com/contents) | Primary
.

Should be compileable on Linux with
```bash
gcc -std=c99 -Wall prompt.c mpc.c -leditline -lm -o prompt
```

You may also lack libeditline, if so, install on Debian based distros with
```bash
sudo apt-get install libeditline-dev
```
