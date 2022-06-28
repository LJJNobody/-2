/*
 *  linux/kernel/sys.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <errno.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>

int sys_ftime()
{
	return -ENOSYS;
}

int sys_break()
{
	return -ENOSYS;
}

int sys_ptrace()
{
	return -ENOSYS;
}

int sys_stty()
{
	return -ENOSYS;
}

int sys_gtty()
{
	return -ENOSYS;
}

int sys_rename()
{
	return -ENOSYS;
}

int sys_prof()
{
	return -ENOSYS;
}

int sys_setregid(int rgid, int egid)
{
	if (rgid>0) {
		if ((current->gid == rgid) || 
		    suser())
			current->gid = rgid;
		else
			return(-EPERM);
	}
	if (egid>0) {
		if ((current->gid == egid) ||
		    (current->egid == egid) ||
		    suser()) {
			current->egid = egid;
			current->sgid = egid;
		} else
			return(-EPERM);
	}
	return 0;
}

int sys_setgid(int gid)
{
/*	return(sys_setregid(gid, gid)); */
	if (suser())
		current->gid = current->egid = current->sgid = gid;
	else if ((gid == current->gid) || (gid == current->sgid))
		current->egid = gid;
	else
		return -EPERM;
	return 0;
}

int sys_acct()
{
	return -ENOSYS;
}

int sys_phys()
{
	return -ENOSYS;
}

int sys_lock()
{
	return -ENOSYS;
}

int sys_mpx()
{
	return -ENOSYS;
}

int sys_ulimit()
{
	return -ENOSYS;
}

int sys_time(long * tloc)
{
	int i;

	i = CURRENT_TIME;
	if (tloc) {
		verify_area(tloc,4);
		put_fs_long(i,(unsigned long *)tloc);
	}
	return i;
}

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
int sys_setreuid(int ruid, int euid)
{
	int old_ruid = current->uid;
	
	if (ruid>0) {
		if ((current->euid==ruid) ||
                    (old_ruid == ruid) ||
		    suser())
			current->uid = ruid;
		else
			return(-EPERM);
	}
	if (euid>0) {
		if ((old_ruid == euid) ||
                    (current->euid == euid) ||
		    suser()) {
			current->euid = euid;
			current->suid = euid;
		} else {
			current->uid = old_ruid;
			return(-EPERM);
		}
	}
	return 0;
}

int sys_setuid(int uid)
{
/*	return(sys_setreuid(uid, uid)); */
	if (suser())
		current->uid = current->euid = current->suid = uid;
	else if ((uid == current->uid) || (uid == current->suid))
		current->euid = uid;
	else
		return -EPERM;
	return(0);
}

int sys_stime(long * tptr)
{
	if (!suser())
		return -EPERM;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies/HZ;
	return 0;
}

int sys_times(struct tms * tbuf)
{
	if (tbuf) {
		verify_area(tbuf,sizeof *tbuf);
		put_fs_long(current->utime,(unsigned long *)&tbuf->tms_utime);
		put_fs_long(current->stime,(unsigned long *)&tbuf->tms_stime);
		put_fs_long(current->cutime,(unsigned long *)&tbuf->tms_cutime);
		put_fs_long(current->cstime,(unsigned long *)&tbuf->tms_cstime);
	}
	return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&
	    end_data_seg < current->start_stack - 16384)
		current->brk = end_data_seg;
	return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
		pid = current->pid;
	if (!pgid)
		pgid = current->pid;
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->pid==pid) {
			if (task[i]->leader)
				return -EPERM;
			if (task[i]->session != current->session)
				return -EPERM;
			task[i]->pgrp = pgid;
			return 0;
		}
	return -ESRCH;
}

int sys_getpgrp(void)
{
	return current->pgrp;
}

int sys_setsid(void)
{
	if (current->leader && !suser())
		return -EPERM;
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = -1;
	return current->pgrp;
}

int sys_getgroups()
{
	return -ENOSYS;
}

int sys_setgroups()
{
	return -ENOSYS;
}

int sys_uname(struct utsname * name)
{
	static struct utsname thisname = {
		"linux .0","nodename","release ","version ","machine "
	};
	int i;

	if (!name) return -ERROR;
	verify_area(name,sizeof *name);
	for(i=0;i<sizeof *name;i++)
		put_fs_byte(((char *) &thisname)[i],i+(char *) name);
	return 0;
}

int sys_sethostname()
{
	return -ENOSYS;
}

int sys_getrlimit()
{
	return -ENOSYS;
}

int sys_setrlimit()
{
	return -ENOSYS;
}

int sys_getrusage()
{
	return -ENOSYS;
}

int sys_gettimeofday()
{
	return -ENOSYS;
}

int sys_settimeofday()
{
	return -ENOSYS;
}


int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}

int sys_sleep(unsigned int seconds){
	if(seconds<0)return -1;
	sys_signal(SIGALRM,SIG_IGN);
	sys_alarm(seconds);
	if(sys_pause()==-1){
		return -1;
	}
	return 0;
}

long sys_getcwd(char * buf, size_t size)
{
    struct m_inode *inode = current->pwd; 
    struct buffer_head *path_head = bread(current->root->i_dev, inode->i_zone[0]);
    struct dir_entry *getpath = (struct dir_entry *)path_head->b_data; 
    unsigned short lst_inode;
    struct m_inode *new_inode;
    char* pathname[512]; 
    char *ans;
    
    int i = 0;
    while(1) {
        lst_inode = getpath->inode;
        new_inode = iget(current->root->i_dev, (getpath + 1)->inode);
        path_head = bread(current->root->i_dev, new_inode->i_zone[0]);
        getpath = (struct dir_entry*) path_head->b_data;
        int j = 1;
        while(1) {
            if ((getpath + j)->inode == lst_inode){
                break;
            }
            j++;
        }
        if (j == 1){
            break;
        }
        pathname[i] = (getpath + j)->name;
        i++;
    }

    int count = 0;
    i--;
    int k;
    if (i < 0){
        return NULL;
    }
    while (i >= 0){
        k = 0;
        ans[count++] = '/';
        while(pathname[i][k] != '\0')
        {
            ans[count] = pathname[i][k];
            k++;
            count++;
        }
        i--;
    }
    if (count < 0){
        return NULL;
    }
    for (k = 0; k < count; k++){
        put_fs_byte(ans[k], buf+k);
    }
    return (long)(ans);
}

struct linux_dirent
{
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[14];
};

int sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count){
    struct linux_dirent lastdirent;
    struct m_inode *inode = current->filp[fd]->f_inode;
    struct buffer_head *path_head = bread(inode->i_dev, inode->i_zone[0]);
    struct dir_entry *path = (struct dir_entry *)path_head->b_data;
    int bytes_num = 0;
    int i, j, k;
    for (i = 0; i < 1024; i++){
        if (path->inode == 0 || (i+1) * 24 > count){
            break;
        }
        lastdirent.d_ino = path[i].inode;
        for (j = 0; j < 14; j++){
            lastdirent.d_name[j] = path[i].name[j];
        }
        lastdirent.d_off = 0;
        lastdirent.d_reclen = 24; 
        for (k = 0; k < 24; k++){
            put_fs_byte(((char*)&lastdirent)[k], ((char*)dirp+bytes_num));
            bytes_num++;
        }
    }
    if (bytes_num == 24){
        return 0;
    }
    return bytes_num;
}

// int sys_execve2(const char *path, char * argv[], char * envp[]){
// 	return 0;
// }

int sys_none(){
	return 0;
}