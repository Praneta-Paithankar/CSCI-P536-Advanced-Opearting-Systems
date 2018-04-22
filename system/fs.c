#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
    int diskblock;
    
    if (fileblock >= INODEBLOCKS - 2) {
        printf("No indirect block support\n");
        return SYSERR;
    }
    
    diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address
    
    return diskblock;
}

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
    int bl, inn;
    int inode_off;
    
    if (dev != 0) {
        printf("Unsupported device\n");
        return SYSERR;
    }
    if (inode_number > fsd.ninodes) {
        printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
        return SYSERR;
    }
    
    bl = inode_number / INODES_PER_BLOCK;
    inn = inode_number % INODES_PER_BLOCK;
    bl += FIRST_INODE_BLOCK;
    
    inode_off = inn * sizeof(struct inode);
    
    /*
     printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
     printf("inn*sizeof(struct inode): %d\n", inode_off);
     */
    
    bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
    memcpy(in, &block_cache[inode_off], sizeof(struct inode));
    
    return OK;
    
}

int fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
    int bl, inn;
    
    if (dev != 0) {
        printf("Unsupported device\n");
        return SYSERR;
    }
    if (inode_number > fsd.ninodes) {
        printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
        return SYSERR;
    }
    
    bl = inode_number / INODES_PER_BLOCK;
    inn = inode_number % INODES_PER_BLOCK;
    bl += FIRST_INODE_BLOCK;
    
    /*
     printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
     */
    
    bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
    memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
    bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);
    
    return OK;
}

int fs_mkfs(int dev, int num_inodes) {
    int i;
    
    if (dev == 0) {
        fsd.nblocks = dev0_numblocks;
        fsd.blocksz = dev0_blocksize;
    }
    else {
        printf("Unsupported device\n");
        return SYSERR;
    }
    
    if (num_inodes < 1) {
        fsd.ninodes = DEFAULT_NUM_INODES;
    }
    else {
        fsd.ninodes = num_inodes;
    }
    
    i = fsd.nblocks;
    while ( (i % 8) != 0) {i++;}
    fsd.freemaskbytes = i / 8;
    
    if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
        printf("fs_mkfs memget failed.\n");
        return SYSERR;
    }
    
    /* zero the free mask */
    for(i=0;i<fsd.freemaskbytes;i++) {
        fsd.freemask[i] = '\0';
    }
    
    fsd.inodes_used = 0;
    
    /* write the fsystem block to SB_BLK, mark block used */
    fs_setmaskbit(SB_BLK);
    bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
    
    /* write the free block bitmask in BM_BLK, mark block used */
    fs_setmaskbit(BM_BLK);
    bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);
    
    return 1;
}

void fs_print_fsd(void) {
    
    printf("fsd.ninodes: %d\n", fsd.ninodes);
    printf("sizeof(struct inode): %d\n", sizeof(struct inode));
    printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
    printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
    int mbyte, mbit;
    mbyte = b / 8;
    mbit = b % 8;
    
    fsd.freemask[mbyte] |= (0x80 >> mbit);
    return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
    int mbyte, mbit;
    mbyte = b / 8;
    mbit = b % 8;
    
    return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
    return OK;
    
}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
    int mbyte, mbit, invb;
    mbyte = b / 8;
    mbit = b % 8;
    
    invb = ~(0x80 >> mbit);
    invb &= 0xFF;
    
    fsd.freemask[mbyte] &= invb;
    return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
 block is indicated in the high-order bit.  Shift the byte by j
 positions to make the match in bit7 (the 8th bit) and then shift
 that value 7 times to the low-order bit to print.  Yes, it could be
 the other way...  */
void fs_printfreemask(void) {
    int i,j;
    
    for (i=0; i < fsd.freemaskbytes; i++) {
        for (j=0; j < 8; j++) {
            printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
        }
        if ( (i % 8) == 7) {
            printf("\n");
        }
    }
    printf("\n");
}


