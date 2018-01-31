# NGSeXplore
Exploratory Data Analysis of NGS Sequence Data  
https://ktmeaton.github.io/NGSeXplore

## Version

Last Stable Release - Version 0.1.4 (master)  
Development - Version 0.1.5 (v0.1.5)

## Installation

Release:  
git clone https://github.com/ktmeaton/NGSeXplore.git  
cd NGSeXplore  
make

Development:  
git clone -b v0.1.2 https://github.com/ktmeaton/NGSeXplore.git  
cd NGSeXplore    
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

## Formatting

Code should be formatted with AStyle according to the following options:

--suffix=none   
--recursive   
--style=allman   
--indent=spaces=4   
--convert-tabs    
--remove-comment-prefix    
--max-code-length=80   
--break-after-logical   
--indent-classes   
--indent-switches   
--indent-modifiers   
--indent-namespaces   
--indent-after-parens   
--indent-continuation=4   
--break-blocks   
--pad-oper   
--pad-comma   
--pad-paren-in   
--pad-header   
--mode=c   
--lineend=linux  
*.cpp *.h  

## Helpful commands  
Merging a development branch into master:  
    (on branch development)$ git merge master  
    (resolve any merge conflicts if there are any)  
    git checkout master  
    git merge --no-ff development (there won't be any conflicts now)  
