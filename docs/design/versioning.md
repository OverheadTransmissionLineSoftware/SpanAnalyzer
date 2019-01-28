# versioning

## app
SpanAnalyzer uses [semantic versioning](https://semver.org/)
(i.e major.minor.patch).

Examples of major version changes include:
* updated xml file versions
* updates that are not backward compatible

Examples of minor version changes include:
* updates that are backward compatible

Examples of patch version changes include:
* bug fixes

## xml files
SpanAnalyzer applies versioning to every file it loads or saves. The file
version is attributed in the top level xml node.

When loading a file, a specific parser is used that aligns with the file
version. When a file is saved, it is always done using the latest version.

A new file version is needed if the format of any of the child xml nodes are
updated. This allows individual child xml nodes to have their own versioning
system, which makes the node reusable/modular for use in other files if needed.
Specific examples of format changes include:
* additional child xml node types
* updated child xml node versions
