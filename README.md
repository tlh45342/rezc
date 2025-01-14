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

```bash
make install
```

## STRUCTURE

    ├── LICENSE                     Copy of the Apache 2.0 license
    ├── README.md                   This file.
    ├── requirements.txt            module requirements
    ├── server.py                   Wsgi app
    └── app
       ├── __init__.py
       ├── app.py                   Main App
       ├── routes.py                flask routes
       ├── user.py                  manager user management routes
       ├── user_db.py               manage sqlite3 db       
       ├── static                   Static files
       │   └── mystyle.css          Fairly minimalistic choices   
       └── templates                Jinja2 templates
           ├── login.html
           ├── protected.html
           └── welcome.html

# Credits

tlh45342@gmail.com
I have had a version like this in some form since the 80's.  I needed it and couldn't find it, so here it is again.

## LICENSE

rezc is licensed under the Apache License, Version 2.0. Find it for the full text.
