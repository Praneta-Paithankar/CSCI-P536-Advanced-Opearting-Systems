Following functions are implemented in this assignment:

1. int fs_create(char *filename, int mode) :

This function creates file and returns its file descriptor.
This function creates file only if mode is O_CREAT. If there is no free inode present in the file system, it returns SYSERR. 
If inode is empty, inode is filled with new file entry and entry is written using fs_put_inode_by_num() function. Inode is also added into directory entry.
Entry in oft is created and status of file is open.

2. int fs_open(char *filename, int flags):

This function opens file and returns its file descriptor. It checks whether flag has appropriate value such as read only, write only or read and write mode.
If flag is not correct, then error is shown. It checks whether file is created before opening it. If file is not created, then error message is displayed. 
After checking the file in directory, file entry is checked in open file table (oft). If File is already opened, then error message is shown. 
If file is not present in oft, new entry is created in oft.

3. int fs_close(int fd):

This function closes the file. If value of fd is invalid, then this function returns SYSERR.
It also shows error message when file is already closed. If file state is open, it closes the file and sets its fileptr to 0.

4. int fs_seek(int fd, int offset)

This function changes the value of fileptr of file. Fileptr is used to read file from specified location. 
Offset is added to current value of fileptr.

5.int fs_read(int fd, void *buf, int nbytes):

This function is used to read nbytes from a file. It stores the result in buf and returns number of byte read. 
If value of fd then this function shows error message and returns SYSERR. It also shows error if file is closed. 
Value of nbytes should be greater than 0 else error will occur. It also throws error if file is empty. 
It will read nbytes from the location pointed by fileptr. If remaining bytes from fileptr is less than requested bytes, 
function reads all bytes and returns count of bytes. Data block is read using bs_bread() function.

6.int fs_write(int fd, void *buf, int nbytes)

This function writes the file with the content present in buffer. If nbytes is zero or fd is invalid, then it throws error.
It will also throw error if file is closed. In this function, free data block is checked using fs_getmaskbit(). 
If free block is present then 512 bytes of data is written in free block using bs_bwrite(). 
Total number of blocks required to write data is calculated by dividing nbytes by block size. 
If nbytes is not completely divisible by block size, then one more block is added. 
After writing the data into free block then its maskbit is set using fs_setmaskbit() function. 
This block number is added into blocks array of inode by using fs_put_inode_by_num. block number is also added in blocks array of inode of file entry in oft. 
This function returns the number of bytes written.

Lessons learned:

This assignment has helped me to understand the file system structure of ext2. It also helped to understand the working of system file calls such create, open etc. 




