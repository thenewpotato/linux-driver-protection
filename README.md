- `mknod /dev/stringstore c [MAJOR] 0`
- `head -c10 /dev/stringstore`
- `echo "Bye!" > /dev/stringstore`
- `head -c10 /dev/stringstore`