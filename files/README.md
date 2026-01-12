# Simple static site server
So today I was bored and I found a tutorial on this. I built a really simple HTTPS server.
It's just a tiny project to understand C better and to learn multithreading.
I learned how the internet protocols are really implemented and found it pretty interesting.

## Features ##
* It can serve static html files in the /files dir
* It can serve multiple browsers at once using multithreading
* It can do nothing else and is probably not secure

## Project Structure
```text
.
├── main.c              # The server source code
├── README.md           # Documentation
└── files/              # Root directory for web content
    ├── index.html      # Homepage
    ├── 404.html        # Error page
    └── favicon.ico     # Browser tab icon
```

## Flaws ##
* Users can request the main.c source file and probably other files too XD
* It doesn't really handle errors the way it should

Maybe I'll get back to it later :)


