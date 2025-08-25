// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"
#define NBUCKETS 13  // 桶数量（质数，减少哈希冲突）
// 哈希函数：将blockno映射到桶索引

struct {
    struct spinlock lock[NBUCKETS];  // 每个桶的锁（保护本桶的链表）
    struct buf buf[NBUF];            // 所有缓冲区（全局数组）
    // 每个桶的双向链表：按最近使用（LRU）排序，head.next为最近使用
    struct buf head[NBUCKETS];       
} bcache;
int hash(uint blockno) {
    return blockno % NBUCKETS;
}
void
binit(void)
{
  struct buf *b;
    // 初始化每个桶的锁
    for (int i = 0; i < NBUCKETS; i++) {
        initlock(&bcache.lock[i], "bcache");  // 锁名以"bcache"开头
    }
    // 初始化每个桶的双向链表（空链表：head.prev = head.next = &head）
    for (int i = 0; i < NBUCKETS; i++) {
        bcache.head[i].prev = &bcache.head[i];
        bcache.head[i].next = &bcache.head[i];
    }
    // 将所有缓冲区初始加入第0个桶的链表
    for (b = bcache.buf; b < bcache.buf + NBUF; b++) {
        initsleeplock(&b->lock, "buffer");  // 初始化缓冲区自身的睡眠锁
        // 插入到第0个桶的链表头部（最近使用位置）
        b->next = bcache.head[0].next;
        b->prev = &bcache.head[0];
        bcache.head[0].next->prev = b;
        bcache.head[0].next = b;
    }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  int id = hash(blockno);

  acquire(&bcache.lock[id]);

  // Is the block already cached?
  for (b = bcache.head[id].next; b != &bcache.head[id]; b = b->next)
  {
    if (b->dev == dev && b->blockno == blockno)
    {
      b->refcnt++;
      release(&bcache.lock[id]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  int i = id;
  while (1)
  {
    i = (i + 1) % NBUCKETS;
    if (i == id) // 防止死循环
      continue;
    acquire(&bcache.lock[i]);
    for (b = bcache.head[i].prev; b != &bcache.head[i]; b = b->prev)
    {
      if (b->refcnt == 0)
      {
        b->dev = dev;
        b->blockno = blockno;
        b->valid = 0;
        b->refcnt = 1;

        b->prev->next = b->next; // 断开当前缓冲区的链表连接
        b->next->prev = b->prev;

        release(&bcache.lock[i]);

        b->prev = &bcache.head[id]; // 将缓冲区插入到新的位置
        b->next = bcache.head[id].next;
        b->next->prev = b;
        b->prev->next = b;
        release(&bcache.lock[id]);
        acquiresleep(&b->lock);
        return b;
      }
    }
    release(&bcache.lock[i]);
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if (!holdingsleep(&b->lock))
    panic("brelse");
  int id = hash(b->blockno);
  releasesleep(&b->lock);

  acquire(&bcache.lock[id]);
  b->refcnt--;
  if (b->refcnt == 0)
  {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head[id].next;
    b->prev = &bcache.head[id];
    bcache.head[id].next->prev = b;
    bcache.head[id].next = b;
  }

  release(&bcache.lock[id]);
}

void
bpin(struct buf *b) {
  int id = hash(b->blockno);
  acquire(&bcache.lock[id]);
  b->refcnt++;
  release(&bcache.lock[id]);
}

void
bunpin(struct buf *b) {
  int id = hash(b->blockno);
  acquire(&bcache.lock[id]);
  b->refcnt--;
  release(&bcache.lock[id]);
}


