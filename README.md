# PHP-minifier

If used when deploying a website it can reduce the total size of the project
Please don't use it during development, unless you like suffering ...

## How to use it ?

```shell
sh minifier.sh 'src/'
```
This will minify every `.php` files contained in the `src` directory, except those inside the ignored subdirectories.

### Ignored folders ?

I usually put all my files inside a `_dev` folder, so I decided to filter it out in my script to avoid potential mistake. 
It will still minify anything that's outside this folder, beware.

Feel free to add your own by modifying the `-not -path` filter inside the shell script:

```shell
for PHP_FILE in `find $IN_DIR -name "*.php" -not -path "**/_dev/*"`
```

## Prerequesites

- GCC

