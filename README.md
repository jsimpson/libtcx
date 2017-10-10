# fasttcx
A C TCX file parser

## Development

### Notes

#### libxml2

On Mac, libxml2 needs to be installed and needs to find it's way in to the include path. Traditional methods failed me and what I ended up with was symlinking the Homebrew install path to the include path. Not great, but it works.

```sh
sudo ln -s /usr/local/opt/libxml2/include/libxml2/libxml /usr/local/include/libxml
```