int fs_open(char *filename, int flags) {
    if (flags == O_RDONLY ||flags == O_WRONLY || flags ==  O_RDWR )
    {
        int i=0,j;
        for (i=0; i<fsd.root_dir.numentries; i++) {
            if(strncmp(fsd.root_dir.entry[i].name,filename, strlen(filename))==0)
                break;
        }
        if (i == fsd.root_dir.numentries)
        {
            printf("File is not present;\n");
            return SYSERR;
        }
        for (j=0; j<NUM_FD; j++) {
            if(strncmp(fsd.root_dir.entry[i].name,oft[j].de->name, strlen(oft[j].de->name))==0)
            {
                if (oft[j].state == FSTATE_OPEN) {
                    printf("File is alreay opened.\n");
                    return SYSERR;
                }
                else
                {
                    struct inode node;
                    int get_inode_status = fs_get_inode_by_num(0, oft[j].in.id, &node);
                    
                    // Checking if error in completion of fs_get_inode_by_num
                    if(get_inode_status == SYSERR)
                    {
                        printf("fs_open :: Error In fs_get_inode_by_num\n");
                        return SYSERR;
                    }
                    oft[j].fileptr = 0;
                    oft[j].de = &fsd.root_dir.entry[i];
                    oft[j].in = node;
                    oft[j].state = FSTATE_OPEN;
                    return j;
                }
            }
        }
        printf("File is not present in file table");
        return SYSERR;
    }
    printf("Value of Flags should be read only, write only or read and write mode.");
    return SYSERR;
    
}

int fs_close(int fd) {
    if(fd < 0 || fd >= NUM_FD)
    {
        printf("Invalid File descriptor.\n");
        return SYSERR;
    }
    if(oft[fd].state == FSTATE_CLOSED)
    {
        printf("File is already closed.\n");
        return SYSERR;
    }
    // Setting file state to be close
    oft[fd].state = FSTATE_CLOSED;
    oft[fd].fileptr = 0;
    return OK;
   
}

int fs_create(char *filename, int mode) {
    int i=0;
    if(mode != O_CREAT)
    {
        printf("Invalid Mode for creation of a file\n");
        return SYSERR;
    }
    if(fsd.inodes_used < fsd.ninodes)
    {
        while (i <= fsd.root_dir.numentries) {
            if(strncmp(fsd.root_dir.entry[i].name,filename, strlen(filename))==0)
            {
                printf("\n File name is already present");
                return SYSERR;
            }
            i++;
        }
        i=fsd.root_dir.numentries;
        
        struct inode node;
        node.id= ++fsd.inodes_used;
        node.type = INODE_TYPE_FILE;
        node.device = dev0;
        
        int status = fs_put_inode_by_num(dev0, node.id,&node);
        if(status == OK)
        {
            //Add entry into directory
            
            strcpy(fsd.root_dir.entry[i].name,filename);
            fsd.root_dir.entry[i].inode_num = fsd.inodes_used;
            
            oft[fsd.inodes_used].state = FSTATE_OPEN;
            oft[fsd.inodes_used].fileptr = 0;
            oft[fsd.inodes_used].de = &fsd.root_dir.entry[fsd.root_dir.numentries++];
            oft[fsd.inodes_used].in = node;
            
            //Add entry into file table
            return fsd.inodes_used;
        }
        
    }
    // No inode is empty
    return SYSERR;
}

int fs_seek(int fd, int offset) {
    if(fd < 0 || fd >= NUM_FD)
    {
        printf("Invalid File descriptor\n");
        return SYSERR;
    }
    
    if(oft[fd].state == FSTATE_CLOSED)
    {
        printf("File is closed.\n");
        return SYSERR;
    }
    
    oft[fd].fileptr += offset;
    
    return oft[fd].fileptr;
}

