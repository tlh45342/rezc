# rezc
A resource compiler of sorts

This is a dirt simple resource compiler and that is being generous.  Its whole task is to take a resource file and compile it, or produce a binary afrom it.
The directives are basic for this verision and inclde .hex .dec .name

## Getting

```bash
git clone https://github.com/tlh45342/rezc.git
```

# Installation

To build and install rezc
The default is for clean build install

```bash
make
```

## STRUCTURE

    ├── documentation               Documentation
    │   ├── directives.md           rezc directives   
    │   └── project_guidelines.md   Guidelines for project developement   
    ├── LICENSE                     Copy of the Apache 2.0 license
    ├── Makefile                    The Makefile
    ├── README.md                   This file.
    ├── src                
    │   └── rezc.c    
    └── tests
        ├── helloworld.txt          PDP11 baremental bin @ 2000
        ├── Makkefile               Main App
        ├── test1.txt               example
        ├── test2.txt               example
        ├── test3.txt               example
        ├── test4.txt               example
        ├── test5.txt               example
        ├── test6.txt               example with bad directive; a primitive test      
        └── test7.txt               example .oct directive given but data is ff which is illegal

# Credits

tlh45342@gmail.com
I have had a version like this in some form since the 80's.  I needed it and couldn't find it, so here it is again.

## LICENSE

rezc is licensed under the Apache License, Version 2.0. Find it for the full text.
