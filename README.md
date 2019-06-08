# NGSeXplore
Exploratory Data Analysis of NGS Sequence Data  
https://ktmeaton.github.io/NGSeXplore

## Version

Last Stable Release - master (v0.1.5)

## Prerequisites
make  
g++

## Installation

Release:  
git clone https://github.com/ktmeaton/NGSeXplore.git  
cd NGSeXplore  

### Compile single programs  
make bin/NGSXRemoveDuplicates  
make bin/NGSXFastQStats  
### Compile all programs  
make  

## Usage

Binaries in "bin/" are directly executable from anywhere.

## Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## History

See CHANGELOG.md.

## Credits

author: Katherine Eaton (ktmeaton@gmail.com)

## License

TODO: Write license

## Helpful commands  
Merging a development branch into master:  
    (on branch development)`$ git merge master`  
    (resolve any merge conflicts if there are any)  
    `git checkout master`  
    `git merge --no-ff development` (there won't be any conflicts now)  
