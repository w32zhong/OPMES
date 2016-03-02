# About 
This is the repository of OPMES (Operation-tree Pruning based Math Expression Search).
The code implements a prototype and is for demonstration only.

The source repo is mainly for the code reference of our published system on ECIR 2016, and source code is not intended to be update in this repo. 

Our paper can be downloaded from [here](https://github.com/tkhost/tkhost.github.io/blob/master/opmes/ecir2016.pdf) and you may find [the slides](https://github.com/tkhost/tkhost.github.io/blob/master/opmes/ECIR16-OPMES-slides-handouts.pdf) helpful to understand our system.

Also, here is the link to our online demo:
[http://tkhost.github.io/opmes](http://tkhost.github.io/opmes)

# Demo Plan 
In our paper, we have a demo plan trying to illustrate how our system related to our method by showing:

* Parser output
* Index tree structure
* A simple query-to-results command and explanation.

You can going through the above by the following instructions:

1. Clone source code from this repo
2. After building (simply type ``make``) the project, run ``parser/parser.out`` to see parser output given an input LaTeX mode string.
3. Type ``make demo`` and view a demo index tree under directory ``./col``
4. Search a simple query by typing

		./search/search.out -n -q '1/2 (n-1)!'

## Inner Module Dependency
`common` -> `parser` -> `index` -> `search` -> `web`

## External Binary/Library Dependency
The typical installations to meet all the binary/library dependencies:

1. [bison](http://ftp.gnu.org/gnu/bison/bison-3.0.tar.xz)
2. [flex and libfl](http://sourceforge.net/projects/flex/files/flex-2.5.39.tar.xz/download)
3. [libreadline](http://ftp.gnu.org/gnu/readline/readline-6.3.tar.gz)
4. [libncurses](http://ftp.gnu.org/pub/gnu/ncurses/ncurses-5.7.tar.gz)
5. [libcrypto and libssl](http://ftp.de.debian.org/debian/pool/main/o/openssl/openssl_0.9.8o.orig.tar.gz)
6. [libtokyocabinet](http://fallabs.com/tokyocabinet/tokyocabinet-1.4.48.tar.gz)
	* [libz](http://zlib.net/zlib-1.2.8.tar.gz)
	* [libbz](http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz)
7. [libcurl](http://curl.haxx.se/download/curl-7.41.0.tar.gz)

For Debian/Ubuntu users, issue the following command:
```
sudo apt-get update
sudo apt-get install bison flex libreadline-dev libncurses5-dev \
                     libcurl4-openssl-dev libtokyocabinet-dev libbz2-dev
```

## Index Size
In an experimental index, a bzip2 compressed 7.6 MB data file (all are plain text files with one formula in every line) would result in a 1.3 GB collection directory, and hundreds MB of database files.

Another example, when finished the index of the entire math.stackexchange.com (over 8 million LaTeX equation IDs in 27180 pages of questions), it has:

* 892M    formula.db
* 7.2G    textree.db
* 65M     webpage.db
* 9.4G    collection directory.

## Apache2 CGI Configuration 
Here are commands on Ubuntu 14.04 distribution as an example.

1. Enable CGI module:

		sudo a2enmod cgi

2. Config `/etc/apache2/apache2.conf`, adding line:

		ScriptAlias /cgi/ /var/www/foo/cgi/
		Alias /foo/ /var/www/foo/res/

3. Restart Apache server:

		sudo /etc/init.d/apache2 restart

## Nginx CGI Configuration 

1. Install fcgiwrap (and fastcgi if you do not have)

		sudo pacman -S fcgiwrap
		sudo systemctl enable fcgiwrap.socket
		sudo systemctl start fcgiwrap.socket

2. Configure nginx.conf:

		location ~ \.cgi$ {
			fastcgi_pass   unix:/run/fcgiwrap.sock;
			fastcgi_param  SCRIPT_FILENAME  /usr/share/nginx/html/foo$fastcgi_script_name;
			include        fastcgi_params;
		}
		location /foo/ {
			alias  /usr/share/nginx/html/foo/res/;
		}

3. Restart Nginx server:

		sudo systemctl restart nginx

## Testing CGI program for your httpd
Just generate a CGI program (e.g. `web/helloworld.cgi`), copy it into directory `/var/www/foo/`.

To test it, simply open the Web browser and enter the URL:

http://127.0.0.1/cgi/helloworld.cgi

## Deploying on Your Server 
1. ``git clone --depth=1`` the master branch. 

2. Copy the project to your server. You may want to delete ``doc``, ``.git`` folders and ``crawler/*.tar.gz2`` to save your transmission time.

3. Before building on your server machine, you probably need to modify:
	* All the pointed directories for library dependency in ``dep/*`` files.
	* The ``web/config.mk`` file to configure your hosting directory (e.g. under your Apache server ``DocumentRoot``).

4. ``make``, ``cd web/`` and ``make install`` to finally install on your server.
