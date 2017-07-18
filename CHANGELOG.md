# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project "attempts" to adhere to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Changed
- Pretty-ifying space separators

## [0.1.1] - 2017-07-18
### Added
- New Makefile target "cleanest" to remove all binaries, object code, and shared libraries
- Completed implementation of MapIntersect template function in include/Utilities*

### Changed
- Implementaiton of MapIntersect to use a std::pair container rather than FastQ::FastQPaired

## [0.1.0] - 2017-07-14
### Added
- Utilities shared library (libUtilities.so/ lUtilities)
- "res/" directory for resources (ex.images)
- resource file "NGSeXplore.png"

### Changed
- Started versioning according to Semantic Versioning specifications (1.0.5 -> 0.1.0)
- Accessory code in "include/" is now implemented as shared libraries.
- Makefile now includes improved automation with pattern rules and implicit rules.

### Removed
- STATIC-LIB.mk: now depracated. Now implemented in Makefile as shared libraries.

## [1.0.3] - 2017-06-14
### Added
- The last official branch using arbitrary versioning before switching to Semantic Versioning
- Unofficially went up to 1.0.5
- Current src files in modules and pipelines for (TBD!)


https://github.com/ktmeaton/NGSeXplore.git

[Unreleased]: https://github.com/ktmeaton/NGSeXplore/compare/v0.1.2...HEAD
[0.1.0]: https://github.com/ktmeaton/NGSeXplore/compare/v0.1.0...1.0.3
[0.1.1]: https://github.com/ktmeaton/NGSeXplore/compare/v0.1.1...1.0.0
[1.0.3]: https://github.com/ktmeaton/NGSeXplore/compare/1.0.3...1.0.2
