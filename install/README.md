## Installation Scripts
This directory contains the installation scripts for various platforms. The
scripts are mainly consist of moving files to the proper directories. Each
platform has two scripts:
- Installation from a release. This will be included in the release download.
- Installation from the repository. This requires that the 'Release' target
  already be built.

## Linux
Installs from a release.
```
/bin/bash ./install/linux/install_release.sh
```

Installs from the repository.
```
/bin/bash ./install/linux/install_repo.sh
```

## Windows
Installs from a release.
```
.\install\windows\install_release.bat
```

Installs from the repository.
```
.\install\windows\install_repo.bat
```

