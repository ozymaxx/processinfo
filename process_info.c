#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/stat.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <asm/pgtable.h>

#define BUFSIZE 100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ozan Can Altiok, Metin Can Siper, Umut Utku Calis");

static int pid = 0;

module_param( pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC( pid, "PID of the process");

static int __init processinfo_init(void) {
	printk( KERN_INFO "CS342 Project 4: Kernel Module\n");
	printk( KERN_INFO "Starting module...\n");
	
	struct task_struct *task = current;
	struct task_struct *desiredTask = NULL;
	
	for_each_process( task) {
		if ( task->pid == pid) {
			desiredTask = task;
		}
	}
	
	if ( desiredTask != NULL) {
		printk( KERN_INFO "--A process is found with the PID = %d--\n", pid);
		printk( KERN_INFO "--The curently opened files information--\n");
		struct fdtable *filesTable;
		struct path fPath;
		char *filePath;
		char *buffer = (char *) kmalloc( GFP_KERNEL, BUFSIZE * sizeof( char) );
		
		filesTable = files_fdtable( desiredTask->files);
		
		int i = 0;
		while ( filesTable->fd[i]) {
			fPath = filesTable->fd[i]->f_path;
			filePath = d_path( &fPath, buffer, BUFSIZE * sizeof( char) );
			printk( KERN_INFO "\t%s\n", filePath);
			i++;
		}
		
		printk( KERN_INFO "--Memory Management Information--\n" );

		struct mm_struct* mm = desiredTask->mm;
		printk( KERN_INFO "[CODE START]\t[CODE END]\t[CODE SIZE]\n");
		printk( KERN_INFO "%lx\t\t%lx\t%lu\n", mm->start_code, 
			mm->end_code, mm->end_code - mm->start_code );
			
		printk( KERN_INFO "[DATA START]\t[DATA END]\t[DATA SIZE]\n");
		printk( KERN_INFO "%lx\t\t%lx\t%lu\n\n", mm->start_data, 
			mm->end_data, mm->end_data - mm->start_data );
		
		printk( KERN_INFO "\nNotice: The stack data will be written in virtual memory part.\n" );
		
		printk( KERN_INFO "[ARG START]\t[ARG END]\t[ARG SIZE]\n");
		printk( KERN_INFO "%lx\t\t%lx\t%lu\n\n", mm->arg_start, 
			mm->arg_end, mm->arg_end - mm->arg_start );
		
		printk( KERN_INFO "[ENV START]\t[ENV END]\t[ENV SIZE]\n");
		printk( KERN_INFO "%lx\t\t%lx\t%lu\n\n", mm->env_start, 
			mm->env_end, mm->env_end - mm->env_start );
		
		printk( KERN_INFO "Total VM area = %lu\n", mm->total_vm);
		printk( KERN_INFO "Number of frames used by the process = %lu\n\n", get_mm_rss( mm) );

		struct vm_area_struct *mmap = mm->mmap;
		
		printk( KERN_INFO "--Virtual Memory Information--\n" );
		printk( KERN_INFO "[VM START]\t[VM_END]\t[VM_SIZE]");
		while( mmap != NULL )
		{
			if( mmap -> vm_next == NULL ) {
				printk( KERN_INFO "\nThe stack information of the process:\n");
				printk( KERN_INFO "[STACK START]\t[STACK END]\t[STACK SIZE]\n" );
			}
			printk( KERN_INFO "%lx\t\t%lx\t%lu\n", mmap -> vm_start, 
				mmap -> vm_end, mmap -> vm_end - mmap -> vm_start );
			mmap = mmap -> vm_next;
		}
		
		printk( KERN_INFO "--The filesystem information--\n");
		struct fs_struct *filesStruct = desiredTask->fs;
		printk( KERN_INFO "\tRoot: %s\n", filesStruct->root.dentry->d_name.name);
		printk( KERN_INFO "\tWorking Directory: %s\n", filesStruct->pwd.dentry->d_name.name);
		
		/* -- THIS PART IS ABOUT THE PAGE TABLE INFORMATION
		 * -- BUT THERE ARE SOME STRANGE ERRORS
		 * -- SO WE COMMENTED THIS PART
		printk( KERN_INFO "--Page Table Information--\n");
		pgd_t *PGD;
		pud_t *PUD;
		pmd_t *PMD;
		pte_t *PTE;
		struct page *processPage = NULL;
		int j, k, l, m;
		
		//printk( KERN_INFO "Totalhighpages = %llu\n", totalhigh_pages);
		//printk( KERN_INFO "Pud Size = %llu\n", sizeof( pgd_t) );
		for ( m = 0; m < totalhigh_pages; m++ ) {
			PGD = pgd_offset( mm, sizeof( pgd_t) * m);
			if ( !pgd_none( *PGD) && !pgd_bad( *PGD) ) {
				for ( j = 0; j < totalhigh_pages; j++ ) {
					PUD = pud_offset( PGD, sizeof( pud_t) * j);
					if ( !pud_none( *PUD) && !pud_bad( *PUD) ) {
						for ( k = 0; k < totalhigh_pages; k++ ) {
							PMD = pmd_offset( PUD, sizeof( pmd_t) * k);
							if ( !pmd_none( *PMD) && !pmd_bad( *PMD) ) {
								for ( l = 0; l < totalhigh_pages; l++ ) {
									PTE = pte_offset_map( PMD, sizeof( pte_t) * l);
									if ( PTE) {
										processPage = pte_page( *PTE);
										if ( processPage) {
											printk( KERN_INFO "\tPage frame struct address = %p", processPage);
										}
										pte_unmap( PTE);
									}
								}
							}
						}
					}
				}
			}
		}
		
		if ( !processPage) {
			printk( KERN_INFO "There is no page table information.\n");
		} 
		*/
	}
	else {
		printk( KERN_INFO "There is not a process with PID = %d, exiting...\n", pid);
	}
	
	return 0;
}

static void __exit processinfo_exit( void) {
	printk( KERN_INFO "The module successfully removed.\n");
}

module_init( processinfo_init);
module_exit( processinfo_exit)
