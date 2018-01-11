## Help Files
The help manual topics are grouped into HTML books. The HTML books use the
Microsoft HTML Help Workshop file format.

Each HTML book has its own subdirectory to store resource files (html, image,
HTML Help Workshop project, etc). The books are individually zipped into a
`.htb` file. This duplicates the help files in the repository, but allows the
individual help resource files to be tracked.

These help files are not embedded into the executable. Instead, they are stored
in a `res` directory alongside the executable. The help files are loaded into
the application on startup.

## XPM Files
The XPM files contain small images that are used in the application. The image
information is stored in a C string, so these are embedded into the executable.

## XRC Files
The XRC files contain the window, dialog, and menu layouts used in the
application. These are embedded into the executable using the `wxrc` utility
provided by wxWidgets.
