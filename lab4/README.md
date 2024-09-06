# Hey! I'm Filing Here

In this lab, I successfully implements an ext2 filesystem with 1 KiB sized blocks and 128 inodes, featuring a root directory, a lost+found directory, a regular file named "hello-world", and a symbolic link named "hello" pointing to "hello-world"

## Building
Run this command with in the directory of all the other files to compile 

make

 Then run this command; it will generate an executable named ext2-create, which can be used to create the filesystem image.

./ext2-createO

## Running
We can now run multiple commands. 

To dump the file system info for debugging, run this command:

dumpe2fs cs111 -base.img

To check that your filesystem is correct, run this command:

fsck.ext2 cs111 -base.img 

To mount our file system, we create a directory and then mounts the filesystem. Run this pair of commands: 

mkdir mnt 
sudo mount -o loop cs111 -base.img mnt 

## Cleaning up

If you mounted your filesystem, when done, you unmount and remove the directory with the following commands:

sudo umount mnt
rmdir mnt 

To clean the rest of the files, run the following command:

make clean