int fs_read(int fd, void *buf, int nbytes) {
    if(fd < 0 || fd >= NUM_FD)
    {
        printf("Invalid File descriptor\n");
        return SYSERR;
    }
    // Throw error if file is in closed state
    if(oft[fd].state == FSTATE_CLOSED)
    {
        printf(" File is closed.\n");
        return SYSERR;
    }
    
    if(nbytes <=0)
    {
        printf("Invalid Length Of Read Buffer\n");
        return SYSERR;
    }
    
    if(oft[fd].in.size == 0)
    {
        printf("Empty File to Read\n");
        return SYSERR;
    }
    int startBlock= (oft[fd].fileptr/ MDEV_BLOCK_SIZE);
    int offset = oft[fd].fileptr % MDEV_BLOCK_SIZE;
    int remainingBytes= nbytes;
    memset(buf,NULL,nbytes);
    int blockNum;
    int blockCount = oft[fd].in.size/ MDEV_BLOCK_SIZE ;
    if ((oft[fd].in.size % MDEV_BLOCK_SIZE)>0)
        blockCount ++;
    int start=0;
    memset(buf, '\0', (MDEV_BLOCK_SIZE * MDEV_NUM_BLOCKS));
    for (blockNum=startBlock; blockNum<blockCount && remainingBytes >0; blockNum++) {
        int minBytes = remainingBytes > MDEV_BLOCK_SIZE ? MDEV_BLOCK_SIZE: remainingBytes;
        if (blockNum == startBlock) {
            if(bs_bread(dev0,oft[fd].in.blocks[blockNum],offset,buf,minBytes) ==SYSERR)
            {
                printf("Error in reading a block");
                return SYSERR;
            }
        }
        else
        {
            if(bs_bread(dev0,oft[fd].in.blocks[blockNum],0,buf+start,minBytes) ==SYSERR)
            {
                printf("Error in reading a block");
                return SYSERR;
            }
        }
        remainingBytes-=minBytes;
        start += minBytes;
    }
    oft[fd].fileptr +=start;
    return start;
}

int fs_write(int fd, void *buf, int nbytes) {
    if(nbytes == 0)
    {
        printf("\nZero Length File..Write Failed\n");
        return SYSERR;
    }
    int i = 0;
    if(fd < 0 || fd >= NUM_FD)
    {
        printf("\nWrite Failed..Invalid File");
        return SYSERR;
    }
    
    if(oft[fd].state == FSTATE_CLOSED)
    {
        printf("\nFile not open for writing");
        return SYSERR;
    }
    int blockCount=0;
    //clear previous block if present
    if (oft[fd].in.size>0) {
        
        blockCount = oft[fd].in.size/ MDEV_BLOCK_SIZE ;
        if ((oft[fd].in.size % MDEV_BLOCK_SIZE)>0)
            blockCount ++;
        for (i=0; i<blockCount; i++) {
            fs_clearmaskbit(oft[fd].in.blocks[i]);
        }
    }
    blockCount= nbytes /MDEV_BLOCK_SIZE;
    if ((nbytes % MDEV_BLOCK_SIZE)>0) {
        blockCount++;
    }
    int blockNum=0;
    int remainingBytes=nbytes;
    int current_inode_block=0;
    for (blockNum = FIRST_INODE_BLOCK+NUM_INODE_BLOCKS; (current_inode_block<blockCount) && (blockNum<fsd.nblocks); blockNum++) {
        if (fs_getmaskbit(blockNum)==0) {
            // Free block
           
            int minBytes = remainingBytes > MDEV_BLOCK_SIZE ? MDEV_BLOCK_SIZE: remainingBytes;
            if(bs_bwrite(dev0,blockNum,0,buf,minBytes) ==SYSERR)
            {
                printf("Error in writing a block");
                return SYSERR;
            }
            
            remainingBytes-=minBytes;
            buf =(char*) buf+ minBytes;
            fs_setmaskbit(blockNum);
            oft[fd].in.blocks[current_inode_block++]=blockNum;
        }
    }
    oft[fd].in.size=nbytes;
    int status=fs_put_inode_by_num(dev0,oft[fd].in.id,&oft[fd].in);
    if (status ==SYSERR)
        return SYSERR;
    oft[fd].fileptr=nbytes;
    return nbytes;
}

#endif /* FS */
