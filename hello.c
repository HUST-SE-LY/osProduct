#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/kfifo.h>
dev_t devno;
int major = 255;
const char DEVNAME[] = "hello_device";
int in=0;
int out =0;
int * innum=&in;
int * outnum=&out;
char KFIFO[32]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',};
#define BUFFER_SIZE 32
char var1[32]={};
char var2[32]={};
static DECLARE_WAIT_QUEUE_HEAD(read_queue);
static DECLARE_WAIT_QUEUE_HEAD(write_queue);
static int isfull(size_t count)
{
 int k;
 int flag=1;
 for(k=in;k<in+count;k++)
    {if(KFIFO[k%32]!='0')
        flag=0;
    } 
    return flag;
}
static int isempty(size_t count)
{
 int flag=1;
if((in-out)<count) flag=0;
 return flag;
}
bool special(int *innum,int *outnum,size_t count)
{  if((*innum-*outnum)>=count)
       return true;
    else return false;
}

int hello_open(struct inode * ip, struct file * fp)
{
    printk("%s : %d\n", __func__, __LINE__);
    
    /* 一般用来做初始化设备的操作 */
    return 0;
}

int hello_close(struct inode * ip, struct file * fp)
{
    printk("%s : %d\n", __func__, __LINE__);
    
    /* 一般用来做和open相反的操作，open申请资源，close释放资源 */
    return 0;
}



ssize_t hello_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
  int ret;
  int j;
  int y;
  int f=isempty(count);
  printk("开始运行%d\n",current->pid);
   if(!f) 
   { printk("阻塞%d\n",current->pid);
   wait_event_interruptible(read_queue, special(innum,outnum,count));
   printk("被唤醒%d\n",current->pid);
   }
   
      for(y=0;y<count;y++)
    var2[y] = KFIFO[(out+y)%32];
  ret=copy_to_user(buf,var2, count);
    out=out+count;
    for(j=out-count;j<out;j++)
     KFIFO[j%32]='0';
    printk("out=%d\n",out);
    printk("outnext%s\n",KFIFO);
    wake_up_interruptible(&write_queue);
    return ret;
    
}

ssize_t hello_write(struct file *file, const char __user *buf, size_t count, loff_t *loft)
{
   
    int ret;
   int u;
    int f=isfull(count);
   printk("开始运行%d\n",current->pid);
   if(!f) 
   {printk("isfull=%d",isfull(count));
    printk("阻塞%d\n",current->pid);
   wait_event_interruptible(write_queue, isfull(count)==1);
   printk("被唤醒%d\n",current->pid);
   
   }
   
   ret=copy_from_user(var1, buf, count);
   for(u=0;u<count;u++)
     KFIFO[(in+u)%32]=var1[u];
   in=in+count;
     printk("in=%d\n",in);
     printk("innext%s\n",KFIFO);
     wake_up_interruptible(&read_queue);
    return ret;
}

/* file_operations sturct */
struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open  = hello_open,
    .release = hello_close,
    .read = hello_read,
    .write = hello_write,
};
struct cdev cdev;

static int hello_init(void)
{
    int ret;
    printk("%s : %d\n", __func__, __LINE__);
    
    /* 1. 生成并注册设备号 */
    devno = MKDEV(major, 0);
    ret  = register_chrdev_region(devno, 1, DEVNAME);
    if (ret != 0)
    {
        printk("%s : %d fail to register_chrdev_region\n", __func__, __LINE__);
        return -1;
    }
    
    /* 3. 分配、设置、注册cdev结构体 */
    cdev.owner = THIS_MODULE;
    ret = cdev_add(&cdev, devno, 1);
    cdev_init(&cdev, &hello_fops);
    if (ret < 0)
    {
        printk("%s : %d fail to cdev_add\n", __func__, __LINE__);
        return -1;
    }
    printk("success!\n");
    return 0;
}

static void hello_exit(void)
{
    printk("%s : %d\n", __func__, __LINE__);
      
    /* 释放资源 */
    cdev_del(&cdev);
    unregister_chrdev_region(devno, 1);
}

MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);
