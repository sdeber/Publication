
inline void kernelthread_wait(int index) {
   static struct sembuf semosleep[1]={{0,-1,0}};
   int result;

//   fprintf(stderr,"Kernel thread %d is dozing off\n",index);
   result = semop(sched.sem_id[index], semosleep,1);

   // error checking
   if (result<0) {
     switch (errno) {
        case EACCES : perror("EACCES ");exit(0);
        case E2BIG : perror("E2BIG ");exit(0);
        case EAGAIN : perror("EAGAIN ");exit(0);
        case EFAULT : perror("EFAULT ");exit(0);
        case EFBIG : perror("EFBIG ");exit(0);
        case EIDRM : perror ("EIDRM");exit(0);
        case EINTR : perror("EINTR ");exit(0);
        case EINVAL : perror("EINVAL ");exit(0);
        case ENOMEM : perror("ENOMEM ");exit(0);
        case ERANGE : perror("ERANGE ");exit(0);
     } // end of switch
     fprintf(stderr,"%d failed on semop\n",THREAD_SELF->id);
     exit(0);
   } else { // Thread is woken up
 //      fprintf(stderr,"Kernel thread %d has woken up\n",index);
       //jump here
       sched_jmp(index);
   }
}

