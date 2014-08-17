This Software is a collection of simple eCAP adapters.
The adapters illustrate eCAP usage but are not be suitable for production use.

Available adapters (installed in /usr/local/lib/ by default)

The libecap library is required to build and use these adapters. You can get
the library from http://www.e-cap.org/. The adapters can be built and
installed from source, usually by running:

    % ./configure
    % make
    % make install

For adding source files, you should modify the src/Makefile.am file and running:

    % autoreconf -ivf

For documentation, the libecap library implementation, and support
information, please visit the eCAP project web site: http://www.e-cap.org/
